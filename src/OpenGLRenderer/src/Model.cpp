#include <Renderer/Model.h>

#include <Renderer/TextureLibrary.h>
#include <Utils/Log.h>

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

namespace {
PEANUT::Texture::Type AssimpToPEANUTTexType(const aiTextureType type)
{
    switch (type) {
    case aiTextureType_DIFFUSE:
        return PEANUT::Texture::Type::Diffuse;
        break;

    case aiTextureType_SPECULAR:
        return PEANUT::Texture::Type::Diffuse;
        break;

    default:
        LOG_WARN("Non-Implemented Texture type: {}", static_cast<int>(type));
        return PEANUT::Texture::Type::None;
        break;
    }
}
}

namespace PEANUT {

Model::Model(const Renderable& renderable)
    : m_modelFile()
    , m_renderables()
{
    m_renderables.push_back(renderable);
}

Model::Model(const std::string& modelFile)
    : m_modelFile(modelFile)
{
    if (std::filesystem::exists(m_modelFile) == false) {
        LOG_ERROR("Could not find model file: {}", m_modelFile.string());
    }

    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(m_modelFile, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR("Failed to load model: {}", m_modelFile.string());
        LOG_ERROR("ASSIMP: {}", importer.GetErrorString());
    }

    ProcessNode(scene->mRootNode, scene);
}

void Model::ProcessNode(const aiNode* node, const aiScene* scene)
{
    LOG_DEBUG("Processing node: {}", node->mName.C_Str());
    for (unsigned int i = 0; i < node->mNumMeshes; ++i) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        m_renderables.push_back(LoadRenderable(mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; ++i) {
        ProcessNode(node->mChildren[i], scene);
    }
}

Renderable Model::LoadRenderable(const aiMesh* mesh, const aiScene* scene)
{
    LOG_DEBUG("Loading Mesh: {}", mesh->mName.C_Str());
    std::vector<Vertex> verts;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i) {
        Vertex vert;
        vert.position = glm::vec3(mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z);
        vert.normal = glm::vec3(mesh->mNormals[i].x,
            mesh->mNormals[i].y,
            mesh->mNormals[i].z);
        vert.texCoords = mesh->mTextureCoords[0] ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0.0f, 0.0f);

        verts.push_back(vert);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j) {
            indices.push_back(face.mIndices[j]);
        }
    }

    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
    std::vector<Texture> diffuse = LoadTextures(material, aiTextureType_DIFFUSE);
    std::vector<Texture> specular = LoadTextures(material, aiTextureType_SPECULAR);

    textures.insert(textures.end(), diffuse.begin(), diffuse.end());
    textures.insert(textures.end(), specular.begin(), specular.end());

    return { OpenglMesh(std::move(verts), std::move(indices)), Material(std::move(textures)) };
}

std::vector<Texture> Model::LoadTextures(const aiMaterial* mat, const int type)
{
    LOG_DEBUG("Loading Material {} Texture Type {}", mat->GetName().C_Str(), type);
    std::vector<Texture> textures;
    aiTextureType assimpType = static_cast<aiTextureType>(type);
    for (unsigned int i = 0; i < mat->GetTextureCount(assimpType); ++i) {
        aiString name;
        mat->GetTexture(assimpType, i, &name);
        Texture::Type myType = AssimpToPEANUTTexType(assimpType);
        LOG_DEBUG("Loading Texture: {}", name.C_Str());
        Texture tex = TextureLibrary::Load(m_modelFile.parent_path() / name.C_Str(), myType);
        textures.push_back(tex);
    }
    return textures;
}

}