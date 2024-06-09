#pragma once

namespace PEANUT {

class IndexBuffer {
public:
    IndexBuffer(const unsigned int elements, const void* data);
    ~IndexBuffer();
    IndexBuffer(IndexBuffer&&);
    IndexBuffer& operator=(IndexBuffer&&);
    IndexBuffer(const IndexBuffer&) = delete;
    IndexBuffer& operator=(const IndexBuffer&) = delete;

    void Bind() const;
    [[nodiscard]] inline unsigned int GetElementCount() const { return m_elementCount; }

private:
    unsigned int m_ID;
    unsigned int m_elementCount;
};

}