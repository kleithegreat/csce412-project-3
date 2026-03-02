/**
 * @file RequestQueue.cpp
 * @brief Implementation of the RequestQueue class methods.
 */

#include "RequestQueue.h"

/**
 * @brief Adds a request to the back of the queue.
 * @param r The Request to enqueue.
 */
void RequestQueue::enqueue(Request r) {
    q.push(r);
}

/**
 * @brief Removes and returns the request at the front of the queue.
 * @return The front Request.
 */
Request RequestQueue::dequeue() {
    Request front = q.front();
    q.pop();
    return front;
}

/**
 * @brief Returns the current number of requests in the queue.
 * @return The queue size as an int.
 */
int RequestQueue::size() {
    return static_cast<int>(q.size());
}

/**
 * @brief Checks whether the queue is empty.
 * @return True if the queue contains no requests, false otherwise.
 */
bool RequestQueue::isEmpty() {
    return q.empty();
}
