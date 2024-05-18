#pragma once

#include <unordered_map>

#include "shader.h"
#include "utils/non_copyable.h"

class ShaderManager : public NonCopyable
{
public:
    static void Init();

    static ShaderManager *GetInstance();

    static void Deinit();

    const Shader& GetOrCreateShader(
        const std::string &filename,
        const std::string &entryPoint,
        const ShaderType shaderType);

private:
    ShaderManager() = default;
    ~ShaderManager() = default;

    static ShaderManager *s_instance;

    std::unordered_map<std::string, Shader> m_shaders;
};
