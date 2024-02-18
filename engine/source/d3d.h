#pragma once

#include <iostream>

#include "common/common.h"
#include "common/non_copyable.h"

class Direct3D : public NonCopyable
{
public:
    static void Init();

    static const Direct3D *GetInstance();

    static void Deinit();

    wrl::ComPtr<IDXGIFactory5> m_factory;
    wrl::ComPtr<ID3D12Device> m_device;

    wrl::ComPtr<ID3D12Fence> m_fence;
    wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
    wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;

private:
    Direct3D();
    ~Direct3D();

    void CreateDevice();

    void CreateFence();
    void CreateCommandObjects();

    static Direct3D *m_instance;
};
