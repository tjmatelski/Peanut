#include <Log.h>
#include <Renderer/VertexArray.h>
#include <glad/glad.h>

#include "GLDebug.h"

namespace PEANUT {

VertexArray::VertexArray()
    : m_ID(0)
{
    glGenVertexArrays(1, &m_ID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_ID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const BufferLayout& bl)
{
    Bind();
    vb.Bind();

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