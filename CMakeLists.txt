cmake_minimum_required(VERSION 3.1)

project(Peanut
    VERSION 0.1
    LANGUAGES CXX)

add_subdirectory(apps)
add_subdirectory(src)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(extern/glfw)
add_subdirectory(extern/glad)
add_subdirectory(extern/spdlog)