#pragma once

#include <d3dcompiler.h>
#include <iostream>
#include <string>
#include <vector>

#include <utils/common.h>

enum class ShaderType
{
    VERTEX_SHADER = 0,
    PIXEL_SHADER,
};

class Shader
{
public:
    Shader(
        const std::string &filename,
        const std::string &entryPoint,
        const ShaderType shaderType,
        const std::vector<D3D12_INPUT_ELEMENT_DESC> *inputLayout = nullptr);

    wrl::ComPtr<ID3DBlob> GetBytecode() const
    {
        return m_compiled;
    }

    const std::vector<D3D12_INPUT_ELEMENT_DESC> & GetInputLayout() const
    {
        return m_inputLayout;
    }

private:
    void Compile();

    std::wstring m_filename;
    std::string m_entryPoint;
    ShaderType m_shaderType;
    wrl::ComPtr<ID3DBlob> m_compiled;

    // vertex shader only:
    std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
};
