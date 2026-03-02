/**
 * @file Request.h
 * @brief Declaration of the Request struct representing a network request.
 */

#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <iostream>

/**
 * @brief Represents a single network request with source/destination IPs, processing time, and job type.
 */
struct Request {
    /** @brief Source IP address of the request. */
    std::string ip_in;

    /** @brief Destination IP address of the request. */
    std::string ip_out;

    /** @brief Processing time required for this request (in clock cycles). */
    int time;

    /** @brief Job type identifier ('P' for processing, 'S' for streaming). */
    char job_type;

    /**
     * @brief Default constructor. Initializes an empty request with time 0 and job type 'P'.
     */
    Request();

    /**
     * @brief Parameterized constructor.
     * @param ip_in Source IP address.
     * @param ip_out Destination IP address.
     * @param time Processing time in clock cycles.
     * @param job_type Job type character ('P' or 'S').
     */
    Request(std::string ip_in, std::string ip_out, int time, char job_type);

    /**
     * @brief Generates a random request with a random source/destination IP and random time.
     * @param min_time Minimum processing time.
     * @param max_time Maximum processing time.
     * @return A randomly generated Request.
     */
    static Request generateRandom(int min_time, int max_time);

    /**
     * @brief Prints the request details to the given output stream.
     * @param os Output stream to print to (defaults to std::cout).
     */
    void print(std::ostream& os = std::cout);
};

#endif
