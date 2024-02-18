#pragma once

#include <windows.h>
#include <stdint.h>

#include "common/common.h"
#include "d3d.h"

struct Size
{
    uint16_t width;
    uint16_t height;
};

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

    Size GetClientSize() const;

private:
    void CreateWindowClass(HINSTANCE instanceHandle, WNDPROC windowProc);

    void CreateSwapChain();
    void CreateDescriptorHeaps();

    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView(uint8_t backBufferIndex = 0) const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

    void CreateBackBuffers();

    void CreateViewport();
    void CreateScissorRectangles();

public:
    HWND m_windowHandle;
    uint16_t m_posX;
    uint16_t m_posY;
    uint16_t m_clientWidth;
    uint16_t m_clientHeight;

    static const DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static const uint8_t m_swapChainBufferCount = 2;
    uint8_t m_currentBackBuffer = 0;

    wrl::ComPtr<IDXGISwapChain> m_swapChain;
    wrl::ComPtr<ID3D12Resource> m_backBuffers[m_swapChainBufferCount]; // swapChain buffers

    wrl::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    UINT m_rtvDescriptorSize;
};
