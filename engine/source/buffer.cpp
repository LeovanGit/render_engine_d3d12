#include "buffer.h"

Buffer::Buffer()
    : m_size(0)
    , m_stride(0)
    , m_isMapped(false)
    , m_gpuVirtualAddress(0)
{

}

Buffer::Buffer(const void *data, uint32_t size, uint32_t stride)
{
    Init(data, size, stride);
}

void Buffer::Init(const void *data, uint32_t size, uint32_t stride)
{
    m_size = size;
    m_stride = stride;
    m_isMapped = false;

    const Direct3D *d3d = Direct3D::GetInstance();
    m_defaultBuffer = d3d->CreateDefaultBuffer(data, m_size * m_stride, m_uploadBuffer);

    m_gpuVirtualAddress = m_defaultBuffer->GetGPUVirtualAddress();
}

Buffer::~Buffer()
{
    assert(!m_isMapped && "Attempting to destruct mapped Buffer!");
}

void* Buffer::Map()
{
    assert(!m_isMapped && "Buffer is already mapped!");

    void *mappedData = nullptr;
    m_uploadBuffer->Map(0, nullptr, &mappedData);
    m_isMapped = true;

    return mappedData;
}

void Buffer::Unmap()
{
    assert(m_isMapped && "Buffer is not mapped!");

    m_uploadBuffer->Unmap(0, nullptr);
    m_isMapped = false;
}