#pragma once

namespace PEANUT {

class VertexBuffer {
public:
    VertexBuffer(const unsigned int size, const void* data);
    ~VertexBuffer();
    VertexBuffer(VertexBuffer&&);
    VertexBuffer& operator=(VertexBuffer&&);
    VertexBuffer(const VertexBuffer&) = delete;
    VertexBuffer& operator=(const VertexBuffer&) = delete;

    void Bind() const;

private:
    unsigned int m_ID = 0;
};

}