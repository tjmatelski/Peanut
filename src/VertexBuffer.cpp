#include <VertexBuffer.h>
#include <glad/glad.h>

VertexBuffer::VertexBuffer(const unsigned int size, const void* data) : m_ID(0)
{
    glGenBuffers(1, &m_ID);
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void VertexBuffer::Bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, m_ID);
}