#include "renderer.h"

Renderer::Renderer(const Window *window)
    : m_window(window)
{
    Size windowClientSize = m_window->GetClientSize();

    CreateDescriptorHeaps();
    CreateDepthStencilBuffer(windowClientSize);
    CreateDepthStencilView();
}

void Renderer::Render()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->ResetCommandList(); // start recording commands

    m_window->BindViewports();
    m_window->BindScissorRects();

    BindDepthStencilBuffer();
    m_window->BindRenderTarget(GetDepthStencilView());

    mesh->BindVertexBuffer();
    mesh->BindIndexBuffer();

    d3d->m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    d3d->m_commandList->DrawIndexedInstanced(
        mesh->m_indexBuffer.GetSize(),
        1,
        0,
        0,
        0);

    d3d->CloseCommandList(); // done recording commands
    d3d->ExecuteCommandList();

    m_window->SwapBackBuffers();

    d3d->FlushCommandQueue();
}

void Renderer::CreateDescriptorHeaps()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    ZeroMemory(&dsvHeapDesc, sizeof(dsvHeapDesc));
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;

    D3D_ASSERT(d3d->m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

    m_dsvDescriptorSize = d3d->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilView() const
{
    return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void Renderer::CreateDepthStencilBuffer(const Size &windowClientSize)
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_RESOURCE_DESC depthStencilBufferDesc;
    ZeroMemory(&depthStencilBufferDesc, sizeof(depthStencilBufferDesc));
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
    D3D12_CLEAR_VALUE clearValue;
    ZeroMemory(&clearValue, sizeof(clearValue));
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
}

void Renderer::CreateDepthStencilView()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    // create descriptor (view):
    d3d->m_device->CreateDepthStencilView(
        m_depthStencilBuffer.Get(),
        nullptr,
        GetDepthStencilView());
}

void Renderer::ClearDepthStencilView()
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

    ClearDepthStencilView();

    transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        m_depthStencilBuffer.Get(),
        D3D12_RESOURCE_STATE_DEPTH_WRITE,
        D3D12_RESOURCE_STATE_COMMON);
    d3d->m_commandList->ResourceBarrier(1, &transitionBarrier);
}