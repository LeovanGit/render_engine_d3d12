#pragma once

#include <d3dx12.h>
#include <DirectXMath.h>

#include "d3d.h"

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 color;
};

class Buffer
{
public:
    Buffer();
    Buffer(const void *data, uint32_t size, uint32_t stride);

    void Init(const void *data, uint32_t size, uint32_t stride);

    ~Buffer();

    uint32_t GetSize() const
    {
        return m_size;
    }

    uint32_t GetStride() const
    {
        return m_stride;
    }

    uint32_t GetByteSize() const
    {
        return m_size * m_stride;
    }

    D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const
    {
        return m_gpuVirtualAddress;
    }

    wrl::ComPtr<ID3D12Resource> GetHandle() const
    {
        return m_defaultBuffer;
    }

    void* Map();
    void Unmap();

private:
    uint32_t m_size; // elements count
    uint32_t m_stride; // byte size of element
    bool m_isMapped;

    // cache to avoid calls to ID3D12Resource::GetGpuVirtualAddress():
    D3D12_GPU_VIRTUAL_ADDRESS m_gpuVirtualAddress;

    // GPU buffer (in default heap)
    wrl::ComPtr<ID3D12Resource> m_defaultBuffer;

    // to upload data from CPU to GPU (to m_defaultBuffer)
    wrl::ComPtr<ID3D12Resource> m_uploadBuffer;
};
