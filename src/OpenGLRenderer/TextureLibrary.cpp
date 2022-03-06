#include <Renderer/TextureLibrary.h>

#include <Log.h>

namespace fs = std::filesystem;

namespace PEANUT {

TextureLibrary& TextureLibrary::GetInstance()
{
    static TextureLibrary s_library;
    return s_library;
}

Texture TextureLibrary::Load(const fs::path& textureName, const Texture::Type type)
{
    return GetInstance().LoadImpl(textureName, type);
}

Texture TextureLibrary::LoadImpl(const fs::path& textureName, const Texture::Type type)
{
    if (fs::exists(textureName) == false) {
        LOG_ERROR("Texture File '{}' does not exist", textureName.string());
    }

    std::string fullPath = fs::canonical(textureName).string();
    if (m_savedTextures.count(fullPath) != 0) {
        return Texture(m_savedTextures.at(fullPath), type);
    }

    m_savedTextures.emplace(std::make_pair(fullPath, Texture(fullPath)));
    return Texture(m_savedTextures.at(fullPath), type);
}

} // namespace Rhino
