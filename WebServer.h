/**
 * @file WebServer.h
 * @brief Declaration of the WebServer class representing a single server in the load balancer pool.
 */

#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

/**
 * @brief Represents a single web server that processes one request at a time.
 *
 * Each server has a unique ID, tracks its busy/idle state, counts down
 * remaining processing time, and records how many requests it has handled.
 */
class WebServer {
private:
    /** @brief Unique identifier for this server. */
    int id;

    /** @brief Whether the server is currently processing a request. */
    bool busy;

    /** @brief Clock cycles remaining to finish the current request. */
    int remaining_time;

    /** @brief The request currently being processed. */
    Request current_request;

    /** @brief Total number of requests this server has completed. */
    int requests_handled;

public:
    /**
     * @brief Constructs a WebServer with the given ID.
     * @param id Unique server identifier.
     */
    WebServer(int id);

    /**
     * @brief Assigns a new request to this server for processing.
     * @param r The Request to process.
     */
    void assignRequest(Request r);

    /**
     * @brief Advances the server's clock by one cycle.
     *
     * Decrements remaining_time. If the request finishes, marks the
     * server as idle and increments the handled count.
     *
     * @return True if a request was completed during this tick, false otherwise.
     */
    bool tick();

    /**
     * @brief Checks whether the server is currently processing a request.
     * @return True if busy, false if idle.
     */
    bool isBusy();

    /**
     * @brief Returns the unique ID of this server.
     * @return The server ID.
     */
    int getId();

    /**
     * @brief Returns the total number of requests this server has completed.
     * @return The count of handled requests.
     */
    int getRequestsHandled();

    /**
     * @brief Returns the request currently assigned to this server.
     * @return The current Request (may be default-constructed if idle).
     */
    Request getCurrentRequest();
};

#endif
