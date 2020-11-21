#pragma once

#include <vector>

namespace PEANUT
{

class BufferLayout
{
public:
    BufferLayout();
    template<typename T>
    void Push(const unsigned int size);
    inline unsigned int GetStride() const { return m_stride; }

    struct Element
    {
        unsigned int size;
        unsigned int type;
        unsigned char normalized;

        static unsigned int GetSizeOfGLType(const unsigned int glType);
    };
    inline const std::vector<Element>& GetElements() const { return m_elements; }

private:
    unsigned int m_stride;
    std::vector<Element> m_elements;
};

}