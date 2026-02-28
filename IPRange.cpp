#include "IPRange.h"
#include <sstream>

IPRange::IPRange(std::string low, std::string high)
    : low(low), high(high) {}

unsigned long IPRange::ipToNum(std::string ip) {
    unsigned long result = 0;
    std::istringstream ss(ip);
    std::string octet;
    for (int i = 0; i < 4; i++) {
        std::getline(ss, octet, '.');
        result = (result << 8) + std::stoul(octet);
    }
    return result;
}

bool IPRange::contains(std::string ip) const {
    unsigned long num = ipToNum(ip);
    return num >= ipToNum(low) && num <= ipToNum(high);
}