#pragma once

#include <vector>

class BufferLayout
{
public:
    BufferLayout();
    template<typename T>
    void Push(const unsigned int size);
    inline unsigned int GetStride() { return m_stride; }

    struct Element
    {
        unsigned int size;
        unsigned int typeSize;
        unsigned char normalized;
    };
    inline const std::vector<Element>& GetElements() { return m_elements; }

private:
    unsigned int m_stride;
    std::vector<Element> m_elements;

private:
    unsigned int GetSizeOfGLType(const unsigned int glTYpe);
};