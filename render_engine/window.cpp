#include "window.h"

Window::Window(
    HINSTANCE instanceHandle,
    WNDPROC windowProc,
    int cmdShow,
    uint16_t posX,
    uint16_t posY,
    uint16_t clientWidth,
    uint16_t clientHeight)
    : m_posX(posX)
    , m_posY(posY)
    , m_clientWidth(clientWidth)
    , m_clientHeight(clientHeight)
{
    CreateWindowClass(instanceHandle, windowProc);

    // calculate real window size (with borders) from client area size:
    RECT windowSize = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, false);

    m_windowHandle = CreateWindowEx(
        NULL,
        L"WindowClass1",
        L"WindowTitle",
        WS_OVERLAPPEDWINDOW,
        m_posX,
        m_posY,
        windowSize.right - windowSize.left,
        windowSize.bottom - windowSize.top,
        NULL,
        NULL,
        instanceHandle,
        NULL);

    ShowWindow(m_windowHandle, cmdShow);
}

void Window::CreateWindowClass(HINSTANCE instanceHandle, WNDPROC windowProc)
{
    WNDCLASSEX windowClass;
    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = instanceHandle;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    windowClass.lpszClassName = L"WindowClass1";

    RegisterClassEx(&windowClass);
}
