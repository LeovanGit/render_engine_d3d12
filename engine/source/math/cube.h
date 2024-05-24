#pragma once

#include "../buffer.h"

struct Cube
{
    static const uint8_t s_verticesCount = 36;

    static Vertex s_vertices[s_verticesCount];
    static uint32_t s_indices[s_verticesCount];
};
