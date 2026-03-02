#include "Request.h"
#include <cstdlib>
#include <sstream>
#include <iomanip>

Request::Request() : ip_in(""), ip_out(""), time(0), job_type('P') {}

Request::Request(std::string ip_in, std::string ip_out, int time, char job_type)
    : ip_in(ip_in), ip_out(ip_out), time(time), job_type(job_type) {}


static std::string randomIP() {
    return std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256) + "." +
           std::to_string(rand() % 256);
}

Request Request::generateRandom(int min_time, int max_time) {
    std::string in = randomIP();
    std::string out = randomIP();
    int t = min_time + (rand() % (max_time - min_time + 1));
    char type = (rand() % 2 == 0) ? 'P' : 'S';
    return Request(in, out, t, type);
}

void Request::print(std::ostream& os) {
    os << "[" << ip_in << " -> " << ip_out
       << " | time=" << time
       << " | type=" << job_type << "]";
}