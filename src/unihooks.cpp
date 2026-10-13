#include "unihooks.h"
#include "unihooks_internal.h"

namespace hooks
{
	GraphicsType g_renderType = GraphicsType::Unknown;
	uint_t* g_methodsTable = NULL;

	GraphicsType DetectBackend()
	{
		GraphicsType type = GraphicsType::Unknown;

		if (::GetModuleHandle(U_TEXT("d3d9.dll")) != NULL)
		{
			type = GraphicsType::D3D9;
			DEBUG_PRINT("[+] Detected d3d9 graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("d3d10.dll")) != NULL)
		{
			type = GraphicsType::D3D10;
			DEBUG_PRINT("[+] Detected d3d10 graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("d3d11.dll")) != NULL)
		{
			type = GraphicsType::D3D11;
			DEBUG_PRINT("[+] Detected d3d11 graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("d3d12.dll")) != NULL)
		{
			type = GraphicsType::D3D12;
			DEBUG_PRINT("[+] Detected d3d12 graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("opengl32.dll")) != NULL)
		{
			type = GraphicsType::OpenGL;
			DEBUG_PRINT("[+] Detected opengl graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("vulkan-1.dll")) != NULL)
		{
			type = GraphicsType::Vulkan;
			DEBUG_PRINT("[+] Detected vulkan graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("gdi32.dll")) != NULL)
		{
			type = GraphicsType::GDI;
			DEBUG_PRINT("[+] Detected gdi graphics library, initializing hook!");
		}
		else if (::GetModuleHandle(U_TEXT("gdiplus.dll")) != NULL)
		{
			type = GraphicsType::GDIPLUS;
			DEBUG_PRINT("[+] Detected gdiplus graphics library, initializing hook!");
		}
		else {
			type = GraphicsType::Unknown;
			DEBUG_PRINT("[!] Could not find a supported Graphics library.");
		}

		return type;
	}

	Status init(GraphicsType _renderType)
	{
		static bool first = true;
#if UG_DEBUG
		if (first)
		{
			first = false;

			AllocConsole();
			auto _ = freopen("CONOUT$", "w", stdout);
			DEBUG_PRINT("[+] Debugging Console Initialized!");
		}
#endif

		if (g_renderType != GraphicsType::Unknown)
		{
			DEBUG_PRINT("[!] Already Initialized Error");
			return AlreadyInitializedError;
		}

		if (_renderType == GraphicsType::Auto)
		{
			 _renderType = DetectBackend();
		}


		Status s = NotSupportedError;
		switch (_renderType) 
		{
#ifdef UG_ENABLE_D3D9
			case GraphicsType::D3D9: 
				s = initD3D9(g_methodsTable);  
				break;
#endif
#ifdef UG_ENABLE_D3D10
			case GraphicsType::D3D10:
				s = initD3D10(g_methodsTable);
				break;
#endif
#ifdef UG_ENABLE_D3D11
			case GraphicsType::D3D11: 
				s = initD3D11(g_methodsTable);
				break;
#endif
#ifdef UG_ENABLE_D3D12
			case GraphicsType::D3D12:
				s = initD3D12(g_methodsTable);
				break;
#endif
#ifdef UG_ENABLE_OPENGL
			case GraphicsType::OpenGL:
				s = initOpenGl(g_methodsTable);
				break;
#endif
#ifdef UG_ENABLE_VULKAN
			case GraphicsType::Vulkan:
				s = initVulkan(g_methodsTable);
				break;
#endif
			default:
				DEBUG_PRINT("[!] Init failed");
				break;
		}

		if (s == Success)
		{
			MH_Initialize(); 
			g_renderType = _renderType;
		}

		return s;
	}

	void shutdown()
	{
		if (g_renderType != GraphicsType::Unknown)
		{
			MH_DisableHook(MH_ALL_HOOKS);

			::free(g_methodsTable);
			g_methodsTable = NULL;
			g_renderType = GraphicsType::Unknown;

			DEBUG_PRINT("[+] shutdown: Successfully shutdown all mh hooks, methods table and render type!");
		}
	}

	Status bind(uint16_t _index, void** _original, void* _function)
	{
		assert(_index >= 0 && _original != NULL && _function != NULL);

		if (g_renderType != GraphicsType::Unknown)
		{
			void* target = (void*)g_methodsTable[_index];
			if (MH_CreateHook(target, _function, _original) != MH_OK || MH_EnableHook(target) != MH_OK)
			{
				DEBUG_PRINT("[!] bind (target: " << target << ", _index: " << _index << ", _original: " << _original << ", _function: " << _function << "): MH Error!");
				return MHError;
			}

			DEBUG_PRINT("[+] bind (target: " << target << ", _index: " << _index << ", _original: " << _original << ", _function: " << _function << "): Success!");
			return Success;
		}

		DEBUG_PRINT("[!] bind (_index: " << _index << ", _original: " << _original << ", _function: " << _function << "): Not Initialized Error!");
		return NotInitializedError;
	}

	void unbind(uint16_t _index)
	{
		assert(_index >= 0);

		if (g_renderType != GraphicsType::Unknown)
		{
			MH_DisableHook((void*)g_methodsTable[_index]);
			DEBUG_PRINT("[+] unbind (_index: " << _index << "): Successfully unbound method!");
		}
	}

	GraphicsType getRenderType()
	{
		return g_renderType;
	}

	uint_t* getMethodsTable()
	{
		return g_methodsTable;
	}

} // namespace hooks