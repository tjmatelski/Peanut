#include "IndexBuffer.hpp"

#include "GLDebug.hpp"
#include <peanut/Log.hpp>

// external
#include <glad/glad.h>

namespace PEANUT {

IndexBuffer::IndexBuffer(const unsigned int elements, const void* data)
    : m_ID(0)
    , m_elementCount(elements)
{
    static_assert(sizeof(unsigned int) == sizeof(GLuint));

    GLCALL(glGenBuffers(1, &m_ID));
    LOG_TRACE("Generated index buffer {}", m_ID);
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
    GLCALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, elements * sizeof(unsigned int), data, GL_STATIC_DRAW));
}

IndexBuffer::IndexBuffer(IndexBuffer&& other)
{
    *this = std::move(other);
}

IndexBuffer& IndexBuffer::operator=(IndexBuffer&& rhs)
{
    m_ID = rhs.m_ID;
    m_elementCount = rhs.m_elementCount;
    rhs.m_ID = 0;
    return *this;
}

IndexBuffer::~IndexBuffer()
{
    if (m_ID != 0) {
        LOG_TRACE("Deleting index buffer: {}", m_ID);
        GLCALL(glDeleteBuffers(1, &m_ID));
    }
}

void IndexBuffer::Bind() const
{
    GLCALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ID));
}

}