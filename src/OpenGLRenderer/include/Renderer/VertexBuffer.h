#pragma once

namespace PEANUT {

class VertexBuffer {
public:
    VertexBuffer(const unsigned int size, const void* data);
    void Bind() const;

private:
    unsigned int m_ID;
};

}