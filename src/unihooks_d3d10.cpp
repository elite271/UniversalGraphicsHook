#include "unihooks_internal.h"

#include <d3d10_1.h>
#include <d3d10.h>

namespace ug
{
	Status initDXD10(GraphicsType _renderType)
	{
		DummyWindow win;

		if (!win)
			return hooks::UnknownError;

		HMODULE libDXGI;
		HMODULE libD3D10;
		if ((libDXGI = ::GetModuleHandle(U_TEXT("dxgi.dll"))) == NULL || (libD3D10 = ::GetModuleHandle(U_TEXT("d3d10.dll"))) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: libDXGI = GetModuleHandle");
			return ModuleNotFoundError;
		}

		void* CreateDXGIFactory;
		if ((CreateDXGIFactory = ::GetProcAddress(libDXGI, "CreateDXGIFactory")) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: CreateDXGIFactory = GetProcAddress");
			return UnknownError;
		}

		IDXGIFactory* factory;
		if (((long(__stdcall*)(const IID&, void**))(CreateDXGIFactory))(__uuidof(IDXGIFactory), (void**)&factory) < 0)
		{
			DEBUG_PRINT("[!] UnknownError: CreateDXGIFactory");
			return UnknownError;
		}

		IDXGIAdapter* adapter;
		if (factory->EnumAdapters(0, &adapter) == DXGI_ERROR_NOT_FOUND)
		{
			DEBUG_PRINT("[!] UnknownError: EnumAdapters");
			return UnknownError;
		}

		void* D3D10CreateDeviceAndSwapChain;
		if ((D3D10CreateDeviceAndSwapChain = ::GetProcAddress(libD3D10, "D3D10CreateDeviceAndSwapChain")) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: CreateDeviceAndSwapChain = GetProcAddress");
			return UnknownError;
		}

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
		ID3D10Device* device;

		if (((long(__stdcall*)(
			IDXGIAdapter*,
			D3D10_DRIVER_TYPE,
			HMODULE,
			UINT,
			UINT,
			DXGI_SWAP_CHAIN_DESC*,
			IDXGISwapChain**,
			ID3D10Device**))(D3D10CreateDeviceAndSwapChain))(adapter, D3D10_DRIVER_TYPE_HARDWARE, NULL, 0, D3D10_SDK_VERSION, &swapChainDesc, &swapChain, &device) < 0)
		{
			DEBUG_PRINT("[!] UnknownError: CreateDeviceAndSwapChain");
			return UnknownError;
		}

		g_methodsTable = (uint_t*)::calloc(116, sizeof(uint_t));
		::memcpy(g_methodsTable, *(uint_t**)swapChain, 18 * sizeof(uint_t));
		::memcpy(g_methodsTable + 18, *(uint_t**)device, 98 * sizeof(uint_t));

		MH_Initialize();

		swapChain->Release();
		swapChain = NULL;

		device->Release();
		device = NULL;

		g_renderType = GraphicsType::D3D10;

		return Success;
	}
}