#include "Render/Graphics.h"
#include "Render/GraphicsThrowMacros.h"
#include "OWin/OWin.h"
#include <sstream>
#include <unordered_map>
#include <d3dcompiler.h>
#include <cmath>
#include <DirectXMath.h>
#include <array>

namespace dx = DirectX;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib,"D3DCompiler.lib")

#define GFX_THROW_FAILED(hrcall) if (FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__, __FILE__, (hr));
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))

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

    UINT swapCreateFlags = 0u;
#ifndef NDEBUG
    swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    // for checking results of d3d functions
    HRESULT hr;

    // Creates a device that represents the display adapter and a swap chain used for rendering.
    // Also creates front/back buffers.
    GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        swapCreateFlags,
        nullptr,
        0,
        D3D11_SDK_VERSION,
        &sd,
        &pSwap,
        &pDevice,
        nullptr,
        &pContext));

    // Gain access to texture subresource in swap chain (back buffer)
    ID3D11Resource* pBackBuffer = nullptr;
    // first param 0 will give the back buffer,
    pSwap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&pBackBuffer));
    pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &pTarget);
    pBackBuffer->Release();

    // wrl::ComPtr<ID3D11Texture2D> pBackBuffer;
    // GFX_THROW_INFO(pSwap->GetBuffer(0, __uuidof(ID3D11Texture2D), &pBackBuffer));
    // pTarget = std::shared_ptr<Bind::RenderTarget>{ new Bind::OutputOnlyRenderTarget(*this,pBackBuffer.Get()) };
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
    HRESULT hr;
    if (FAILED(hr = pSwap->Present(1u, 0u))) {
        if (hr == DXGI_ERROR_DEVICE_REMOVED) {
            throw GFX_DEVICE_REMOVED_EXCEPT(pDevice->GetDeviceRemovedReason());
        }
        else {
            GFX_THROW_FAILED(hr);
        }
    }
}

// Graphics exception
Graphics::HrException::HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept
    :
    Exception(line, file),
    hr(hr)
{
    // join all info messages with newlines into single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    // remove final newline if exists
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* Graphics::HrException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
        << std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
        << "[Error String] " << GetErrorString() << std::endl
        << "[Description] " << GetErrorDescription() << std::endl;
    if (!info.empty())
    {
        oss << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    }
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::HrException::GetType() const noexcept
{
    return "O Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
    return hr;
}

// Custom error string replacement
std::string Graphics::HrException::GetErrorString() const noexcept
{
    static const std::unordered_map<HRESULT, std::string> errorMap = {
        // Direct3D 11 Common Errors
        {S_OK, "Operation successful"},
        {E_FAIL, "Unspecified error"},
        {E_INVALIDARG, "Invalid argument"},
        {E_OUTOFMEMORY, "Out of memory"},
        {E_NOTIMPL, "Not implemented"},

        // DXGI Specific Errors
        {DXGI_ERROR_DEVICE_REMOVED, "Device removed"},
        {DXGI_ERROR_DEVICE_HUNG, "Device hung"},
        {DXGI_ERROR_DEVICE_RESET, "Device reset"},
        {DXGI_ERROR_DRIVER_INTERNAL_ERROR, "Driver internal error"},
        {DXGI_ERROR_INVALID_CALL, "Invalid call"},
        {DXGI_ERROR_UNSUPPORTED, "Unsupported operation"},
        {DXGI_ERROR_MORE_DATA, "More data required"},
        {DXGI_ERROR_NONEXCLUSIVE, "Resource non-exclusive"},
        {DXGI_ERROR_NOT_CURRENTLY_AVAILABLE, "Resource not currently available"},
        {DXGI_ERROR_WAS_STILL_DRAWING, "Previous operation still in progress"},

        // D3D11 Specific Errors
        {D3D11_ERROR_FILE_NOT_FOUND, "File not found"},
        {D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS, "Too many unique state objects"},
        {D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS, "Too many unique view objects"},
        {D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD, "Invalid deferred context map operation"}
    };

    auto it = errorMap.find(hr);
    if (it != errorMap.end()) {
        return it->second;
    }

    // Handle facility-specific errors
    if (FACILITY_WINDOWS == HRESULT_FACILITY(hr)) {
        return std::system_category().message(hr);
    }

    std::ostringstream oss;
    oss << "Unknown error code (0x" << std::hex << std::uppercase << hr << ")";
    return oss.str();
}

std::string Graphics::HrException::GetErrorDescription() const noexcept {
    switch (hr) {
        // Success cases
    case S_OK:
        return "The operation completed successfully.";

        // General errors
    case E_FAIL:
        return "An unspecified error occurred. Check the debug output for more information.";
    case E_INVALIDARG:
        return "An invalid argument was passed to the function. Verify all parameters are within valid ranges.";
    case E_OUTOFMEMORY:
        return "Insufficient memory to complete the operation. Try reducing resource usage or closing other applications.";
    case E_NOTIMPL:
        return "The requested feature is not implemented in the current version.";

        // DXGI specific errors
    case DXGI_ERROR_DEVICE_REMOVED:
        return "The GPU was physically removed, stopped responding, or driver was updated. "
            "Application should release and recreate the device.";
    case DXGI_ERROR_DEVICE_HUNG:
        return "The GPU stopped responding to commands. This may be due to a driver issue or "
            "intensive operations. Application should reset the device.";
    case DXGI_ERROR_DEVICE_RESET:
        return "The GPU was reset by another application or driver update. "
            "Application should recreate the device and reload all resources.";
    case DXGI_ERROR_DRIVER_INTERNAL_ERROR:
        return "GPU driver encountered an internal error. Try updating your graphics drivers.";
    case DXGI_ERROR_INVALID_CALL:
        return "The method call is invalid in the current state. Verify the correct API usage sequence.";
    case DXGI_ERROR_UNSUPPORTED:
        return "The requested feature is not supported by the hardware or driver.";

        // D3D11 specific errors
    case D3D11_ERROR_FILE_NOT_FOUND:
        return "The specified shader or resource file could not be found. Verify file paths and permissions.";
    case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
        return "Too many unique state objects created. Consider reusing existing state objects.";
    case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
        return "Too many unique view objects created. Consider reusing existing view objects.";

    default: {
        std::ostringstream oss;
        oss << "Unhandled error code (0x" << std::hex << std::uppercase << hr << "). "
            << "Please refer to DirectX documentation or check debug output for more details.";
        return oss.str();
    }
    }
}

std::string Graphics::HrException::GetErrorInfo() const noexcept
{
    return info;
}

const char* Graphics::DeviceRemovedException::GetType() const noexcept
{
    return "O Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept
    :
    Exception(line, file)
{
    // join all info messages with newlines into single string
    for (const auto& m : infoMsgs)
    {
        info += m;
        info.push_back('\n');
    }
    // remove final newline if exists
    if (!info.empty())
    {
        info.pop_back();
    }
}

const char* Graphics::InfoException::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "\n[Error Info]\n" << GetErrorInfo() << std::endl << std::endl;
    oss << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Graphics::InfoException::GetType() const noexcept
{
    return "O Graphics Info Exception";
}

std::string Graphics::InfoException::GetErrorInfo() const noexcept
{
    return info;
}
