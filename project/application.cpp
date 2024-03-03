#include "application.h"

void Application::InitScene()
{
    // pass to CreateShader() later
    D3D12_INPUT_ELEMENT_DESC opaqueVertexLayout[]
    {
        {
            "POSITION",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            0,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "COLOR",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };

    Vertex vertices[]
    {
        { DirectX::XMFLOAT3( 1.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { DirectX::XMFLOAT3(-1.0f, 0.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3( 0.0f, 1.0f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
    };

    MeshSystem *mesh_system = MeshSystem::GetInstance();
    mesh_system->SetMesh(vertices, sizeof(vertices));
}
