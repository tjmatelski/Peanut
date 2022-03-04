cmake_minimum_required(VERSION 3.1)

project(PeanutScript
    VERSION 0.1
    LANGUAGES CXX)

add_library(TestScript SHARED TestScript.cpp)
target_include_directories(TestScript PRIVATE ${PEANUT_INCLUDE_DIR})
target_compile_options(TestScript PRIVATE -fPIC)

install(TARGETS TestScript DESTINATION lib)