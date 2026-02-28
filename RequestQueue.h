#ifndef REQUESTQUEUE_H
#define REQUESTQUEUE_H

#include "Request.h"
#include <queue>

class RequestQueue {
private:
    std::queue<Request> q;

public:
    void enqueue(Request r);

    Request dequeue();

    int size();

    bool isEmpty();
};

#endif