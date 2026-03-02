/**
 * @file RequestQueue.h
 * @brief Declaration of the RequestQueue class for managing a FIFO queue of requests.
 */

#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include "Request.h"
#include <queue>

/**
 * @brief A FIFO queue wrapper for managing incoming network requests.
 *
 * Provides enqueue, dequeue, size, and empty-check operations
 * on top of std::queue<Request>.
 */
class RequestQueue {
private:
    /** @brief Internal STL queue holding the requests. */
    std::queue<Request> q;

public:
    /**
     * @brief Adds a request to the back of the queue.
     * @param r The Request to enqueue.
     */
    void enqueue(Request r);

    /**
     * @brief Removes and returns the request at the front of the queue.
     * @return The front Request.
     */
    Request dequeue();

    /**
     * @brief Returns the current number of requests in the queue.
     * @return The queue size as an int.
     */
    int size();

    /**
     * @brief Checks whether the queue is empty.
     * @return True if the queue contains no requests, false otherwise.
     */
    bool isEmpty();
};

#endif
