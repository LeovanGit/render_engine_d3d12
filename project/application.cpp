#include "application.h"

void Application::InitScene(Renderer &renderer)
{
    std::vector<D3D12_INPUT_ELEMENT_DESC> opaqueInputLayout =
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
        { DirectX::XMFLOAT3( 0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(1.0f, 0.0f, 0.0f) },
        { DirectX::XMFLOAT3(-0.5f, -0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f) },
        { DirectX::XMFLOAT3( 0.0f, 0.5f, 0.0f), DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f) },
    };

    uint32_t indices[]
    {
        0, 1, 2
    };

    renderer.mesh = std::make_shared<Mesh>(
        vertices,
        sizeof(vertices) / sizeof(Vertex),
        sizeof(Vertex),
        indices,
        sizeof(indices) / sizeof(uint32_t),
        sizeof(uint32_t));

    ShaderManager *sm = ShaderManager::GetInstance();
    sm->GetOrCreateShader("../resources/shaders/triangle.hlsl", "mainVS", ShaderType::VERTEX_SHADER, &opaqueInputLayout);
    sm->GetOrCreateShader("../resources/shaders/triangle.hlsl", "mainPS", ShaderType::PIXEL_SHADER);
}
