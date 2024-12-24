#pragma once

#include "IndexBuffer.hpp"
#include "Texture.hpp"
#include "VertexArray.hpp"
#include <peanut/Component.hpp>
#include <peanut/Math.hpp>

// stl
#include <vector>

namespace PEANUT {

class OpenglMesh {
public:
    OpenglMesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indicies);
    ~OpenglMesh() = default;
    OpenglMesh(const OpenglMesh&) = delete;
    OpenglMesh(OpenglMesh&&) = default;
    OpenglMesh& operator=(const OpenglMesh&) = delete;
    OpenglMesh& operator=(OpenglMesh&&) = default;

    [[nodiscard]] inline const VertexArray& GetVertexArray() const { return m_vertexArray; }
    [[nodiscard]] inline const IndexBuffer& GetIndexBuffer() const { return m_indexBuffer; }

private:
    VertexArray m_vertexArray;
    IndexBuffer m_indexBuffer;
};

}