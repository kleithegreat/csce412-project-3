/**
 * @file IPRange.cpp
 * @brief Implementation of the IPRange struct methods.
 */

#include "IPRange.h"
#include <sstream>

/**
 * @brief Constructs an IPRange with the given low and high bounds.
 * @param low Lower bound IP address string.
 * @param high Upper bound IP address string.
 */
IPRange::IPRange(std::string low, std::string high)
    : low(low), high(high) {}

/**
 * @brief Converts a dotted-decimal IPv4 address string to a 32-bit numeric value.
 *
 * Parses each octet separated by '.' and shifts them into a single unsigned long.
 *
 * @param ip The IPv4 address string (e.g., "192.168.1.1").
 * @return The numeric (unsigned long) representation of the IP address.
 */
unsigned long IPRange::ipToNum(std::string ip) {
    unsigned long result = 0;
    std::istringstream ss(ip);
    std::string octet;
    for (int i = 0; i < 4; i++) {
        std::getline(ss, octet, '.');
        result = (result << 8) + std::stoul(octet);
    }
    return result;
}

/**
 * @brief Checks whether a given IP address falls within this range (inclusive).
 * @param ip The IPv4 address string to check.
 * @return True if the IP is within [low, high], false otherwise.
 */
bool IPRange::contains(std::string ip) const {
    unsigned long num = ipToNum(ip);
    return num >= ipToNum(low) && num <= ipToNum(high);
}
