
#pragma once

#include <chrono>

namespace Time
{
    using Clock = std::chrono::steady_clock;
    using TimePoint = Clock::time_point;
    using Duration = Clock::duration;

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
