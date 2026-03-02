/**
 * @file main.cpp
 * @brief Entry point for the Load Balancer simulation.
 *
 * @mainpage CSCE 412 Project 3 - Load Balancer
 *
 * @section intro_sec Introduction
 * This project implements a **load balancer simulation** for CSCE 412. The simulator
 * models a dynamic pool of web servers that process incoming network requests distributed
 * by a central load balancer.
 *
 * @section features_sec Key Features
 * - **Dynamic Server Scaling:** The load balancer automatically adds or removes servers
 *   based on queue pressure thresholds, with a configurable cooldown period between
 *   scaling operations.
 * - **IP Firewall:** Incoming requests can be blocked based on configurable IP address
 *   ranges, simulating a basic firewall.
 * - **Configurable Parameters:** All simulation parameters (number of servers, cycle count,
 *   task time range, scaling cooldown, blocked IP ranges, log file name) can be set via
 *   a configuration file or interactive input.
 * - **Colored Logging:** Console output uses ANSI color codes to highlight different event
 *   types (blocked requests in red, scaling events in yellow, completed requests in green,
 *   status updates in cyan, summaries in bold). All events are also written to a log file.
 *
 * @section arch_sec Architecture
 * - **Request** – A struct representing a network request with source/destination IPs,
 *   processing time, and job type.
 * - **RequestQueue** – A FIFO queue wrapper for managing pending requests.
 * - **WebServer** – Represents a single server that processes one request at a time.
 * - **IPRange** – Defines an IP address range for firewall blocking.
 * - **LoadBalancer** – The core orchestrator that distributes requests, manages the server
 *   pool, handles auto-scaling, and performs logging.
 *
 * @section usage_sec Usage
 * @code
 * ./loadbalancer [config_file]
 * @endcode
 * If no config file is provided, the program looks for `config.txt`. If that is not found,
 * it falls back to interactive input.
 */

#include "LoadBalancer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <map>
#include <vector>
#include <string>

/**
 * @brief Trims leading and trailing whitespace from a string.
 * @param s The input string to trim.
 * @return A new string with whitespace removed from both ends.
 */
std::string trim(std::string s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    int end = s.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    return s.substr(start, end - start + 1);
}

/**
 * @brief Parses a configuration file for simulation parameters and blocked IP ranges.
 *
 * The config file uses `key = value` syntax. Lines starting with '#' are comments.
 * The special key `blocked_range` expects a value in the format `low_ip - high_ip`.
 *
 * @param filename Path to the configuration file.
 * @param config Output map of key-value configuration parameters.
 * @param blocked_ranges Output vector of blocked IP range pairs (low, high).
 * @return True if the file was opened and parsed successfully, false otherwise.
 */
static bool parseConfig(std::string filename,
                        std::map<std::string, std::string>& config,
                        std::vector<std::pair<std::string, std::string>>& blocked_ranges) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty() || line[0] == '#') continue;

        size_t eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = trim(line.substr(0, eq));
        std::string val = trim(line.substr(eq + 1));

        if (key == "blocked_range") {
            size_t dash = val.find('-');
            if (dash != std::string::npos) {
                std::string low = trim(val.substr(0, dash));
                std::string high = trim(val.substr(dash + 1));
                blocked_ranges.push_back({low, high});
            }
        } else config[key] = val;
    }
    return true;
}

/**
 * @brief Main entry point for the load balancer simulation.
 *
 * Reads configuration from a file (or interactive input), initializes the
 * LoadBalancer with the specified parameters and blocked IP ranges, and
 * runs the simulation.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings. argv[1] is an optional config file path.
 * @return 0 on successful completion.
 */
int main(int argc, char* argv[]) {
    srand(time(nullptr));

    int num_servers = 10;
    int total_cycles = 10000;
    int min_task_time = 5;
    int max_task_time = 50;
    int scale_cooldown = 20;
    std::string log_filename = "lb_log.txt";
    std::vector<std::pair<std::string, std::string>> blocked_ranges;

    std::string config_file = (argc > 1) ? argv[1] : "config.txt";
    std::map<std::string, std::string> config;

    if (parseConfig(config_file, config, blocked_ranges)) {
        std::cout << "Loaded config from: " << config_file << "\n\n";
        if (config.count("num_servers")) num_servers = std::stoi(config["num_servers"]);
        if (config.count("total_cycles")) total_cycles = std::stoi(config["total_cycles"]);
        if (config.count("min_task_time")) min_task_time = std::stoi(config["min_task_time"]);
        if (config.count("max_task_time")) max_task_time = std::stoi(config["max_task_time"]);
        if (config.count("scale_cooldown")) scale_cooldown = std::stoi(config["scale_cooldown"]);
        if (config.count("log_file")) log_filename = config["log_file"];
    } else {
        std::cout << "No config file found. Using interactive input.\n\n";
        std::cout << "Enter number of servers: ";
        std::cin >> num_servers;
        std::cout << "Enter total clock cycles: ";
        std::cin >> total_cycles;
        std::cout << "Enter minimum task time: ";
        std::cin >> min_task_time;
        std::cout << "Enter maximum task time: ";
        std::cin >> max_task_time;
        std::cout << "Enter scale cooldown (cycles): ";
        std::cin >> scale_cooldown;
        blocked_ranges.push_back({"10.0.0.0", "10.0.0.255"});
    }

    LoadBalancer lb(num_servers, total_cycles, min_task_time, max_task_time, scale_cooldown, log_filename);

    for (const auto& r : blocked_ranges) lb.addBlockedRange(r.first, r.second);

    lb.run();

    return 0;
}
