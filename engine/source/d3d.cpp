#include "d3d.h"

Direct3D *Direct3D::m_instance = nullptr;

void Direct3D::Init()
{
    if (!m_instance)
        m_instance = new Direct3D;
    else
        std::cout << "Direct3D::Init() was called twice!\n";
}

const Direct3D *Direct3D::GetInstance()
{
    assert(m_instance && "Direct3D is not initialized!");

    return m_instance;
}

void Direct3D::Deinit()
{
    if (m_instance)
    {
        delete m_instance;
        m_instance = nullptr;
    }
    else
        std::cout << "Direct3D::Deinit() was called before Direct3D::Init() or twice!\n";
}

Direct3D::Direct3D()
{
    CreateDevice();
    CreateFence();
    CreateCommandObjects();
}

Direct3D::~Direct3D()
{

}

void Direct3D::CreateDevice()
{
#if defined(DEBUG) || defined(_DEBUG)
    wrl::ComPtr<ID3D12Debug> debugController;
    D3D_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
    debugController->EnableDebugLayer();
#endif

    D3D_ASSERT(CreateDXGIFactory1(IID_PPV_ARGS(&m_factory)));

    HRESULT result = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
    if (FAILED(result)) // fallback to WARP (Windows default software adapter)
    {
        wrl::ComPtr<IDXGIAdapter> warpAdapter;
        D3D_ASSERT(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        D3D_ASSERT(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device)));
    }
}

void Direct3D::CreateFence()
{
    D3D_ASSERT(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
}

void Direct3D::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
    ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));
    commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

    D3D_ASSERT(m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)));

    D3D_ASSERT(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)));

    D3D_ASSERT(m_device->CreateCommandList(
        0,
        D3D12_COMMAND_LIST_TYPE_DIRECT,
        m_commandAllocator.Get(),
        nullptr, // temporary dummy PSO
        IID_PPV_ARGS(&m_commandList)));

    m_commandList->Close();
}
