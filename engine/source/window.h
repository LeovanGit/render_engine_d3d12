#pragma once

#include <windows.h>
#include <stdint.h>

#include "d3dx12.h"

#include "utils/common.h"
#include "d3d.h"

namespace
{
#if defined(DEBUG) || defined(_DEBUG)
const float k_clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
#else
const float k_clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
#endif
} // namespace

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

    ID3D12Resource * GetCurrentBackBuffer() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const;

    void BindViewports() const;
    void BindScissorRects() const;
    void BindRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView) const;

    void SwapBackBuffers() const;

private:
    void CreateWindowClass(HINSTANCE instanceHandle, WNDPROC windowProc);

    void CreateSwapChain();
    void CreateDescriptorHeaps();

    void CreateBackBuffers();
    D3D12_CPU_DESCRIPTOR_HANDLE GetBackBufferView(uint8_t backBufferIndex = 0) const;
    void ClearRenderTargetView() const;

    void CreateViewport();
    void CreateScissorRectangles();

public:
    HWND m_windowHandle;
    uint16_t m_posX;
    uint16_t m_posY;
    uint16_t m_clientWidth;
    uint16_t m_clientHeight;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;

    static const DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static const uint8_t m_swapChainBufferCount = 2;
    mutable uint8_t m_currentBackBuffer = 0;

    wrl::ComPtr<IDXGISwapChain> m_swapChain;
    wrl::ComPtr<ID3D12Resource> m_backBuffers[m_swapChainBufferCount];

    wrl::ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    UINT m_rtvDescriptorSize;
};
