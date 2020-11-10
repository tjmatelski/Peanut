#pragma once

class IndexBuffer
{
public:
    IndexBuffer(const unsigned int elements, const void* data);
    void Bind() const;
    inline unsigned int GetElementCount() const { return m_elementCount; }

private:
    unsigned int m_ID;
    unsigned int m_elementCount;
};