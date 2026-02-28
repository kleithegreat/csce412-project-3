#include "WebServer.h"

WebServer::WebServer(int id)
    : id(id), busy(false), remaining_time(0), requests_handled(0) {}

void WebServer::assignRequest(Request r) {
    current_request = r;
    remaining_time = r.time;
    busy = true;
}

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

bool WebServer::isBusy() {
    return busy;
}

int WebServer::getId() {
    return id;
}

int WebServer::getRequestsHandled() {
    return requests_handled;
}

Request WebServer::getCurrentRequest() {
    return current_request;
}