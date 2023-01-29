cmake_minimum_required(VERSION 3.1)

project(Peanut
    VERSION 0.1
    LANGUAGES CXX)

set(LINT_WITH_CLANG_TIDY OFF CACHE BOOL "Use clang-tidy to perform static analysis when building")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

execute_process(
    COMMAND conan install ${CMAKE_SOURCE_DIR} -s build_type=${CMAKE_BUILD_TYPE} -of ${CMAKE_BINARY_DIR} -if ${CMAKE_BINARY_DIR}
)
execute_process(
    COMMAND conan imports -if ${CMAKE_BINARY_DIR} -imf ${CMAKE_BINARY_DIR} ${CMAKE_SOURCE_DIR}
)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

if ("${LINT_WITH_CLANG_TIDY}")
    set(CMAKE_CXX_CLANG_TIDY clang-tidy --extra-arg=-std=c++17)
endif()
add_subdirectory(apps)
add_subdirectory(src)
unset(CMAKE_CXX_CLANG_TIDY)