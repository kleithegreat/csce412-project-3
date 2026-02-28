#ifndef IPRANGE_H
#define IPRANGE_H

#include <string>

struct IPRange {
    std::string low;
    std::string high;

    IPRange(std::string low, std::string high);

    bool contains(std::string ip) const;

    static unsigned long ipToNum(std::string ip);
};

#endif