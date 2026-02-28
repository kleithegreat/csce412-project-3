#include "RequestQueue.h"

void RequestQueue::enqueue(Request r) {
    q.push(r);
}

Request RequestQueue::dequeue() {
    Request front = q.front();
    q.pop();
    return front;
}

int RequestQueue::size() {
    return static_cast<int>(q.size());
}

bool RequestQueue::isEmpty() {
    return q.empty();
}