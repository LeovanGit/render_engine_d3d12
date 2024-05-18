#include "shader_manager.h"

ShaderManager *ShaderManager::s_instance = nullptr;

void ShaderManager::Init()
{
    if (!s_instance)
        s_instance = new ShaderManager;
    else
        std::cout << "ShaderManager::Init() was called twice!\n";
}

ShaderManager *ShaderManager::GetInstance()
{
    assert(s_instance && "ShaderManager is not initialized!");

    return s_instance;
}

void ShaderManager::Deinit()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
    else
        std::cout << "ShaderManager::Deinit() was called before ShaderManager::Init() or twice!\n";
}

const Shader& ShaderManager::GetOrCreateShader(
    const std::string &filename,
    const std::string &entryPoint,
    const ShaderType shaderType,
    const std::vector<D3D12_INPUT_ELEMENT_DESC> *inputLayout)
{
    std::string key = filename + entryPoint;

    auto found = m_shaders.find(key);
    if (found != m_shaders.end()) return found->second;
    
    auto insert = m_shaders.try_emplace(key, filename, entryPoint, shaderType, inputLayout);

    return insert.first->second;
}
