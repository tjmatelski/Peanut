#pragma once

class IndexBuffer
{
public:
    IndexBuffer(const unsigned int elements, const void* data);
    void Bind() const;

private:
    unsigned int m_ID;
};