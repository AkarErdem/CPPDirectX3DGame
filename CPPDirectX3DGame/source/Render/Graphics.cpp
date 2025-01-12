#include "Render/Graphics.h"
#include "Win32/OWin.h"

#pragma comment(lib, "d3d11.lib")

Graphics::Graphics(HWND hWnd, int width, int height)
{
    DXGI_SWAP_CHAIN_DESC sd = {};
    // Width and height 0 means look at the window and you figure it out
    sd.BufferDesc.Width = width;
    sd.BufferDesc.Height = height;
    // Layout of the pixels, channels in there and this is the BGRA layout
    sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    // Refresh rate, pick whatever is available
    sd.BufferDesc.RefreshRate.Numerator = 0;
    sd.BufferDesc.RefreshRate.Denominator = 0;
    // Since the width and height is undefined, scaling should not be specified
    sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    // Anti-aliasing
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    // Buffer
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = 1; // One back buffer (double buffering)
    // Window mode
    sd.OutputWindow = hWnd;
    sd.Windowed = TRUE;
    // Effect used for flipping the presentation
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    sd.Flags = 0;


    // Creates a device that represents the display adapter and a swap chain used for rendering.
    // Also creates front/back buffers.
    D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext);

    // Gain access to texture subresource in swap chain (back buffer)
    ID3D11Resource* pBackBuffer = nullptr;
    // first param 0 will give the back buffer,
    pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
    pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
    pBackBuffer->Release();
}

Graphics::~Graphics()
{
    if (pTarget != nullptr)
        pTarget->Release();

    if (pContext != nullptr)
        pContext->Release();

    if (pSwap != nullptr)
        pSwap->Release();

    if (pDevice != nullptr)
        pDevice->Release();
}

void Graphics::EndFrame()
{
    pSwap->Present(1u, 0u);
}
