/**
 * @file Request.cpp
 * @brief Implementation of the Request struct methods.
 */

#include "Request.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>

/**
 * @brief Default constructor. Initializes fields to empty strings, time 0, and job type 'P'.
 */
Request::Request() : ip_in(""), ip_out(""), time(0), job_type('P') {}

/**
 * @brief Parameterized constructor.
 * @param ip_in Source IP address.
 * @param ip_out Destination IP address.
 * @param time Processing time in clock cycles.
 * @param job_type Job type character ('P' or 'S').
 */
Request::Request(std::string ip_in, std::string ip_out, int time, char job_type)
    : ip_in(ip_in), ip_out(ip_out), time(time), job_type(job_type) {}

/**
 * @brief Generates a random IPv4 address string (e.g., "192.168.1.42").
 * @return A randomly generated IP address string.
 */
static std::string randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

/**
 * @brief Generates a random request with random IPs, a random time within the given range, and a random job type.
 * @param min_time Minimum processing time.
 * @param max_time Maximum processing time.
 * @return A randomly generated Request.
 */
Request Request::generateRandom(int min_time, int max_time) {
    std::string in = randomIP();
    std::string out = randomIP();
    int t = min_time + (rand() % (max_time - min_time + 1));
    char type = (rand() % 2 == 0) ? 'P' : 'S';
    return Request(in, out, t, type);
}

/**
 * @brief Prints the request in the format [ip_in -> ip_out | time=X | type=Y].
 * @param os Output stream to print to (defaults to std::cout).
 */
void Request::print(std::ostream& os) {
    os << "[" << ip_in << " -> " << ip_out
       << " | time=" << time
       << " | type=" << job_type << "]";
}
