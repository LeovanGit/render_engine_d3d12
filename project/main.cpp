#include <windows.h>

#include <string>

#include "source\window.h"
#include "source\d3d.h"
#include "source\timer.h"

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

namespace
{
constexpr float k_frameTime = 1.0f / 60.0f;
} // namespace

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

    Timer timer;

    MSG msg;
    while (true)
    {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) goto exit;
        }

        if (timer.IsElapsed(k_frameTime))
        {
            float delta_time = timer.GetDeltaTime();
            float fps = 1.0f / delta_time;

            std::wstring windowText =
                L"fps: " + std::to_wstring(fps) +
                L"\ndt: " + std::to_wstring(delta_time);

            RECT rect = { 2, 2, 74, 48 };
            DrawText(
                GetDC(window.m_windowHandle),
                windowText.c_str(),
                windowText.size(),
                &rect,
                DT_LEFT | DT_TOP);
        }
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