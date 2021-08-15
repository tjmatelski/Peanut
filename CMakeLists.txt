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
add_subdirectory(extern/imgui)
option(SPDLOG_BUILD_SHARED "" ON)
add_subdirectory(extern/spdlog)
add_subdirectory(extern/yaml-cpp)

add_library(TestScript SHARED res/scripts/TestScript.cpp)
target_link_libraries(TestScript PRIVATE Peanut)
#target_include_directories(TestScript PRIVATE extern/entt include extern)
#target_compile_features(TestScript PUBLIC cxx_std_17)
target_compile_options(TestScript PRIVATE -fPIC)