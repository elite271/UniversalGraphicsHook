#pragma once
#define WIN32_LEAN_AND_MEAN

#include <stdint.h>
#include <Windows.h>
#include <assert.h>
#include <iostream>
#include <mutex>
#include <vector>

#include <MinHook.h>


namespace hooks
{
	using uint_t = uintptr_t;

	enum Status
	{
		UnknownError = -1,
		NotSupportedError = -2,
		ModuleNotFoundError = -3,

		AlreadyInitializedError = -4,
		NotInitializedError = -5,

		MHError = -6,

		NotImplemented = -7,

		Success = 0,
	};

	enum class GraphicsType : uint8_t
	{
		Unknown,

		D3D9,
		D3D10,
		D3D11,
		D3D12,

		GDI,
		GDIPLUS,

		OpenGL,
		Vulkan,

		Auto
	};

	extern GraphicsType g_renderType;
	extern uint_t* g_methodsTable;

	Status init(GraphicsType _renderType = GraphicsType::Auto);
	void shutdown();

	Status bind(uint16_t index, void** original, void* function);
	void unbind(uint16_t index);

	GraphicsType getRenderType();
	uint_t* getMethodsTable();
}
