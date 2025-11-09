#include "util.h"
#include <string>

//inspired by https://docs.python.org/3/library/csv.html
//https://stackoverflow.com/questions/1120140/how-can-i-read-and-parse-csv-files-in-c
int parseInt(const std::string& s, int defaultVal) {
    try {
        if (s.empty()) return defaultVal;
        return std::stoi(s);
    } catch (...) {
        return defaultVal;
    }
}

