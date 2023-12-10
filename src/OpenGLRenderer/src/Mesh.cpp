#include <Renderer/Mesh.h>

namespace PEANUT {

static_assert(sizeof(Vertex) == 32);

OpenglMesh::OpenglMesh(const std::vector<Vertex>& verticies, const std::vector<unsigned int>& indicies)
    : m_vertexArray()
    , m_indexBuffer(indicies.size(), indicies.data())
{
    VertexBuffer vb(verticies.size() * sizeof(Vertex), verticies.data());
    BufferLayout bl;
    bl.Push<float>(3);
    bl.Push<float>(3);
    bl.Push<float>(2);
    m_vertexArray.AddBuffer(vb, bl);
}

}
