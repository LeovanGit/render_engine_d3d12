#include "cube.h"

// unit ñube in local space.
// we should define all 12 triangles (36 vertices) without indices optimization
// to avoid artefacts with UV and normals.
// all triangles are clockwise
Vertex Cube::s_vertices[] =
{
    // front
    { DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },

    { DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f, -1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f, -1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

    // right
    { DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3( 1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },

    { DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3( 1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3( 1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

    // back
    { DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f,  1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f,  1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f,  1.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },

    { DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f,  1.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f,  1.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  0.0f,  1.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

    // left
    { DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

    { DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(-1.0f,  0.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

    // top
    { DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  1.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3( 0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  1.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  1.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },

    { DirectX::XMFLOAT3( 0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  1.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f, -0.5f), DirectX::XMFLOAT3( 0.0f,  1.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3(-0.5f,  0.5f,  0.5f), DirectX::XMFLOAT3( 0.0f,  1.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },

    // down
    { DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3( 0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 0.0f) },
    { DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },

    { DirectX::XMFLOAT3(-0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 1.0f) },
    { DirectX::XMFLOAT3(-0.5f, -0.5f, -0.5f), DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f), DirectX::XMFLOAT2(0.0f, 0.0f) },
    { DirectX::XMFLOAT3( 0.5f, -0.5f,  0.5f), DirectX::XMFLOAT3(0.0f, -1.0f,  0.0f), DirectX::XMFLOAT2(1.0f, 1.0f) },
};

uint32_t Cube::s_indices[] =
{
    // front
    0, 1, 2,
    3, 4, 5,

    // right
    6, 7, 8,
    9, 10, 11,

    // back
    12, 13, 14,
    15, 16, 17,

    // left
    18, 19, 20,
    21, 22, 23,

    // top
    24, 25, 26,
    27, 28, 29,

    // down
    30, 31, 32,
    33, 34, 35
};
