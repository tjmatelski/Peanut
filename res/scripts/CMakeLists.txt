cmake_minimum_required(VERSION 3.1)

project(PeanutScript
    VERSION 0.1
    LANGUAGES CXX)

add_library(${PEANUT_SCRIPT_NAME} SHARED ${PEANUT_SCRIPT_FILE})
target_include_directories(${PEANUT_SCRIPT_NAME} PRIVATE ${PEANUT_INCLUDE_DIR})
target_compile_options(${PEANUT_SCRIPT_NAME} PRIVATE -fPIC)

install(TARGETS ${PEANUT_SCRIPT_NAME} DESTINATION lib)