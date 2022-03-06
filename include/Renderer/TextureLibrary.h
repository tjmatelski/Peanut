#pragma once

#include "Texture.h"

#include <filesystem>
#include <string>
#include <unordered_map>

namespace PEANUT {

class TextureLibrary {
public:
    static Texture Load(const std::filesystem::path& textureName, const Texture::Type type = Texture::Type::None);

private:
    TextureLibrary() = default;
    std::unordered_map<std::string, Texture> m_savedTextures;

    static TextureLibrary& GetInstance();
    Texture LoadImpl(const std::filesystem::path& textureName, const Texture::Type type);
};

} // namespace Rhino
