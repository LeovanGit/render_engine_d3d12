#include "renderer.h"

Renderer::Renderer(const Window *window)
    : m_window(window)
{

}

void Renderer::Init()
{
    Size windowClientSize = m_window->GetClientSize();

    CreateDescriptorHeaps();
    CreateDepthStencilBuffer(windowClientSize);
    CreateConstantBuffer();

    CreateRootSignature();
    CreatePipeline();
}

void Renderer::Render()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->ResetCommandList(); // start recording commands

    d3d->m_commandList->SetPipelineState(m_PSO.Get());

    m_window->BindViewports();
    m_window->BindScissorRects();

    BindDepthStencilBuffer();
    m_window->BindRenderTarget(GetDepthStencilView());

    d3d->m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // Update Constant Buffer Hardcoded:
    ConstantBufferData cbufferData;
    // translate +1 z and rotate around y and x by 45 grad:
    cbufferData.transformMat =
    {
        0.7f, -0.49f, -0.49f, 0.0f,
        0.0f, 0.7f, -0.7f, 0.0f,
        0.7f, 0.49f, 0.49f, 0.0f,
        0.0f, 0.0f, 1.0f, 1.0f
    };
    UpdateConstantBuffer(&cbufferData);

    BindConstantBuffer();

    mesh->BindVertexBuffer();
    mesh->BindIndexBuffer();

    d3d->m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3d->m_commandList->DrawIndexedInstanced(
        mesh->m_indexBuffer.GetSize(),
        1,
        0,
        0,
        0);

    D3D12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_window->GetCurrentBackBuffer(),
        D3D12_RESOURCE_STATE_RENDER_TARGET,
        D3D12_RESOURCE_STATE_PRESENT);
    d3d->m_commandList->ResourceBarrier(1, &transitionBarrier);

    transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_depthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_COMMON);
    d3d->m_commandList->ResourceBarrier(1, &transitionBarrier);

    d3d->CloseCommandList(); // done recording commands
    d3d->ExecuteCommandList();

    m_window->SwapBackBuffers();

    d3d->FlushCommandQueue();
}

void Renderer::CreateDescriptorHeaps()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    // DSV
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    D3D_ASSERT(d3d->m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));
    m_dsvDescriptorSize = d3d->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    
    // CBV SRV UAV
    D3D12_DESCRIPTOR_HEAP_DESC cbvSrvUavHeapDesc = {};
    cbvSrvUavHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvSrvUavHeapDesc.NumDescriptors = 1;
    cbvSrvUavHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvSrvUavHeapDesc.NodeMask = 0;

    D3D_ASSERT(d3d->m_device->CreateDescriptorHeap(&cbvSrvUavHeapDesc, IID_PPV_ARGS(&m_cbvSrvUavHeap)));
    m_cbvSrvUavDescriptorSize = d3d->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilView() const
{
    return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetConstantBufferView() const
{
    return m_cbvSrvUavHeap->GetCPUDescriptorHandleForHeapStart();
}

void Renderer::CreateDepthStencilBuffer(const Size &windowClientSize)
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_RESOURCE_DESC depthStencilBufferDesc = {};
    depthStencilBufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilBufferDesc.Alignment = 0;
    depthStencilBufferDesc.Width = windowClientSize.width;
    depthStencilBufferDesc.Height = windowClientSize.height;
    depthStencilBufferDesc.DepthOrArraySize = 1;
    depthStencilBufferDesc.MipLevels = 1;
    depthStencilBufferDesc.Format = m_depthStencilBufferFormat;
    depthStencilBufferDesc.SampleDesc.Count = 1;
    depthStencilBufferDesc.SampleDesc.Quality = 0;
    depthStencilBufferDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilBufferDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    // optimized value for clearing resources
    // (clear calls with it potentially faster than with nullptr)
    D3D12_CLEAR_VALUE clearValue = {};
    clearValue.Format = m_depthStencilBufferFormat;
    clearValue.DepthStencil.Depth = 1.0f;
    clearValue.DepthStencil.Stencil = 0;

    // resource heap (in VRAM), not related with descriptors heap:
    D3D12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    D3D_ASSERT(d3d->m_device->CreateCommittedResource(
        &heapProperties,
        D3D12_HEAP_FLAG_NONE,
        &depthStencilBufferDesc,
        D3D12_RESOURCE_STATE_COMMON,
        &clearValue,
        IID_PPV_ARGS(&m_depthStencilBuffer)));

    // create descriptor (view):
    d3d->m_device->CreateDepthStencilView(
        m_depthStencilBuffer.Get(),
        nullptr,
        GetDepthStencilView());
}

void Renderer::ClearDepthStencilBuffer()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->m_commandList->ClearDepthStencilView(
        GetDepthStencilView(),
        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
        1.0f,
        0,
        0,
        nullptr);
}

void Renderer::BindDepthStencilBuffer()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_depthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_COMMON,
        D3D12_RESOURCE_STATE_DEPTH_WRITE);
    d3d->m_commandList->ResourceBarrier(1, &transitionBarrier);

    ClearDepthStencilBuffer();
}

void Renderer::CreateConstantBuffer()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    // Hardware requirement:
    // constant buffer size should be aligned by minimum hardware allocation size (256 bytes):
    UINT elementByteSize = alignUp<UINT>(sizeof(ConstantBufferData), 256);

    // constant buffer updating by CPU very often so we create it immediately in upload heap:
    D3D12_HEAP_PROPERTIES defaultHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    D3D12_RESOURCE_DESC constantBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(elementByteSize);
    D3D_ASSERT(d3d->m_device->CreateCommittedResource(
        &defaultHeapProperties,
        D3D12_HEAP_FLAG_NONE,
        &constantBufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ,
        nullptr,
        IID_PPV_ARGS(&m_constantBuffer)));

    m_constantBufferGpuVirtualAddress = m_constantBuffer->GetGPUVirtualAddress();

    // create descriptor (view):
    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
    cbvDesc.BufferLocation = m_constantBufferGpuVirtualAddress;
    cbvDesc.SizeInBytes = elementByteSize;

    d3d->m_device->CreateConstantBufferView(
        &cbvDesc,
        GetConstantBufferView());
}

void Renderer::UpdateConstantBuffer(const ConstantBufferData* data)
{
    void *mappedData = nullptr;
    m_constantBuffer->Map(0, nullptr, &mappedData);

    memcpy(mappedData, data, sizeof(ConstantBufferData));

    m_constantBuffer->Unmap(0, nullptr);
    mappedData = nullptr;
}

void Renderer::BindConstantBuffer()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    ID3D12DescriptorHeap *descriptorHeaps[] = { m_cbvSrvUavHeap.Get() };
    d3d->m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    CD3DX12_GPU_DESCRIPTOR_HANDLE cbvHandle(m_cbvSrvUavHeap->GetGPUDescriptorHandleForHeapStart());
    cbvHandle.Offset(0, m_cbvSrvUavDescriptorSize);

    d3d->m_commandList->SetGraphicsRootDescriptorTable(0, cbvHandle);
}

void Renderer::CreateRootSignature()
{
    // merge descriptors in descriptor ranges:
    CD3DX12_DESCRIPTOR_RANGE descRange[1];
    descRange[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);

    // create root parameters (descriptor tables):
    CD3DX12_ROOT_PARAMETER rootParameters[1];
    rootParameters[0].InitAsDescriptorTable(1, &descRange[0]);

    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(
        1,
        rootParameters,
        0,
        nullptr,
        D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    wrl::ComPtr<ID3DBlob> rootSignatureBlob = nullptr;
    wrl::ComPtr<ID3DBlob> errorMsg = nullptr;

    D3D_ASSERT(D3D12SerializeRootSignature(
        &rootSignatureDesc,
        D3D_ROOT_SIGNATURE_VERSION_1,
        rootSignatureBlob.GetAddressOf(),
        errorMsg.GetAddressOf()));

    const Direct3D *d3d = Direct3D::GetInstance();
    d3d->m_device->CreateRootSignature(
        0,
        rootSignatureBlob->GetBufferPointer(),
        rootSignatureBlob->GetBufferSize(),
        IID_PPV_ARGS(&m_rootSignature));
}

void Renderer::CreatePipeline()
{
    ShaderManager *sm = ShaderManager::GetInstance();
    const Shader &vs = sm->GetOrCreateShader("../resources/shaders/triangle.hlsl", "mainVS", ShaderType::VERTEX_SHADER);
    const Shader &ps = sm->GetOrCreateShader("../resources/shaders/triangle.hlsl", "mainPS", ShaderType::PIXEL_SHADER);

    D3D12_GRAPHICS_PIPELINE_STATE_DESC PSOdesc = {};
    PSOdesc.pRootSignature = m_rootSignature.Get();
    PSOdesc.VS = { vs.GetBytecode()->GetBufferPointer(), vs.GetBytecode()->GetBufferSize() };
    PSOdesc.PS = { ps.GetBytecode()->GetBufferPointer(), ps.GetBytecode()->GetBufferSize() };
    PSOdesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    PSOdesc.SampleMask = UINT_MAX;
    PSOdesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    PSOdesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    PSOdesc.InputLayout = { vs.GetInputLayout().data(), UINT(vs.GetInputLayout().size()) };
    PSOdesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    PSOdesc.NumRenderTargets = 1;
    PSOdesc.RTVFormats[0] = m_window->m_backBufferFormat;
    PSOdesc.DSVFormat = m_depthStencilBufferFormat;
    PSOdesc.SampleDesc.Count = 1;
    PSOdesc.SampleDesc.Quality = 0;

    const Direct3D *d3d = Direct3D::GetInstance();
    D3D_ASSERT(d3d->m_device->CreateGraphicsPipelineState(
        &PSOdesc,
        IID_PPV_ARGS(&m_PSO)));
}
