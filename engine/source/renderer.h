#pragma once

#include "window.h"

class Renderer
{
public:
    Renderer(const Window &window);

private:
    void CreateDescriptorHeaps();
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;

    void CreateDepthStencilBuffer(const Size &windowClientSize);
    void CreateDepthStencilView();

public:
    wrl::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    UINT m_dsvDescriptorSize;

    static const DXGI_FORMAT m_depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    wrl::ComPtr<ID3D12Resource> m_depthStencilBuffer;
};
