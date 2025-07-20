#include "TextureLibrary.hpp"

// peanut
#include "peanut/Texture.hpp"
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

Texture* TextureLibrary::GetImage(const fs::path& file) { return GetInstance().GetImageImpl(file); }

Texture* TextureLibrary::GetCubemap(const fs::path& dir) { return GetInstance().GetCubemapImpl(dir); }

Texture* TextureLibrary::GetImageImpl(const fs::path& file)
{
    // Error if file doesn't exist
    if (!std::filesystem::exists(file)) {
        LOG_ERROR("Texture file does not exist [{}]", file.c_str());
        return nullptr;
    }

    // Return cached texture if already loaded
    if (const auto it = textures_.find(file); it != textures_.end()) {
        return &it->second;
    }

    // Load texture
    if (const auto p = textures_.emplace(file, Texture::MakeTextureImage(file)); p.second) {
        LOG_DEBUG("Loaded texture [{}]", file.c_str());
        return &p.first->second;
    }

    return nullptr;
}

Texture* TextureLibrary::GetCubemapImpl(const fs::path& dir)
{
    // Error if file doesn't exist
    if (!std::filesystem::exists(dir)) {
        LOG_ERROR("Texture dir does not exist [{}]", dir.c_str());
        return nullptr;
    }

    // Return cached texture if already loaded
    if (const auto it = textures_.find(dir); it != textures_.end()) {
        return &it->second;
    }

    // Load texture
    if (const auto p = textures_.emplace(dir, Texture::MakeTextureCubeMap(dir)); p.second) {
        LOG_DEBUG("Loaded cubemap texture [{}]", dir.c_str());
        return &p.first->second;
    }

    return nullptr;
}

} // namespace Rhino
