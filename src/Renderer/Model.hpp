#pragma once

#include "Material.hpp"
#include "Mesh.hpp"

// stl
#include <filesystem>
#include <string>
#include <vector>

class aiNode;
class aiScene;
class aiMesh;
class aiMaterial;

namespace PEANUT {

struct Renderable {
    OpenglMesh mesh;
    Material material;
};

class Model {
public:
    Model(const std::string& modelFile);
    [[nodiscard]] inline const std::vector<Renderable>& GetRenderables() const { return m_renderables; }

private:
    std::filesystem::path m_modelFile;
    std::vector<Renderable> m_renderables;

    void ProcessNode(const aiNode* node, const aiScene* scene);
    Renderable LoadRenderable(const aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> LoadTextures(const aiMaterial* mat, const int type);
};

}