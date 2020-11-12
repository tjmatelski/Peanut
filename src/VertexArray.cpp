#include <VertexArray.h>
#include <glad/glad.h>
#include <Log.h>

#include "GLDebug.h"

VertexArray::VertexArray() : m_ID(0)
{
    glGenVertexArrays(1, &m_ID);
}

void VertexArray::Bind() const
{
    glBindVertexArray(m_ID);
}

void VertexArray::AddBuffer(const VertexBuffer &vb, const BufferLayout &bl)
{
    Bind();
    vb.Bind();

    unsigned int attribute = 0;
    const unsigned int stride = bl.GetStride();
    const auto &elements = bl.GetElements();
    unsigned int offset = 0;

    for (const auto &element : elements)
    {
        GLCALL(glVertexAttribPointer(attribute, element.size, element.type, element.normalized, stride, (void *)offset));
        GLCALL(glEnableVertexAttribArray(attribute));
        ++attribute;
        offset += element.size * BufferLayout::Element::GetSizeOfGLType(element.type);
    }
}