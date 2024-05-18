#include "mesh.h"

Mesh::Mesh(
    Vertex *vertices,
    uint32_t verticesSize,
    uint32_t verticesStride,
    uint32_t *indices,
    uint32_t indicesSize,
    uint32_t indicesStride)
{
    CreateVertexBuffer(vertices, verticesSize, verticesStride);
    CreateIndexBuffer(indices, indicesSize, indicesStride);
}

void Mesh::CreateVertexBuffer(Vertex *vertices, uint32_t size, uint32_t stride)
{
    m_vertexBuffer = Buffer(vertices, size, stride);

    m_vertexBufferView.BufferLocation = m_vertexBuffer.GetGPUVirtualAddress();
    m_vertexBufferView.SizeInBytes = m_vertexBuffer.GetByteSize();
    m_vertexBufferView.StrideInBytes = m_vertexBuffer.GetStride();
}

void Mesh::CreateIndexBuffer(uint32_t *indices, uint32_t size, uint32_t stride)
{
    m_indexBuffer = Buffer(indices, size, stride);

    m_indexBufferView.BufferLocation = m_indexBuffer.GetGPUVirtualAddress();
    m_indexBufferView.SizeInBytes = m_indexBuffer.GetByteSize();
    m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
}

void Mesh::BindVertexBuffer() const
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
}

void Mesh::BindIndexBuffer() const
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->m_commandList->IASetIndexBuffer(&m_indexBufferView);
}
