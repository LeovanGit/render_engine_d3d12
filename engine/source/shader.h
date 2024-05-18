#pragma once

#include <d3dcompiler.h>
#include <iostream>
#include <string>

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
        const ShaderType shaderType);

private:
    void Compile();

    std::wstring m_filename;
    std::string m_entryPoint;
    ShaderType m_shaderType;
    wrl::ComPtr<ID3DBlob> m_compiled;
};
