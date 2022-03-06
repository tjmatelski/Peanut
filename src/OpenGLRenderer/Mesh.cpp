#include <Renderer/Mesh.h>

namespace PEANUT {

static_assert(sizeof(Mesh::Vertex) == 32);

Mesh::Mesh(std::vector<Vertex>&& verticies, std::vector<unsigned int>&& indicies)
    : m_vertexArray()
    , m_indexBuffer(indicies.size(), indicies.data())
{
    m_vertices = std::move(verticies);
    m_indicies = std::move(indicies);

    VertexBuffer vb(m_vertices.size() * sizeof(Mesh::Vertex), m_vertices.data());
    BufferLayout bl;
    bl.Push<float>(3);
    bl.Push<float>(3);
    bl.Push<float>(2);
    m_vertexArray.AddBuffer(vb, bl);
}

}
