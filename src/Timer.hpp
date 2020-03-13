#pragma once

#include <chrono>

class Timer
{
    long long m_start = 0;
    long long m_end = 0;
    std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;

public:

    Timer()
    {
        start();
    }

    void start()
    {
        m_startTimepoint = std::chrono::high_resolution_clock::now();
    }

    long long elapsed()
    {
        auto endTimepoint = std::chrono::high_resolution_clock::now();

        m_start = std::chrono::time_point_cast<std::chrono::microseconds>(m_startTimepoint).time_since_epoch().count();
        m_end = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch().count();

        return m_end - m_start;
    }
};