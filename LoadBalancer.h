/**
 * @file LoadBalancer.h
 * @brief Declaration of the LoadBalancer class that orchestrates request distribution and server scaling.
 */

#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "RequestQueue.h"
#include "WebServer.h"
#include "IPRange.h"
#include <vector>
#include <string>
#include <fstream>

/**
 * @brief Core load balancer that distributes incoming requests across a dynamic pool of web servers.
 *
 * The LoadBalancer manages a RequestQueue, a vector of WebServer pointers, and a list of
 * blocked IP ranges. It supports automatic scaling (adding/removing servers based on queue
 * pressure), IP-based firewall blocking, and colored console logging with file output.
 */
class LoadBalancer {
private:
    /** @brief Queue of pending requests awaiting assignment. */
    RequestQueue queue;

    /** @brief Dynamic pool of web server pointers. */
    std::vector<WebServer*> servers;

    /** @brief List of blocked IP address ranges for the firewall. */
    std::vector<IPRange> blocked_ranges;

    /** @brief Output file stream for logging simulation events. */
    std::ofstream log_file;

    /** @brief Current clock cycle in the simulation. */
    int clock_cycle;

    /** @brief Total number of clock cycles to run. */
    int total_cycles;

    /** @brief Current number of active servers. */
    int num_servers;

    /** @brief Number of servers at simulation start. */
    int initial_servers;

    /** @brief ID to assign to the next dynamically created server. */
    int next_server_id;

    /** @brief Minimum task processing time (in clock cycles). */
    int min_task_time;

    /** @brief Maximum task processing time (in clock cycles). */
    int max_task_time;

    /** @brief Number of cycles to wait between consecutive scaling operations. */
    int scale_cooldown;

    /** @brief Remaining cooldown cycles before the next scaling operation is allowed. */
    int cooldown_remaining;

    /** @brief Total number of requests created during the simulation. */
    int total_requests_created;

    /** @brief Total number of requests completed during the simulation. */
    int total_requests_completed;

    /** @brief Total number of requests blocked by the IP firewall. */
    int total_requests_blocked;

    /** @brief Size of the request queue after initial generation. */
    int initial_queue_size;

    /** @brief Peak queue size observed during the simulation. */
    int max_queue_size;

    /** @brief Total number of servers dynamically added during the simulation. */
    int servers_added;

    /** @brief Total number of servers dynamically removed during the simulation. */
    int servers_removed;

    /**
     * @brief Checks whether an IP address is in any blocked range.
     * @param ip The IPv4 address string to check.
     * @return True if the IP is blocked, false otherwise.
     */
    bool isBlocked(const std::string& ip) const;

    /**
     * @brief Distributes pending requests from the queue to idle servers.
     */
    void distributeRequests();

    /**
     * @brief Evaluates queue pressure and scales the server pool up or down.
     */
    void scaleServers();

    /**
     * @brief Adds a new server to the pool.
     */
    void addServer();

    /**
     * @brief Removes an idle server from the pool (or the last server if all are busy).
     */
    void removeServer();

    /**
     * @brief Logs a message to both the console (with color) and the log file.
     * @param message The message string to log.
     */
    void logMessage(const std::string& message);

public:
    /**
     * @brief Constructs a LoadBalancer with the given configuration parameters.
     * @param num_servers Initial number of web servers.
     * @param total_cycles Total simulation clock cycles to run.
     * @param min_task_time Minimum task processing time.
     * @param max_task_time Maximum task processing time.
     * @param scale_cooldown Cooldown cycles between scaling operations.
     * @param log_filename Name of the log file (defaults to "lb_log.txt").
     */
    LoadBalancer(int num_servers, int total_cycles, int min_task_time,
                 int max_task_time, int scale_cooldown,
                 const std::string& log_filename = "lb_log.txt");

    /**
     * @brief Destructor. Frees all dynamically allocated servers and closes the log file.
     */
    ~LoadBalancer();

    /**
     * @brief Adds an IP range to the blocked list (firewall).
     * @param low Lower bound IP address (inclusive).
     * @param high Upper bound IP address (inclusive).
     */
    void addBlockedRange(const std::string& low, const std::string& high);

    /**
     * @brief Attempts to add a request to the queue, blocking it if the source IP is firewalled.
     * @param r The Request to add.
     * @return True if the request was enqueued, false if it was blocked.
     */
    bool addRequest(const Request& r);

    /**
     * @brief Generates the initial request queue (num_servers * 100 requests).
     */
    void generateInitialQueue();

    /**
     * @brief Runs the main simulation loop for total_cycles clock cycles.
     */
    void run();

    /**
     * @brief Prints a summary of simulation statistics to the console and log file.
     */
    void printSummary();
};

#endif
