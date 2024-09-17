#pragma once

#include <chrono>

inline auto start_time = std::chrono::high_resolution_clock::now();

inline double get_sec_elapsed() {

    auto current_time = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(current_time - start_time).count()
           * 1e-9;
}
