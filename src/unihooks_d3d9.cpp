#include "unihooks_internal.h"

#include <dxgi.h>
#include <d3d9.h>

#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

namespace hooks
{
	Status initD3D9(uint_t*& table)
	{
		DummyWindow win;

		if (!win)
			return hooks::UnknownError;

		HMODULE libD3D9;
		if ((libD3D9 = ::GetModuleHandle(U_TEXT("d3d9.dll"))) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: libD3D9 = GetModuleHandle");
			return ModuleNotFoundError;
		}

		void* Direct3DCreate9;
		if ((Direct3DCreate9 = ::GetProcAddress(libD3D9, "Direct3DCreate9")) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: Direct3DCreate9 = GetProcAddress");
			return UnknownError;
		}

		LPDIRECT3D9 direct3D9;
		if ((direct3D9 = ((LPDIRECT3D9(__stdcall*)(uint32_t))(Direct3DCreate9))(D3D_SDK_VERSION)) == NULL)
		{
			DEBUG_PRINT("[!] UnknownError: direct3D9 = Direct3DCreate9");
			return UnknownError;
		}

		D3DDISPLAYMODE displayMode;
		if (direct3D9->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &displayMode) < 0)
		{
			DEBUG_PRINT("[!] UnknownError: GetAdapterDisplayMode");
			return UnknownError;
		}

		D3DPRESENT_PARAMETERS params;
		params.BackBufferWidth = 0;
		params.BackBufferHeight = 0;
		params.BackBufferFormat = displayMode.Format;
		params.BackBufferCount = 0;
		params.MultiSampleType = D3DMULTISAMPLE_NONE;
		params.MultiSampleQuality = NULL;
		params.SwapEffect = D3DSWAPEFFECT_DISCARD;
		params.hDeviceWindow = win.hwnd;
		params.Windowed = 1;
		params.EnableAutoDepthStencil = 0;
		params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
		params.Flags = NULL;
		params.FullScreen_RefreshRateInHz = 0;
		params.PresentationInterval = 0;

		LPDIRECT3DDEVICE9 device;
		if (direct3D9->CreateDevice(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, 
			win.hwnd, 
			D3DCREATE_SOFTWARE_VERTEXPROCESSING | 
			D3DCREATE_DISABLE_DRIVER_MANAGEMENT, 
			&params, &device) < 0)
		{
			direct3D9->Release();
			DEBUG_PRINT("[!] UnknownError: CreateDevice");
			return UnknownError;
		}

		g_methodsTable = (uint_t*)::calloc(119, sizeof(uint_t));
		::memcpy(g_methodsTable, *(uint_t**)device, 119 * sizeof(uint_t));

		MH_Initialize();

		direct3D9->Release();
		direct3D9 = NULL;

		device->Release();
		device = NULL;

		g_renderType = GraphicsType::D3D9;

		return Success;
	}
}
