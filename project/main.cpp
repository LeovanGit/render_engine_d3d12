#include <windows.h>

#include "source\window.h"
#include "source\d3d.h"

LRESULT CALLBACK WindowProc(
    HWND windowHandle,
    UINT message,
    WPARAM wParam,
    LPARAM lParam);

// for std::cout
void initConsole()
{
    AllocConsole();
    FILE *dummy;
    auto s = freopen_s(&dummy, "CONOUT$", "w", stdout);
}

int WINAPI WinMain(
    HINSTANCE instanceHandle,
    HINSTANCE prevInstanceHandle, // outdated thing
    LPSTR cmdLine,
    int cmdShow)
{
    initConsole();

    Direct3D::Init();

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
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) goto exit;
        }

        // game code
    }

exit:
    Direct3D::Deinit();

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