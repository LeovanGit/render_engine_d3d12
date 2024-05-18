#include "shader.h"

namespace
{
UINT PATH_NOT_FOUND = 0x80070003;
} // namespace

Shader::Shader(
    const std::string &filename,
    const std::string &entryPoint,
    const ShaderType shaderType,
    const std::vector<D3D12_INPUT_ELEMENT_DESC> *inputLayout)
    // convert string to wstring (works only for 1 byte symbols):
    : m_filename(filename.begin(), filename.end())
    , m_entryPoint(entryPoint)
    , m_shaderType(shaderType)
{
    if (inputLayout) m_inputLayout = *inputLayout;

    Compile();
}

void Shader::Compile()
{
#if defined(DEBUG) || defined(_DEBUG)
    UINT flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    UINT flags = 0;
#endif

    wrl::ComPtr<ID3DBlob> errorMsg = nullptr;

    HRESULT result = D3DCompileFromFile(
        m_filename.c_str(),
        nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE,
        m_entryPoint.c_str(),
        m_shaderType == ShaderType::VERTEX_SHADER ? "vs_5_1" : "ps_5_1",
        flags,
        0,
        m_compiled.GetAddressOf(),
        &errorMsg);

    if (FAILED(result))
    {
        std::cout << "\n==========[FAILED TO COMPILE SHADER]==========\n";
        std::wcout << "Shader: " << m_filename << "\n";
        std::cout << "Entry Point: " << m_entryPoint << "\n\n";

        if (errorMsg)
            std::cout << (char *)errorMsg->GetBufferPointer() << "\n";
        else if (result == PATH_NOT_FOUND)
            std::cout << "Cannot find shader by specified path\n";

        assert(false && "D3DCompileFromFile()");
    }
}
