cmake_minimum_required(VERSION 3.15)

project(UniversalHook LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(UG_ENABLE_D3D9   "Enable D3D9 backend"   ON)
option(UG_ENABLE_D3D10  "Enable D3D10 backend"  OFF)
option(UG_ENABLE_D3D11  "Enable D3D11 backend"  OFF)
option(UG_ENABLE_D3D12  "Enable D3D12 backend"  OFF)
option(UG_ENABLE_OPENGL "Enable OpenGL backend" ON)
option(UG_ENABLE_VULKAN "Enable Vulkan backend" OFF)
option(UG_DEBUG_CONSOLE "Enable debug console/logging" ON)

set(MINHOOK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include/minhook")

set(MINHOOK_SOURCES
    "${MINHOOK_DIR}/buffer.c"
    "${MINHOOK_DIR}/hook.c"
    "${MINHOOK_DIR}/trampoline.c"
)

if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    list(APPEND MINHOOK_SOURCES "${MINHOOK_DIR}/hde64.c")
else()
    list(APPEND MINHOOK_SOURCES "${MINHOOK_DIR}/hde32.c")
endif()

add_library(minhook STATIC ${MINHOOK_SOURCES})
target_include_directories(minhook PUBLIC "${MINHOOK_DIR}")

add_library(imgui STATIC
    "include/imgui/imgui.cpp"
    "include/imgui/imgui_demo.cpp"
    "include/imgui/imgui_draw.cpp"
    "include/imgui/imgui_tables.cpp"
    "include/imgui/imgui_widgets.cpp"

    "include/imgui/imgui_impl_win32.cpp"
    "include/imgui/imgui_impl_opengl3.cpp"
    "include/imgui/imgui_impl_dx9.cpp"
)

target_include_directories(imgui PUBLIC 
    "include/imgui"
)

if(WIN32)
    # d3dx9 removed: it's not part of modern Windows SDKs and isn't
    # needed by imgui_impl_dx9 anyway.
    target_link_libraries(imgui PUBLIC d3d9 dwmapi uxtheme)
endif()



# add_executable(UniversalHook)
add_library(UniversalHook SHARED) # shared for DLL

target_sources(UniversalHook PRIVATE
    src/main.cpp
    src/unihooks.h
    src/unihooks_internal.h
    src/unihooks.cpp
)

if(UG_ENABLE_D3D9)
    target_sources(UniversalHook PRIVATE src/unihooks_d3d9.cpp)
endif()
if(UG_ENABLE_D3D10)
    target_sources(UniversalHook PRIVATE src/unihooks_d3d10.cpp)
endif()
if(UG_ENABLE_D3D11)
    target_sources(UniversalHook PRIVATE src/unihooks_d3d11.cpp)
endif()
if(UG_ENABLE_D3D12)
    target_sources(UniversalHook PRIVATE src/unihooks_d3d12.cpp)
endif()
if(UG_ENABLE_OPENGL)
    target_sources(UniversalHook PRIVATE src/unihooks_opengl.cpp)
endif()
if(UG_ENABLE_VULKAN)
    target_sources(UniversalHook PRIVATE src/unihooks_vulkan.cpp)
endif()


#    UNICODE
#    _UNICODE
target_compile_definitions(UniversalHook PRIVATE
    $<$<BOOL:${UG_ENABLE_D3D9}>:UG_ENABLE_D3D9>
    $<$<BOOL:${UG_ENABLE_D3D10}>:UG_ENABLE_D3D10>
    $<$<BOOL:${UG_ENABLE_D3D11}>:UG_ENABLE_D3D11>
    $<$<BOOL:${UG_ENABLE_D3D12}>:UG_ENABLE_D3D12>
    $<$<BOOL:${UG_ENABLE_OPENGL}>:UG_ENABLE_OPENGL>
    $<$<BOOL:${UG_ENABLE_VULKAN}>:UG_ENABLE_VULKAN>
    $<$<BOOL:${UG_DEBUG_CONSOLE}>:UG_DEBUG=1>
)


#target_sources(UniversalHook PRIVATE 
	#src/main.cpp
	#src/unihooks.h
	#src/unihooks.cpp
    #src/unihooks_d3d9.cpp
    #src/unihooks_d3d10.cpp
    #src/unihooks_d3d11.cpp
    #src/unihooks_d3d12.cpp
    #src/unihooks_opengl.cpp
    #src/unihooks_vulkan.cpp
#)

target_link_libraries(UniversalHook PRIVATE minhook imgui)