#pragma once

#include <d3dx12.h>
#include <DirectXMath.h>

#include "d3d.h"

struct Vertex
{
    DirectX::XMFLOAT3 position;
    DirectX::XMFLOAT3 color;
};

template<class T>
class Buffer
{
public:
    Buffer() = default;

    Buffer(const T *data, uint64_t byteSize)
    {
        Init(data, byteSize);
    }

    void Init(const T *data, uint64_t byteSize)
    {
        const Direct3D *d3d = Direct3D::GetInstance();

        m_byteSize = byteSize;

        m_buffer = d3d->CreateDefaultBuffer(data, byteSize, m_uploadBuffer);
        CreateVertexBufferView();
    };

    void Bind() const
    {
        const Direct3D *d3d = Direct3D::GetInstance();

        d3d->m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
    }

    uint64_t GetByteSize() const
    {
        return m_byteSize;
    }

    uint64_t GetSize() const
    {
        return m_byteSize / sizeof(T);
    }

private:
    void CreateVertexBufferView()
    {
        m_vertexBufferView.BufferLocation = m_buffer->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = m_byteSize;
        m_vertexBufferView.StrideInBytes = sizeof(T);
    }

    wrl::ComPtr<ID3D12Resource> m_uploadBuffer;

    uint64_t m_byteSize;
    wrl::ComPtr<ID3D12Resource> m_buffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
};
