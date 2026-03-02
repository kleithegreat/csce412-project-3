/**
 * @file LoadBalancer.cpp
 * @brief Implementation of the LoadBalancer class methods.
 */

#include "LoadBalancer.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iomanip>

/** @brief ANSI escape code to reset terminal color. */
#define CLR_RESET   "\033[0m"
/** @brief ANSI escape code for red text. */
#define CLR_RED     "\033[31m"
/** @brief ANSI escape code for green text. */
#define CLR_GREEN   "\033[32m"
/** @brief ANSI escape code for yellow text. */
#define CLR_YELLOW  "\033[33m"
/** @brief ANSI escape code for blue text. */
#define CLR_BLUE    "\033[34m"
/** @brief ANSI escape code for cyan text. */
#define CLR_CYAN    "\033[36m"
/** @brief ANSI escape code for bold text. */
#define CLR_BOLD    "\033[1m"

/**
 * @brief Constructs a LoadBalancer, allocates the initial server pool, and opens the log file.
 * @param num_servers Initial number of web servers.
 * @param total_cycles Total simulation clock cycles to run.
 * @param min_task_time Minimum task processing time.
 * @param max_task_time Maximum task processing time.
 * @param scale_cooldown Cooldown cycles between scaling operations.
 * @param log_filename Name of the log file (defaults to "lb_log.txt").
 */
LoadBalancer::LoadBalancer(int num_servers, int total_cycles, int min_task_time,
                           int max_task_time, int scale_cooldown,
                           const std::string& log_filename)
    : clock_cycle(0), total_cycles(total_cycles), num_servers(num_servers),
      initial_servers(num_servers), next_server_id(num_servers),
      min_task_time(min_task_time), max_task_time(max_task_time),
      scale_cooldown(scale_cooldown), cooldown_remaining(0),
      total_requests_created(0), total_requests_completed(0),
      total_requests_blocked(0), initial_queue_size(0),
      max_queue_size(0), servers_added(0), servers_removed(0) {
    for (int i = 0; i < num_servers; i++) {
        servers.push_back(new WebServer(i));
    }

    log_file.open(log_filename);
    if (!log_file.is_open()) {
        std::cerr << "Error: couldn't open log file: " << log_filename << std::endl;
    }
}

/**
 * @brief Destructor. Frees all dynamically allocated servers and closes the log file.
 */
LoadBalancer::~LoadBalancer() {
    for (auto* server : servers) {
        delete server;
    }
    servers.clear();
    if (log_file.is_open()) {
        log_file.close();
    }
}

/**
 * @brief Adds an IP range to the blocked list (firewall).
 * @param low Lower bound IP address (inclusive).
 * @param high Upper bound IP address (inclusive).
 */
void LoadBalancer::addBlockedRange(const std::string& low, const std::string& high) {
    blocked_ranges.emplace_back(low, high);
}

/**
 * @brief Checks whether an IP address falls within any blocked range.
 * @param ip The IPv4 address string to check.
 * @return True if the IP is blocked, false otherwise.
 */
bool LoadBalancer::isBlocked(const std::string& ip) const {
    for (const auto& r : blocked_ranges) {
        if (r.contains(ip)) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Attempts to add a request to the queue, blocking it if the source IP is firewalled.
 * @param r The Request to add.
 * @return True if the request was enqueued, false if it was blocked.
 */
bool LoadBalancer::addRequest(const Request& r) {
    if (isBlocked(r.ip_in)) {
        total_requests_blocked++;
        logMessage("BLOCKED request from " + r.ip_in);
        return false;
    }
    queue.enqueue(r);
    total_requests_created++;
    if (queue.size() > max_queue_size) {
        max_queue_size = queue.size();
    }
    return true;
}

/**
 * @brief Generates the initial request queue with num_servers * 100 random requests.
 */
void LoadBalancer::generateInitialQueue() {
    int count = num_servers * 100;
    logMessage("Generating an initial queue of " + std::to_string(count) + " requests");

    for (int i = 0; i < count; i++) {
        Request r = Request::generateRandom(min_task_time, max_task_time);
        addRequest(r);
    }

    initial_queue_size = queue.size();
    logMessage("Initial queue size: " + std::to_string(initial_queue_size));
}

/**
 * @brief Distributes pending requests from the queue to all idle servers.
 */
void LoadBalancer::distributeRequests() {
    for (auto* server : servers) {
        if (!server->isBusy() && !queue.isEmpty()) {
            Request r = queue.dequeue();
            server->assignRequest(r);

            std::ostringstream oss;
            oss << "Assigned request ";
            r.print(oss);
            oss << " to Server " << server->getId();
            logMessage(oss.str());
        }
    }
}

/**
 * @brief Evaluates queue pressure and scales the server pool up or down.
 *
 * If the queue size exceeds 80 * server_count, a new server is added.
 * If the queue size falls below 50 * server_count, an idle server is removed.
 * Scaling operations are subject to a cooldown period.
 */
void LoadBalancer::scaleServers() {
    if (cooldown_remaining > 0) {
        cooldown_remaining--;
        return;
    }

    int q_size = queue.size();
    int lower_bound = 50 * static_cast<int>(servers.size());
    int upper_bound = 80 * static_cast<int>(servers.size());

    if (q_size > upper_bound && servers.size() > 0) {
        addServer();
        servers_added++;
        cooldown_remaining = scale_cooldown;

        std::ostringstream oss;
        oss << "SCALING UP: Queue=" << q_size << " > " << upper_bound
            << " | Added Server " << (next_server_id - 1)
            << " | Total servers: " << servers.size();
        logMessage(oss.str());

    } else if (q_size < lower_bound && servers.size() > 1) {
        removeServer();
        servers_removed++;
        cooldown_remaining = scale_cooldown;

        std::ostringstream oss;
        oss << "SCALING DOWN: Queue=" << q_size << " < " << lower_bound
            << " | Removed a server"
            << " | Total servers: " << servers.size();
        logMessage(oss.str());
    }
}

/**
 * @brief Allocates a new WebServer with an auto-incremented ID and adds it to the pool.
 */
void LoadBalancer::addServer() {
    servers.push_back(new WebServer(next_server_id++));
}

/**
 * @brief Removes an idle server from the pool. If all servers are busy, removes the last one.
 */
void LoadBalancer::removeServer() {
    for (int i = servers.size() - 1; i >= 0; i--) {
        if (!servers[i]->isBusy()) {
            delete servers[i];
            servers.erase(servers.begin() + i);
            return;
        }
    }
    if (!servers.empty()) {
        delete servers.back();
        servers.pop_back();
    }
}

/**
 * @brief Logs a message to the console with ANSI color coding and to the log file.
 *
 * Color is determined by keywords in the message:
 * - "BLOCKED" -> red
 * - "SCALING UP/DOWN" -> yellow
 * - "COMPLETED" -> green
 * - "STATUS" -> cyan
 * - "SUMMARY" -> bold
 *
 * @param message The message string to log.
 */
void LoadBalancer::logMessage(const std::string& message) {
    std::string color = CLR_RESET;
    if (message.find("BLOCKED") != std::string::npos)       color = CLR_RED;
    else if (message.find("SCALING UP") != std::string::npos)   color = CLR_YELLOW;
    else if (message.find("SCALING DOWN") != std::string::npos) color = CLR_YELLOW;
    else if (message.find("COMPLETED") != std::string::npos)    color = CLR_GREEN;
    else if (message.find("STATUS") != std::string::npos)       color = CLR_CYAN;
    else if (message.find("SUMMARY") != std::string::npos)      color = CLR_BOLD;

    std::cout << color << "[Cycle " << clock_cycle << "] " << message << CLR_RESET << std::endl;

    if (log_file.is_open()) {
        log_file << "[Cycle " << clock_cycle << "] " << message << std::endl;
    }
}

/**
 * @brief Runs the main simulation loop.
 *
 * Each clock cycle:
 * 1. Generates 0-2 random new requests.
 * 2. Distributes queued requests to idle servers.
 * 3. Ticks each server (advancing their processing).
 * 4. Evaluates auto-scaling.
 * 5. Logs a status update every 1000 cycles.
 */
void LoadBalancer::run() {
    logMessage("Load Balancer Starting");
    logMessage("Servers: " + std::to_string(num_servers) +
               " | Cycles: " + std::to_string(total_cycles) +
               " | Task time: " + std::to_string(min_task_time) + "-" +
               std::to_string(max_task_time) +
               " | Scale cooldown: " + std::to_string(scale_cooldown));
    logMessage("Blocked IP ranges: " + std::to_string(blocked_ranges.size()));

    generateInitialQueue();

    for (clock_cycle = 1; clock_cycle <= total_cycles; clock_cycle++) {
        int new_requests = rand() % 3;
        for (int i = 0; i < new_requests; i++) {
            Request r = Request::generateRandom(min_task_time, max_task_time);
            addRequest(r);
        }

        distributeRequests();

        for (auto* server : servers) {
            bool finished = server->tick();
            if (finished) {
                total_requests_completed++;
                std::ostringstream oss;
                oss << "Server " << server->getId() << " COMPLETED request ";
                server->getCurrentRequest().print(oss);
                logMessage(oss.str());
            }
        }

        scaleServers();

        if (clock_cycle % 1000 == 0) {
            int busy_count = 0;
            for (auto* s : servers) if (s->isBusy()) busy_count++;

            std::ostringstream oss;
            oss << "--- STATUS: Cycle " << clock_cycle
                << " | Queue: " << queue.size()
                << " | Servers: " << servers.size()
                << " (busy: " << busy_count
                << ", idle: " << (servers.size() - busy_count) << ")"
                << " | Completed: " << total_requests_completed
                << " | Blocked: " << total_requests_blocked;
            logMessage(oss.str());
        }
    }

    logMessage("Simulation done!");
    printSummary();
}

/**
 * @brief Prints a summary of all simulation statistics to the console and log file.
 *
 * Includes initial/final server counts, scaling events, queue metrics,
 * task time range, request counts, and active/idle server breakdown.
 */
void LoadBalancer::printSummary() {
    int busy_count = 0;
    int idle_count = 0;
    for (auto* s : servers) {
        if (s->isBusy()) busy_count++;
        else idle_count++;
    }

    std::ostringstream summary;
    summary << "\nSUMMARY:\n"
            << "Initial servers: " << initial_servers << "\n"
            << "Final servers: " << servers.size() << "\n"
            << "Servers added: " << servers_added << "\n"
            << "Servers removed: " << servers_removed << "\n"
            << "Starting queue: " << initial_queue_size << "\n"
            << "Ending queue: " << queue.size() << "\n"
            << "Peak queue: " << max_queue_size << "\n"
            << "Task time range: " << min_task_time << " - " << max_task_time << "\n"
            << "Total cycles: " << total_cycles << "\n"
            << "Requests created: " << total_requests_created << "\n"
            << "Requests completed: " << total_requests_completed << "\n"
            << "Requests blocked: " << total_requests_blocked << "\n"
            << "Active servers: " << busy_count << "\n"
            << "Idle servers: " << idle_count << "\n"
            << "Remaining in queue: " << queue.size() << "\n";

    std::cout << summary.str() << std::endl;

    if (log_file.is_open()) log_file << summary.str() << std::endl;
}
