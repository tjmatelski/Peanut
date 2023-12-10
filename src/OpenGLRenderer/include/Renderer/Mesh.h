#pragma once

#include "IndexBuffer.h"
#include "Scene/Component.h"
#include "Texture.h"
#include "Utils/Math.h"
#include "VertexArray.h"

#include <vector>

namespace PEANUT {

class OpenglMesh {
public:
    OpenglMesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indicies);

    inline const VertexArray& GetVertexArray() const { return m_vertexArray; }
    inline const IndexBuffer& GetIndexBuffer() const { return m_indexBuffer; }

private:
    VertexArray m_vertexArray;
    IndexBuffer m_indexBuffer;
};

}
