#include "TextureLibrary.hpp"

#include "../Settings.hpp"
#include <peanut/Log.hpp>

// stl
#include <filesystem>

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
    fs::path fullPath = textureName;
    if (!fs::exists(fullPath)) {
        fullPath = Settings::GetResourceDir() / fullPath;
    }
    if (!fs::exists(fullPath)) {
        LOG_ERROR("Texture File '{}' could not be found", textureName.c_str());
    }

    fullPath = fs::canonical(fullPath);
    if (m_savedTextures.count(fullPath) != 0) {
        return { m_savedTextures.at(fullPath), type };
    }

    LOG_DEBUG("Adding texture to cache '{}'", fullPath.c_str());
    m_savedTextures.emplace(std::make_pair(fullPath, Texture(fullPath, type)));
    return { m_savedTextures.at(fullPath), type };
}

} // namespace Rhino
