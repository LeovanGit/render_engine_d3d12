#include "timer.h"

Timer::Timer()
    : m_prevTimePoint(Clock::now())
    , m_deltaTime(0.0f)
{
    
}

bool Timer::IsElapsed(float durationInSeconds)
{
    using namespace std::chrono;

    TimePoint timePoint = Clock::now();
    auto elapsed = duration_cast<Seconds>(timePoint - m_prevTimePoint);

    if (elapsed.count() < durationInSeconds) return false;
        
    m_deltaTime = elapsed;
    m_prevTimePoint = timePoint;

    return true;
}

float Timer::GetDeltaTime() const
{
    return m_deltaTime.count();
}
