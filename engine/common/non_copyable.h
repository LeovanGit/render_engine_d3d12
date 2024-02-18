#pragma once

class NonCopyable
{
public:
    NonCopyable() {}
    ~NonCopyable() {}

    NonCopyable(const NonCopyable &other) = delete;
    void operator=(const NonCopyable &other) = delete;

    NonCopyable(const NonCopyable &&other) = delete;
    void operator=(const NonCopyable &&other) = delete;
};