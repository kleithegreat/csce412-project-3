#ifndef LOADBALANCER_H
#define LOADBALANCER_H

#include "RequestQueue.h"
#include "WebServer.h"
#include "IPRange.h"
#include <vector>
#include <string>
#include <fstream>

class LoadBalancer {
private:
    RequestQueue queue;
    std::vector<WebServer*> servers;
    std::vector<IPRange> blocked_ranges;
    std::ofstream log_file;

    int clock_cycle;
    int total_cycles;
    int num_servers;
    int initial_servers;
    int next_server_id;

    int min_task_time;
    int max_task_time;
    int scale_cooldown;
    int cooldown_remaining;

    int total_requests_created;
    int total_requests_completed;
    int total_requests_blocked;
    int initial_queue_size;
    int max_queue_size;
    int servers_added;
    int servers_removed;

    bool isBlocked(const std::string& ip) const;

    void distributeRequests();

    void scaleServers();

    void addServer();

    void removeServer();

    void logMessage(const std::string& message);

public:
    LoadBalancer(int num_servers, int total_cycles, int min_task_time,
                 int max_task_time, int scale_cooldown,
                 const std::string& log_filename = "lb_log.txt");

    ~LoadBalancer();

    void addBlockedRange(const std::string& low, const std::string& high);

    bool addRequest(const Request& r);

    void generateInitialQueue();

    void run();

    void printSummary();
};

#endif