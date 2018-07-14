
#include "Time.h"

Time::Duration Time::FromSeconds(float seconds)
{
    using FloatSec = std::chrono::duration<float>;
    return(std::chrono::duration_cast<Duration>(FloatSec(seconds)));
}

Time::TimePoint Time::Now()
{
    return(Clock::now());
}

Time::Duration Time::StopWatch::Reset()
{
    const TimePoint now = Time::Now();
    const Duration elapsed = (now - mStartTick);
    mStartTick = now;
    return(elapsed);
}
