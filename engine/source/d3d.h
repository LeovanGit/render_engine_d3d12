#pragma once

#include <iostream>

#include "utils/common.h"
#include "utils/non_copyable.h"

class Direct3D : public NonCopyable
{
public:
    static void Init();

    static const Direct3D *GetInstance();

    static void Deinit();

    void ResetCommandList() const;
    void CloseCommandList() const;
    void ExecuteCommandList() const;

    void FlushCommandQueue() const;

    wrl::ComPtr<IDXGIFactory5> m_factory;
    wrl::ComPtr<ID3D12Device> m_device;

    mutable uint64_t m_currentFenceValue;
    wrl::ComPtr<ID3D12Fence> m_fence;
    wrl::ComPtr<ID3D12CommandQueue> m_commandQueue;
    wrl::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
    wrl::ComPtr<ID3D12GraphicsCommandList> m_commandList;

private:
    Direct3D();
    ~Direct3D();

    static Direct3D *s_instance;

    void CreateDevice();

    void CreateFence();
    void CreateCommandObjects();
};
