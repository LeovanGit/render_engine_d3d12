#include "window.h"

Window::Window(
    HINSTANCE instanceHandle,
    WNDPROC windowProc,
    int cmdShow,
    uint16_t posX,
    uint16_t posY,
    uint16_t clientWidth,
    uint16_t clientHeight)
    : m_posX(posX)
    , m_posY(posY)
    , m_clientWidth(clientWidth)
    , m_clientHeight(clientHeight)
{
    CreateWindowClass(instanceHandle, windowProc);

    // calculate real window size (with borders) from client area size:
    RECT windowSize = { 0, 0, m_clientWidth, m_clientHeight };
    AdjustWindowRect(&windowSize, WS_OVERLAPPEDWINDOW, false);

    m_windowHandle = CreateWindowEx(
        NULL,
        L"WindowClass1",
        L"WindowTitle",
        WS_OVERLAPPEDWINDOW,
        m_posX,
        m_posY,
        windowSize.right - windowSize.left,
        windowSize.bottom - windowSize.top,
        NULL,
        NULL,
        instanceHandle,
        NULL);

    ShowWindow(m_windowHandle, cmdShow);

    CreateSwapChain();
    CreateDescriptorHeaps();
    CreateBackBuffers();

    CreateViewport();
    CreateScissorRectangles();
}

Size Window::GetClientSize() const
{
    return { m_clientWidth, m_clientHeight };
}

ID3D12Resource * Window::GetCurrentBackBuffer() const
{
    return m_backBuffers[m_currentBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE Window::GetCurrentBackBufferView() const
{
    return GetBackBufferView(m_currentBackBuffer);
}

D3D12_CPU_DESCRIPTOR_HANDLE Window::GetBackBufferView(uint8_t backBufferIndex) const
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(
        m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
        backBufferIndex,
        m_rtvDescriptorSize);
}

void Window::BindViewports() const
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->m_commandList->RSSetViewports(1, &m_viewport);

}

void Window::BindScissorRects() const
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->m_commandList->RSSetScissorRects(1, &m_scissorRect);
}

void Window::BindRenderTarget(D3D12_CPU_DESCRIPTOR_HANDLE depthStencilView) const
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_RESOURCE_BARRIER transitionBarrier = CD3DX12_RESOURCE_BARRIER::Transition(
        GetCurrentBackBuffer(),
        D3D12_RESOURCE_STATE_PRESENT,
        D3D12_RESOURCE_STATE_RENDER_TARGET);
    d3d->m_commandList->ResourceBarrier(1, &transitionBarrier);
    
    ClearRenderTargetView();

    D3D12_CPU_DESCRIPTOR_HANDLE backBufferView = GetCurrentBackBufferView();
    d3d->m_commandList->OMSetRenderTargets(
        1,
        &backBufferView,
        true,
        &depthStencilView);
}

void Window::ClearRenderTargetView() const
{
    const Direct3D *d3d = Direct3D::GetInstance();

    d3d->m_commandList->ClearRenderTargetView(
        GetCurrentBackBufferView(),
        k_clearColor,
        0,
        nullptr);
}

void Window::SwapBackBuffers() const
{
    // swap back and front buffers
    D3D_ASSERT(m_swapChain->Present(0, 0));

    m_currentBackBuffer = (m_currentBackBuffer + 1) % m_swapChainBufferCount;
}

void Window::CreateWindowClass(HINSTANCE instanceHandle, WNDPROC windowProc)
{
    WNDCLASSEX windowClass;
    ZeroMemory(&windowClass, sizeof(WNDCLASSEX));

    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = instanceHandle;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
    windowClass.lpszClassName = L"WindowClass1";

    RegisterClassEx(&windowClass);
}

void Window::CreateSwapChain()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
    swapChainDesc.BufferDesc.Width = m_clientWidth;
    swapChainDesc.BufferDesc.Height = m_clientHeight;
    swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    swapChainDesc.BufferDesc.Format = m_backBufferFormat;
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    swapChainDesc.SampleDesc.Count = 1; // MSAA
    swapChainDesc.SampleDesc.Quality = 0;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = m_swapChainBufferCount;
    swapChainDesc.OutputWindow = m_windowHandle;
    swapChainDesc.Windowed = true;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    D3D_ASSERT(d3d->m_factory->CreateSwapChain(
        d3d->m_commandQueue.Get(),
        &swapChainDesc,
        &m_swapChain));
}

void Window::CreateDescriptorHeaps()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    ZeroMemory(&rtvHeapDesc, sizeof(rtvHeapDesc));
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = m_swapChainBufferCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;

    D3D_ASSERT(d3d->m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    m_rtvDescriptorSize = d3d->m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}

void Window::CreateBackBuffers()
{
    const Direct3D *d3d = Direct3D::GetInstance();

    for (uint8_t i = 0; i != m_swapChainBufferCount; ++i)
    {
        D3D_ASSERT(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])));

        d3d->m_device->CreateRenderTargetView(
            m_backBuffers[i].Get(),
            nullptr,
            GetBackBufferView(i));
    }
}

void Window::CreateViewport()
{
    ZeroMemory(&m_viewport, sizeof(m_viewport));
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<float>(m_clientWidth);
    m_viewport.Height = static_cast<float>(m_clientHeight);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;
}

void Window::CreateScissorRectangles()
{
    ZeroMemory(&m_scissorRect, sizeof(m_scissorRect));
    m_scissorRect.left = 0;
    m_scissorRect.top = 0;
    m_scissorRect.right = m_clientWidth;
    m_scissorRect.bottom = m_clientHeight;
}
