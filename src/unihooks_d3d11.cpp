#include "unihooks_internal.h"

#include <dxgi.h>
#include <d3d11.h>

namespace hooks
{
	Status initDXD11(GraphicsType _renderType)
	{
		DummyWindow win;

		if (!win)
			return hooks::UnknownError;

		HMODULE libD3D11;
		if ((libD3D11 = ::GetModuleHandle(U_TEXT("d3d11.dll"))) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: libD3D11 = GetModuleHandle");
			return ModuleNotFoundError;
		}

		void* D3D11CreateDeviceAndSwapChain;
		if ((D3D11CreateDeviceAndSwapChain = ::GetProcAddress(libD3D11, "D3D11CreateDeviceAndSwapChain")) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: CreateDeviceAndSwapChain = GetProcAddress");
			return UnknownError;
		}

		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };

		DXGI_RATIONAL refreshRate;
		refreshRate.Numerator = 60;
		refreshRate.Denominator = 1;

		DXGI_MODE_DESC bufferDesc;
		bufferDesc.Width = 100;
		bufferDesc.Height = 100;
		bufferDesc.RefreshRate = refreshRate;
		bufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		bufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		bufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

		DXGI_SAMPLE_DESC sampleDesc;
		sampleDesc.Count = 1;
		sampleDesc.Quality = 0;

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		swapChainDesc.BufferDesc = bufferDesc;
		swapChainDesc.SampleDesc = sampleDesc;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 1;
		swapChainDesc.OutputWindow = win.hwnd;
		swapChainDesc.Windowed = 1;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain* swapChain;
		ID3D11Device* device;
		ID3D11DeviceContext* context;

		if (((long(__stdcall*)(
			IDXGIAdapter*,
			D3D_DRIVER_TYPE,
			HMODULE,
			UINT,
			const D3D_FEATURE_LEVEL*,
			UINT,
			UINT,
			const DXGI_SWAP_CHAIN_DESC*,
			IDXGISwapChain**,
			ID3D11Device**,
			D3D_FEATURE_LEVEL*,
			ID3D11DeviceContext**))(D3D11CreateDeviceAndSwapChain))(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, featureLevels, 1, D3D11_SDK_VERSION, &swapChainDesc, &swapChain, &device, &featureLevel, &context) < 0)
		{
			DEBUG_PRINT("[!] UnknownError: CreateDeviceAndSwapChain");
			return UnknownError;
		}

		g_methodsTable = (uint_t*)::calloc(205, sizeof(uint_t));
		::memcpy(g_methodsTable, *(uint_t**)swapChain, 18 * sizeof(uint_t));
		::memcpy(g_methodsTable + 18, *(uint_t**)device, 43 * sizeof(uint_t));
		::memcpy(g_methodsTable + 18 + 43, *(uint_t**)context, 144 * sizeof(uint_t));

		MH_Initialize();

		swapChain->Release();
		swapChain = NULL;

		device->Release();
		device = NULL;

		context->Release();
		context = NULL;

		g_renderType = GraphicsType::D3D11;

		return Success;
	}
}