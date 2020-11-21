#include <Renderer/BufferLayout.h>
#include <glad/glad.h>

#include <cassert>

namespace PEANUT
{

BufferLayout::BufferLayout() : m_stride(0), m_elements()
{}

template<typename T>
void BufferLayout::Push(const unsigned int size)
{
    assert(false);
}

template<>
void BufferLayout::Push<float>(const unsigned int size)
{
    m_elements.push_back({size, GL_FLOAT, GL_FALSE});
    m_stride += size * Element::GetSizeOfGLType(GL_FLOAT);
}

unsigned int BufferLayout::Element::GetSizeOfGLType(const unsigned int glType)
{
    switch (glType)
    {
        case GL_FLOAT: return 4;
    }
    assert(false);
    return 0;
}

}