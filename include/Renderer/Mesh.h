#pragma once

#include "../Math.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "VertexArray.h"

#include <vector>

namespace PEANUT {

class Mesh {
public:
    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 texCoords;
    };

    Mesh(std::vector<Vertex>&& verticies, std::vector<unsigned int>&& indicies);

    inline const VertexArray& GetVertexArray() const { return m_vertexArray; }
    inline const IndexBuffer& GetIndexBuffer() const { return m_indexBuffer; }

private:
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indicies;
    VertexArray m_vertexArray;
    IndexBuffer m_indexBuffer;
};

}
