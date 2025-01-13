#pragma once
#include "OWin/OWin.h"
#include "Exception/OException.h"
#include <d3d11.h>
#include <string>
#include <vector>

class Graphics
{
public:
	class Exception : public OException
	{
		using OException::OException;
	};
	class HrException : public Exception
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs = {}) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
		std::string GetErrorDescription() const noexcept;
		std::string GetErrorInfo() const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class InfoException : public Exception
	{
	public:
		InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept;
		const char* what() const noexcept override;
		const char* GetType() const noexcept override;
		std::string GetErrorInfo() const noexcept;
	private:
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType() const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics(HWND hWnd, int width, int height);
	~Graphics();

	// No need to have a copy constructor
	Graphics(const Graphics&) = delete;
	Graphics& operator =(const Graphics&) = delete;

	void EndFrame();
	void ClearBuffer(float r, float g, float b) noexcept
	{
		const float color[]{ r, g, b, 1.0f };
		pContext->ClearRenderTargetView(pTarget, color);
	};

private:
	ID3D11Device* pDevice = nullptr;
	IDXGISwapChain* pSwap = nullptr;
	ID3D11DeviceContext* pContext = nullptr;
	ID3D11RenderTargetView* pTarget = nullptr;
};