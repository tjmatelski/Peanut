cmake_minimum_required(VERSION 3.1)

project(Peanut
    VERSION 0.1
    LANGUAGES CXX)

set(LINT_WITH_CLANG_TIDY OFF CACHE BOOL "Use clang-tidy to perform static analysis when building")

if ("${LINT_WITH_CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy --extra-arg=-std=c++17)
endif()
add_subdirectory(apps)
add_subdirectory(src)
unset(CMAKE_CXX_CLANG_TIDY)

add_subdirectory(extern/imgui)