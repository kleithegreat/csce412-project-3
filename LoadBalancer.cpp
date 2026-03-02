#include "LoadBalancer.h"
#include <iostream>
#include <sstream>
#include <cstdlib>
#include <algorithm>
#include <iomanip>

#define CLR_RESET   "\033[0m"
#define CLR_RED     "\033[31m"
#define CLR_GREEN   "\033[32m"
#define CLR_YELLOW  "\033[33m"
#define CLR_BLUE    "\033[34m"
#define CLR_CYAN    "\033[36m"
#define CLR_BOLD    "\033[1m"

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

LoadBalancer::~LoadBalancer() {
    for (auto* server : servers) {
        delete server;
    }
    servers.clear();
    if (log_file.is_open()) {
        log_file.close();
    }
}

void LoadBalancer::addBlockedRange(const std::string& low, const std::string& high) {
    blocked_ranges.emplace_back(low, high);
}

bool LoadBalancer::isBlocked(const std::string& ip) const {
    for (const auto& r : blocked_ranges) {
        if (r.contains(ip)) {
            return true;
        }
    }
    return false;
}

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

void LoadBalancer::addServer() {
    servers.push_back(new WebServer(next_server_id++));
}

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