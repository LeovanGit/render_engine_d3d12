#pragma once

#include <iostream>
#include <cassert>

#include "utils\non_copyable.h"

#include "buffer.h"

class MeshSystem : public NonCopyable
{
public:
    static void Init();

    static MeshSystem *GetInstance();

    static void Deinit();

    void SetMesh(Vertex * vertices, uint64_t byteSize);

    Buffer<Vertex> m_vertexBuffer;

private:
    MeshSystem() = default;
    ~MeshSystem() = default;

    static MeshSystem *s_instance;
};
