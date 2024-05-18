#pragma once

#include "buffer.h"

struct Mesh
{
public:
    Mesh(
        Vertex *vertices,
        uint32_t verticesSize,
        uint32_t verticesStride,
        uint32_t *indices,
        uint32_t indicesSize,
        uint32_t indicesStride);

private:
    void CreateVertexBuffer(Vertex *vertices, uint32_t size, uint32_t stride);
    void CreateIndexBuffer(uint32_t *indices, uint32_t size, uint32_t stride);

public:
    void BindVertexBuffer() const;
    void BindIndexBuffer() const;

    Buffer m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    Buffer m_indexBuffer;
    D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
};
