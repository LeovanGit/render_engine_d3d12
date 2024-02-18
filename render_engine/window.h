#pragma once

#include <windows.h>
#include <stdint.h>

class Window
{
public:
    Window(
        HINSTANCE instanceHandle,
        WNDPROC windowProc,
        int cmdShow,
        uint16_t posX,
        uint16_t posY,
        uint16_t clientWidth,
        uint16_t clientHeight);

private:
    void CreateWindowClass(HINSTANCE instanceHandle, WNDPROC windowProc);

public:
    HWND m_windowHandle;
    uint16_t m_posX;
    uint16_t m_posY;
    uint16_t m_clientWidth;
    uint16_t m_clientHeight;
};