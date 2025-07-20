#pragma once

#include <peanut/Texture.hpp>

// stl
#include <filesystem>
#include <unordered_map>

namespace PEANUT {

class TextureLibrary {
public:
    static Texture* GetImage(const std::filesystem::path& file);
    static Texture* GetCubemap(const std::filesystem::path& dir);

private:
    TextureLibrary() = default;
    std::unordered_map<std::filesystem::path, Texture> textures_;

    static TextureLibrary& GetInstance();
    Texture* GetImageImpl(const std::filesystem::path& file);
    Texture* GetCubemapImpl(const std::filesystem::path& dir);
};

} // namespace Rhino
