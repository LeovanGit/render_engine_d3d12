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
            "NORMAL",
            0,
            DXGI_FORMAT_R32G32B32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
        {
            "TEXCOORD",
            0,
            DXGI_FORMAT_R32G32_FLOAT,
            0,
            D3D12_APPEND_ALIGNED_ELEMENT,
            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
            0
        },
    };

    renderer.mesh = std::make_shared<Mesh>(
        Cube::s_vertices,
        sizeof(Cube::s_vertices) / sizeof(Vertex),
        sizeof(Vertex),
        Cube::s_indices,
        sizeof(Cube::s_indices) / sizeof(uint32_t),
        sizeof(uint32_t));

    ShaderManager *sm = ShaderManager::GetInstance();
    sm->GetOrCreateShader("../resources/shaders/triangle.hlsl", "mainVS", ShaderType::VERTEX_SHADER, &opaqueInputLayout);
    sm->GetOrCreateShader("../resources/shaders/triangle.hlsl", "mainPS", ShaderType::PIXEL_SHADER);
}
