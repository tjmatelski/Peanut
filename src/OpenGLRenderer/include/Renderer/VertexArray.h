#pragma once

#include "BufferLayout.h"
#include "VertexBuffer.h"
#include <vector>

namespace PEANUT {

class VertexArray {
public:
    VertexArray();
    ~VertexArray();
    VertexArray(VertexArray&&);
    VertexArray& operator=(VertexArray&&);
    VertexArray(const VertexArray&) = delete;
    VertexArray& operator=(const VertexArray&) = delete;

    void Bind() const;
    void AddBuffer(VertexBuffer&& vb, const BufferLayout& bl);

private:
    unsigned int m_ID;
    std::vector<VertexBuffer> m_vertexBuffers;
};

}