/**
 * @file WebServer.cpp
 * @brief Implementation of the WebServer class methods.
 */

#include "WebServer.h"

/**
 * @brief Constructs a WebServer with the given ID in an idle state.
 * @param id Unique server identifier.
 */
WebServer::WebServer(int id)
    : id(id), busy(false), remaining_time(0), requests_handled(0) {}

/**
 * @brief Assigns a new request to this server and marks it as busy.
 * @param r The Request to process.
 */
void WebServer::assignRequest(Request r) {
    current_request = r;
    remaining_time = r.time;
    busy = true;
}

/**
 * @brief Advances the server's clock by one cycle.
 *
 * If the server is busy, decrements remaining_time. When remaining_time
 * reaches zero, the server becomes idle and the handled count is incremented.
 *
 * @return True if a request was completed during this tick, false otherwise.
 */
bool WebServer::tick() {
    if (!busy) return false;

    remaining_time--;
    if (remaining_time <= 0) {
        busy = false;
        requests_handled++;
        return true;
    }
    return false;
}

/**
 * @brief Checks whether the server is currently processing a request.
 * @return True if busy, false if idle.
 */
bool WebServer::isBusy() {
    return busy;
}

/**
 * @brief Returns the unique ID of this server.
 * @return The server ID.
 */
int WebServer::getId() {
    return id;
}

/**
 * @brief Returns the total number of requests this server has completed.
 * @return The count of handled requests.
 */
int WebServer::getRequestsHandled() {
    return requests_handled;
}

/**
 * @brief Returns the request currently assigned to this server.
 * @return The current Request (may be default-constructed if idle).
 */
Request WebServer::getCurrentRequest() {
    return current_request;
}
