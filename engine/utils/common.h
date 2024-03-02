#pragma once

#include <d3d12.h>
#include <dxgi1_5.h>
#include <wrl\client.h> // ComPtr

// #include "d3dx12.h"

#include <cassert>

namespace wrl = Microsoft::WRL;

#if defined(DEBUG) || defined(_DEBUG)
    #define D3D_ASSERT(hresult) assert((hresult) >= 0)
#else
    #define D3D_ASSERT(hresult) hresult
#endif
