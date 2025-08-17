cmake_minimum_required(VERSION 3.1)

project(Peanut
    VERSION 0.1
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)

find_program(CCACHE_EXECUTABLE ccache)
if(CCACHE_EXECUTABLE)
  message(STATUS "ccache found: ${CCACHE_EXECUTABLE}")
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_EXECUTABLE})
else()
  message(WARNING "ccache not found.")
endif()

set(LINT_WITH_CLANG_TIDY OFF CACHE BOOL "Use clang-tidy to perform static analysis when building")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# CPM Dependencies
include("cmake/CPM.cmake")
set(CPM_SOURCE_CACHE ".cpm_cache")
CPMAddPackage(URI "gh:gabime/spdlog@1.15.3"
    OPTIONS "SPDLOG_STATIC_LIB ON" "SPDLOG_BUILD_PIC ON")
CPMAddPackage(URI "gh:skypjack/entt@3.11.1")
CPMAddPackage(URI "gh:pybind/pybind11@2.10.4")
CPMAddPackage(URI "gh:jbeder/yaml-cpp#yaml-cpp-0.7.0")
CPMAddPackage(URI "gh:pkgconf/pkgconf#pkgconf-1.9.3")
CPMAddPackage(URI "gh:g-truc/glm#0.9.9.8")
CPMAddPackage(URI "gh:Dav1dde/glad@0.1.36")
CPMAddPackage(URI "gh:assimp/assimp@5.4.3"
    OPTIONS
        "ASSIMP_BUILD_ASSIMP_TOOLS OFF"
        "ASSIMP_WARNINGS_AS_ERRORS OFF"
        "BUILD_SHARED_LIBS OFF")
CPMAddPackage(URI "gh:glfw/glfw#3.3.8")
CPMAddPackage(NAME imgui
    GIT_REPOSITORY "https://github.com/ocornut/imgui"
    GIT_TAG "v1.89.9-docking"
    DOWNLOAD_ONLY YES)
if (imgui_ADDED)
    message("imgui found at :${imgui_SOURCE_DIR}")
    add_library(imgui STATIC)
    target_sources(imgui PRIVATE
        "${imgui_SOURCE_DIR}/backends/imgui_impl_glfw.cpp"
        "${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp"
        "${imgui_SOURCE_DIR}/imgui.cpp"
        "${imgui_SOURCE_DIR}/imgui_draw.cpp"
        "${imgui_SOURCE_DIR}/imgui_tables.cpp"
        "${imgui_SOURCE_DIR}/imgui_widgets.cpp")
    target_include_directories(imgui PUBLIC "${imgui_SOURCE_DIR}/backends" "${imgui_SOURCE_DIR}")
    target_link_libraries(imgui PRIVATE glfw)
endif()

if ("${LINT_WITH_CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy --extra-arg=-std=c++20)
endif()
add_subdirectory(apps)
add_subdirectory(src)
unset(CMAKE_CXX_CLANG_TIDY)