#include <windows.h>

#include "window.h"

LRESULT CALLBACK WindowProc(
    HWND windowHandle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

int WINAPI WinMain(
    HINSTANCE instanceHandle,
    HINSTANCE prevInstanceHandle, // outdated thing
    LPSTR cmdLine,
    int cmdShow)
{
    Window window(
        instanceHandle,
        WindowProc,
        cmdShow,
        500,
        200,
        800,
        600);

    MSG msg;
    while (true)
    {
        // while there are messages in the queue
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg); // translate keystroke messages into the right format
            DispatchMessage(&msg); // send the message to the WindowProc()

            if (msg.message == WM_QUIT) goto exit;
        }

        // game code
    }

exit:
    return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WindowProc(
    HWND windowHandle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    switch (message)
    {
    case WM_DESTROY:
    {
        PostQuitMessage(0); // send WM_QUIT

        return 0;
    }
    }

    return DefWindowProc(windowHandle, message, wParam, lParam);
}