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

// Since CPU hasn't access to defaultBuffer commited to D3D12_HEAP_TYPE_DEFAULT,
// to copy CPU memory data (system data) into it we need to create an intermediate uploadBuffer
// into D3D12_HEAP_TYPE_UPLOAD, fill it by CPU data and then copy to defaultBuffer

// WARNING:
// uploadBuffer has to be kept alive after this function, because
// the command list has not been executed yet that performs the actual copy.
// So the caller should Release the uploadBuffer after it knows the copy has been executed
wrl::ComPtr<ID3D12Resource> Direct3D::CreateDefaultBuffer(
    const void *data,
    uint64_t byteSize,
    wrl::ComPtr<ID3D12Resource> &uploadBuffer) const
{
    wrl::ComPtr<ID3D12Resource> defaultBuffer;

    D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    D3D12_RESOURCE_DESC defaultResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    D3D_ASSERT(m_device->CreateCommittedResource(
        &defaultHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &defaultResourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        nullptr,
        IID_PPV_ARGS(&defaultBuffer)));

    D3D12_HEAP_PROPERTIES uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC uploadResourceDesc = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    D3D_ASSERT(m_device->CreateCommittedResource(
        &uploadHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &uploadResourceDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&uploadBuffer)));
    
    // Describe data which we want to copy into the defaultBuffer
    D3D12_SUBRESOURCE_DATA subresourceData;
    subresourceData.pData = data;
    subresourceData.RowPitch = byteSize;
    subresourceData.SlicePitch = byteSize;

    D3D12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_COPY_DEST);
    m_commandList->ResourceBarrier(1, &transitionBarrier);

    // copy CPU memory into defaultBuffer using uploadBuffer (wrapper from d3dx12.h)
    UpdateSubresources<1>(
        m_commandList.Get(),
        defaultBuffer.Get(),
        uploadBuffer.Get(),
        0,
        0,
        1,
        &subresourceData);

    transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        defaultBuffer.Get(),
        D3D12_RESOURCE_STATE_COPY_DEST,
        D3D12_RESOURCE_STATE_GENERIC_READ);
    m_commandList->ResourceBarrier(1, &transitionBarrier);

    return defaultBuffer;
}
