#include "GLDebug.h"
#include <Renderer/VertexBuffer.h>
#include <glad/glad.h>

namespace PEANUT {

VertexBuffer::VertexBuffer(const unsigned int size, const void* data)
    : m_ID(0)
{
    GLCALL(glGenBuffers(1, &m_ID));
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

void VertexBuffer::Bind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
}

}