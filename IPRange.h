/**
 * @file IPRange.h
 * @brief Declaration of the IPRange struct for representing a range of blocked IP addresses.
 */

#ifndef IPRANGE_H
#define IPRANGE_H

#include <string>

/**
 * @brief Represents a contiguous range of IPv4 addresses used for IP-based firewall blocking.
 *
 * An IPRange is defined by a low and high IP address (inclusive).
 * It provides a method to check whether a given IP falls within the range.
 */
struct IPRange {
    /** @brief Lower bound of the IP range (inclusive). */
    std::string low;

    /** @brief Upper bound of the IP range (inclusive). */
    std::string high;

    /**
     * @brief Constructs an IPRange with the given low and high bounds.
     * @param low Lower bound IP address string (e.g., "10.0.0.0").
     * @param high Upper bound IP address string (e.g., "10.0.0.255").
     */
    IPRange(std::string low, std::string high);

    /**
     * @brief Checks whether a given IP address falls within this range.
     * @param ip The IPv4 address string to check.
     * @return True if the IP is within [low, high], false otherwise.
     */
    bool contains(std::string ip) const;

    /**
     * @brief Converts a dotted-decimal IPv4 address string to a numeric value.
     * @param ip The IPv4 address string (e.g., "192.168.1.1").
     * @return The numeric (unsigned long) representation of the IP address.
     */
    static unsigned long ipToNum(std::string ip);
};

#endif
