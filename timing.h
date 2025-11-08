#ifndef TIMING_H
#define TIMING_H

#include <chrono>
#include <iostream>
#include <string>

template<typename Func>
auto timedCall(const std::string& label, Func&& f) -> decltype(f()) {
    auto start  = std::chrono::high_resolution_clock::now();
    auto result = f();
    auto end    = std::chrono::high_resolution_clock::now();

    auto us = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    double ms = us / 1000.0;

    std::cout << "[TIMER] " << label << " took " << ms << " ms\n";

    return result;
}

#endif