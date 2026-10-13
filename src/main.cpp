#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <iostream>

#include "unihooks.h"

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


typedef BOOL(WINAPI* wglSwapBuffers_t)(HDC);
wglSwapBuffers_t oWglSwapBuffers = nullptr;
WNDPROC oWndProc = nullptr;

LRESULT CALLBACK hkWndProc(HWND h, UINT msg, WPARAM w, LPARAM l)
{
    ImGui_ImplWin32_WndProcHandler(h, msg, w, l);
    return CallWindowProc(oWndProc, h, msg, w, l);
}


BOOL WINAPI hkWglSwapBuffers(HDC hdc)
{
    static bool imguiInit = false;
    if (!imguiInit)
    {
        HWND hwnd = WindowFromDC(hdc);
        ImGui::CreateContext();
        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplOpenGL3_Init("#version 130");
        oWndProc = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
        imguiInit = true;
    }

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("menu");
    {
        // render loop for menu
        ImGui::Text("");
    }
    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    return oWglSwapBuffers(hdc);
}

DWORD WINAPI MyWorkerThread(LPVOID lpParam) 
{
    HMODULE hModule = reinterpret_cast<HMODULE>(lpParam);

    if (hooks::init() != hooks::Success)
    {
        MessageBox(nullptr, "Error", "Init failed", 0);
        return FALSE;
    }

    if (hooks::bind(0 /* wglSwapBuffers index */,
        (void**)&oWglSwapBuffers, hkWglSwapBuffers) != hooks::Success)
    {
        MessageBox(nullptr, "Error", "Function binding failed", 0);
        return FALSE;
    }

    MessageBox(nullptr, "Success", "Success", 0);

    FreeLibraryAndExitThread(hModule, 0);
}


BOOL WINAPI DllMain(
    HINSTANCE hinstDLL,
    DWORD fdwReason,
    LPVOID lpvReserved)
{
    switch (fdwReason)
    {
        case DLL_PROCESS_ATTACH:
        {
            DisableThreadLibraryCalls(hinstDLL);

            HMODULE hSelf = NULL;

            if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                (LPCWSTR)DllMain, &hSelf)) 
            {

                HANDLE hThread = CreateThread(NULL, 0, MyWorkerThread, hSelf, 0, NULL);

                if (hThread) 
                {
                    CloseHandle(hThread);
                }
                else
                {
                    FreeLibrary(hSelf);
                }
            }

            break;
        }

        case DLL_THREAD_ATTACH:
        {
            break;
        }

        case DLL_THREAD_DETACH:
        {
            break;
        }

        case DLL_PROCESS_DETACH:
        {
            if (lpvReserved != nullptr)
            {
                break;
            }
            break;
        }
    }

    return TRUE;
}