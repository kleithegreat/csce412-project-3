#ifndef REQUEST_H
#define REQUEST_H

#include <string>
#include <iostream>

struct Request {
    std::string ip_in;
    std::string ip_out;
    int time;
    char job_type;

    Request();

    Request(std::string ip_in, std::string ip_out, int time, char job_type);

    static Request generateRandom(int min_time, int max_time);

    void print();
};

#endif