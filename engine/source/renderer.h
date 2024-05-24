#pragma once

#include "d3dx12.h"

#include "window.h"
#include "d3d.h"
#include "mesh.h"
#include "shader_manager.h"

#include <utils/utils.h>

class Renderer
{
public:
    Renderer(const Window *window);

    void Init();

    void Render();

    struct ConstantBufferData
    {
        DirectX::XMFLOAT4X4 transformMat;
    };

private:
    void CreateDescriptorHeaps();
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetConstantBufferView() const;

    void CreateDepthStencilBuffer(const Size &windowClientSize);
    void ClearDepthStencilBuffer();
    void BindDepthStencilBuffer();

    void CreateConstantBuffer();
    void UpdateConstantBuffer(const ConstantBufferData *data);
    void BindConstantBuffer();

    void CreateRootSignature();
    void CreatePipeline();

public:
    const Window *m_window;

    std::shared_ptr<Mesh> mesh;

    wrl::ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    UINT m_dsvDescriptorSize;

    wrl::ComPtr<ID3D12DescriptorHeap> m_cbvSrvUavHeap;
    UINT m_cbvSrvUavDescriptorSize;

    static const DXGI_FORMAT m_depthStencilBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    wrl::ComPtr<ID3D12Resource> m_depthStencilBuffer;

    wrl::ComPtr<ID3D12Resource> m_constantBuffer;
    // cache to avoid calls to ID3D12Resource::GetGpuVirtualAddress():
    D3D12_GPU_VIRTUAL_ADDRESS m_constantBufferGpuVirtualAddress;

    wrl::ComPtr<ID3D12RootSignature> m_rootSignature;
    wrl::ComPtr<ID3D12PipelineState> m_PSO;
};
