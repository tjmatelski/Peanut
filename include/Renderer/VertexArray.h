#pragma once

#include "VertexBuffer.h"
#include "BufferLayout.h"

class VertexArray
{
public:
    VertexArray();
    void Bind() const;
    void AddBuffer(const VertexBuffer& vb, const BufferLayout& bl);

private:
    unsigned int m_ID;
};