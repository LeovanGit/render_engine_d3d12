#pragma once

#include <iostream>
#include <cassert>
#include <chrono>

namespace
{
using Clock = std::chrono::steady_clock;
using TimePoint = std::chrono::time_point<Clock>;
using Seconds = std::chrono::duration<float>;
} // namespace

class Timer
{
public:
    Timer();

    bool IsElapsed(float durationInSeconds);

    float GetDeltaTime() const;

    float GetFPS() const;

private:
    TimePoint m_prevTimePoint;
    Seconds m_deltaTime;
};
