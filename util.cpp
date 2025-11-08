#include "util.h"
#include <string>

int parseInt(const std::string& s, int defaultVal) {
    try {
        if (s.empty()) return defaultVal;
        return std::stoi(s);
    } catch (...) {
        return defaultVal;
    }
}

