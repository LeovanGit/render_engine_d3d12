#include <windows.h>

#include <string>

#include "source\engine.h"
#include "source\window.h"
#include "source\renderer.h"
#include "source\timer.h"

#include "application.h"

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

    Engine::Init();

    Window window(
        instanceHandle,
        WindowProc,
        cmdShow,
        500,
        200,
        800,
        600);

    Renderer renderer(&window);

    Timer timer;

    Application application;
    application.InitScene(renderer);
    renderer.Init(); // call it after Application::InitScene()

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
            std::wstring windowText = L"fps: " + std::to_wstring(1.0f / timer.GetDeltaTime());
            SetWindowText(window.m_windowHandle, windowText.c_str());

            renderer.Render();
        }
    }

exit:
    Engine::Deinit();

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
    //case WM_ACTIVATE:
    //{
    //    if (LOWORD(wParam) == WA_INACTIVE) // pause game or decrease fps
    //    else // unpause game
    //}
    //case WM_SIZE:
    // 1. resize back buffers to new client area size: IDXGISwapChain::ResizeBuffers()
    // 2. destroy and recreate depth stencil buffer with new client area size
    // 3. recreate depth stencil view and render taget view
 
    // WARNING: we should use WM_EXITSIZEMOVE instead of WM_SIZE and WM_MOVE,
    // to resize/move only once - when user has finished window resizing/moving

    //case WM_LBUTTONDOWN:
    //case WM_RBUTTONDOWN:
    //case WM_LBUTTONUP:
    //case WM_RBUTTONUP:
    //case WM_MOUSEMOVE:
    // WARNING: use #include <Windowsx.h> for the usefull GET_X_LPARAM() and GET_Y_LPARAM() macros
    }

    return DefWindowProc(windowHandle, message, wParam, lParam);
}