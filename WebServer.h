#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "Request.h"

class WebServer {
private:
    int id;
    bool busy;
    int remaining_time;
    Request current_request;
    int requests_handled;

public:
    WebServer(int id);

    void assignRequest(Request r);

    bool tick();

    bool isBusy();

    int getId();

    int getRequestsHandled();

    Request getCurrentRequest();
};

#endif