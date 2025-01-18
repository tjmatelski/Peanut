#pragma once

#include <peanut/IndexBuffer.hpp>
#include <peanut/Math.hpp>
#include <peanut/Vertex.hpp>
#include <peanut/VertexArray.hpp>

// stl
#include <vector>

namespace PEANUT {

class Mesh {
public:
    Mesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indicies);
    ~Mesh() = default;
    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = default;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = default;

    [[nodiscard]] inline const VertexArray& GetVertexArray() const { return m_vertexArray; }
    [[nodiscard]] inline const IndexBuffer& GetIndexBuffer() const { return m_indexBuffer; }

private:
    VertexArray m_vertexArray;
    IndexBuffer m_indexBuffer;
};

}
