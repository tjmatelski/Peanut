#include <Renderer/VertexArray.h>
#include <Utils/Log.h>
#include <glad/glad.h>

#include "GLDebug.h"

namespace PEANUT {

VertexArray::VertexArray()
    : m_ID(0)
{
    GLCALL(glGenVertexArrays(1, &m_ID));
    LOG_TRACE("Generated vertex array: {}", m_ID);
}

VertexArray::VertexArray(VertexArray&& other)
{
    *this = std::move(other);
}

VertexArray& VertexArray::operator=(VertexArray&& rhs)
{
    m_ID = rhs.m_ID;
    m_vertexBuffers = std::move(rhs.m_vertexBuffers);
    rhs.m_ID = 0;
    return *this;
}

VertexArray::~VertexArray()
{
    if (m_ID != 0) {
        LOG_TRACE("Deleting vertex array: {}", m_ID);
        GLCALL(glDeleteVertexArrays(1, &m_ID));
    }
}

void VertexArray::Bind() const
{
    GLCALL(glBindVertexArray(m_ID));
}

void VertexArray::AddBuffer(VertexBuffer&& vb, const BufferLayout& bl)
{

    Bind();
    vb.Bind();
    m_vertexBuffers.push_back(std::move(vb));

    unsigned int attribute = 0;
    const unsigned int stride = bl.GetStride();
    const auto& elements = bl.GetElements();
    unsigned char* offset = nullptr;

    for (const auto& element : elements) {
        GLCALL(glVertexAttribPointer(attribute, element.size, element.type, element.normalized, stride, static_cast<void*>(offset)));
        GLCALL(glEnableVertexAttribArray(attribute));
        ++attribute;
        offset += static_cast<std::size_t>(element.size * BufferLayout::Element::GetSizeOfGLType(element.type));
    }
}

}