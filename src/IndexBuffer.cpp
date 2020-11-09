#include <IndexBuffer.h>
#include <glad/glad.h>

#include "GLDebug.h"

IndexBuffer::IndexBuffer(const unsigned int elements, const void* data) : m_ID(0)
{
    static_assert(sizeof(unsigned int) == sizeof(GLuint));

    GLCALL(glGenBuffers(1, &m_ID));
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

void IndexBuffer::Bind() const 
{
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
}