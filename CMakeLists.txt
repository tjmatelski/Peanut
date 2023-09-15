cmake_minimum_required(VERSION 3.1)

project(Peanut
    VERSION 0.1
    LANGUAGES CXX)

set(LINT_WITH_CLANG_TIDY OFF CACHE BOOL "Use clang-tidy to perform static analysis when building")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
  message(STATUS "Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
  file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                TLS_VERIFY ON)
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
    REQUIRES
        assimp/5.2.2
        entt/3.11.1
        glad/0.1.36
        glfw/3.3.8
        glm/0.9.9.8
        imgui/cci.20220621+1.88.docking
        lua/5.4.4
        pkgconf/1.9.3
        sol2/3.3.0
        spdlog/1.11.0
        yaml-cpp/0.7.0
    GENERATORS
        cmake_find_package_multi
        CMakeToolchain
    IMPORTS
        "res/bindings, imgui_impl_glfw.cpp -> ./imports"
        "res/bindings, imgui_impl_opengl3.cpp -> ./imports"
        "res/bindings, imgui_impl_glfw.h -> ./imports"
        "res/bindings, imgui_impl_opengl3.h -> ./imports"
        "res/bindings, imgui_impl_opengl3_loader.h -> ./imports"
)

conan_cmake_autodetect(settings)

conan_cmake_install(
    PATH_OR_REFERENCE .
    BUILD missing
    REMOTE conancenter
    SETTINGS ${settings}
    INSTALL_FOLDER ${CMAKE_BINARY_DIR}
    OUTPUT_FOLDER ${CMAKE_BINARY_DIR}
)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if ("${LINT_WITH_CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy --extra-arg=-std=c++17)
endif()
add_subdirectory(apps)
add_subdirectory(src)
unset(CMAKE_CXX_CLANG_TIDY)