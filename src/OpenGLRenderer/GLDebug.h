#pragma once
#include <glad/glad.h>
#include <Log.h>

#include <string>
#include <iostream>

GLenum glCheckError_(const char *file, int line);
#define GL_CHECK_ERROR() glCheckError_(__FILE__, __LINE__)

#define GLCALL(loc) loc; GL_CHECK_ERROR()