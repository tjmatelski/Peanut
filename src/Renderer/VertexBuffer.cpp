#include "VertexBuffer.hpp"

#include "GLDebug.hpp"
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>

// stl
#include <utility>

namespace PEANUT {

VertexBuffer::VertexBuffer(const unsigned int size, const void* data)
{
    GLCALL(glGenBuffers(1, &m_ID));
    LOG_TRACE("Generated vertex buffer: {}", m_ID);
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
    GLCALL(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
}

VertexBuffer::VertexBuffer(VertexBuffer&& other)
{
    *this = std::move(other);
}

VertexBuffer& VertexBuffer::operator=(VertexBuffer&& rhs)
{
    m_ID = rhs.m_ID;
    rhs.m_ID = 0;
    return *this;
}

VertexBuffer::~VertexBuffer()
{
    if (m_ID != 0) {
        LOG_TRACE("Deleting vertex buffer: {}", m_ID);
        GLCALL(glDeleteBuffers(1, &m_ID));
    }
}

void VertexBuffer::Bind() const
{
    GLCALL(glBindBuffer(GL_ARRAY_BUFFER, m_ID));
}

}