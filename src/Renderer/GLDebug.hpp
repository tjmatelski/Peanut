#pragma once
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>

// stl
#include <iostream>
#include <string>

GLenum glCheckError_(const char* file, int line);
#define GL_CHECK_ERROR() glCheckError_(__FILE__, __LINE__)

#define GLCALL(loc) \
    loc;            \
    GL_CHECK_ERROR()
