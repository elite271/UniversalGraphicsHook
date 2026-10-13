#pragma once

#include "unihooks.h"

#ifndef U_DEBUG
# define U_DEBUG 0
#endif

#define DEBUG_PRINT(fmt, ...) \
            do { if (U_DEBUG) std::cout << fmt << std::endl; } while (0)

#ifdef _UNICODE
# define U_TEXT(text) L##text
#else
# define U_TEXT(text) text
#endif

#define U_ARRAY_SIZE(arr) ((size_t)(sizeof(arr)/sizeof(arr[0])))


namespace hooks
{
	struct DummyWindow
	{
		HWND hwnd = NULL;
		WNDCLASSEX wc{};

		DummyWindow()
		{
			wc = {
				sizeof(WNDCLASSEX),
				CS_HREDRAW | CS_VREDRAW,
				DefWindowProc, 0, 0,
				GetModuleHandle(NULL),
				NULL, NULL, NULL, NULL,
				U_TEXT("UG"), NULL
			};

			if (!RegisterClassEx(&wc))
				return;   // hwnd stays NULL

			hwnd = CreateWindow(
				wc.lpszClassName,
				U_TEXT("UG DirectX Window"),
				WS_OVERLAPPEDWINDOW,
				0, 0, 100, 100,
				NULL, NULL, wc.hInstance, NULL);

			if (!hwnd)
				UnregisterClass(wc.lpszClassName, wc.hInstance);
		}

		~DummyWindow()
		{
			if (!hwnd) // nothing to clean up
				return;

			DestroyWindow(hwnd);
			UnregisterClass(wc.lpszClassName, wc.hInstance);
		}

		explicit operator bool() const { return hwnd != NULL; }

		DummyWindow(const DummyWindow&) = delete;            // no accidental copies
		DummyWindow& operator=(const DummyWindow&) = delete;
	};

	Status initD3D9(uint_t*& table);
	Status initD3D10(uint_t*& table);
	Status initD3D11(uint_t*& table);
	Status initD3D12(uint_t*& table);
	Status initOpenGl(uint_t*& table);
	Status initVulkan(uint_t*& table);
}