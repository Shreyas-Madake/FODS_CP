#pragma once
#include <chrono>

/**
 * @brief RAII timer helper for precise execution time measurement.
 */
class Timer {
public:
    Timer() : m_start(std::chrono::high_resolution_clock::now()) {}

    void reset() {
        m_start = std::chrono::high_resolution_clock::now();
    }

    double elapsedMs() const {
        auto now = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = now - m_start;
        return elapsed.count();
    }

private:
    std::chrono::high_resolution_clock::time_point m_start;
};
