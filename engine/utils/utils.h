#pragma once

template<typename T>
inline T alignUp(T value, T alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}
