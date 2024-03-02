#include "d3d.h"

Direct3D *Direct3D::s_instance = nullptr;

void Direct3D::Init()
{
    if (!s_instance)
        s_instance = new Direct3D;
    else
        std::cout << "Direct3D::Init() was called twice!\n";
}

const Direct3D *Direct3D::GetInstance()
{
    assert(s_instance && "Direct3D is not initialized!");

    return s_instance;
}

void Direct3D::Deinit()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
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

    IDXGIAdapter *adapter = nullptr;
    for (uint8_t i = 0; m_factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
    {
        HRESULT result = D3D12CreateDevice(adapter, D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&m_device));
        
        if (SUCCEEDED(result))
        {
            DXGI_ADAPTER_DESC adapterDesc;
            adapter->GetDesc(&adapterDesc);
            std::wcout << "GPU[" << i << "] was selected: " << adapterDesc.Description << "\n";
            
            break;
        }
    }
}

void Direct3D::CreateFence()
{
    D3D_ASSERT(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
    m_currentFenceValue = 0;
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

void Direct3D::ResetCommandList() const
{
    D3D_ASSERT(m_commandAllocator->Reset());
    D3D_ASSERT(m_commandList->Reset(m_commandAllocator.Get(), nullptr));
}

void Direct3D::CloseCommandList() const
{
    D3D_ASSERT(m_commandList->Close());
}

void Direct3D::ExecuteCommandList() const
{
    // add command list to command queue for execution:
    ID3D12CommandList *commandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);
}

void Direct3D::FlushCommandQueue() const
{
    D3D_ASSERT(m_commandQueue->Signal(m_fence.Get(), ++m_currentFenceValue));

    // wait until GPU has completed all commands before new fence point
    if (m_fence->GetCompletedValue() < m_currentFenceValue)
    {
        HANDLE fenceEvent = CreateEvent(nullptr, false, false, nullptr);
        // execute some event when GPU receive new fence value
        // (needed only to make CPU wait here):
        D3D_ASSERT(m_fence->SetEventOnCompletion(m_currentFenceValue, fenceEvent));
        WaitForSingleObject(fenceEvent, INFINITE);

        CloseHandle(fenceEvent);
    }
}