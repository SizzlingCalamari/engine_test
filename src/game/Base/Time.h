
#pragma once

#include <chrono>

namespace Time
{
    using TimePoint = std::chrono::high_resolution_clock::time_point;
    using Duration = std::chrono::high_resolution_clock::duration;

    Duration FromSeconds(float seconds);

    TimePoint Now();

    class StopWatch
    {
    public:
        Duration Reset();

    private:
        TimePoint mStartTick;
    };
}
