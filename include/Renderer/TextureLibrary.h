#pragma once

#include "Texture.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace PEANUT {

class TextureLibrary {
public:
    TextureLibrary() = default;
    std::shared_ptr<Texture> Load(const std::string& textureName);

private:
    std::unordered_map<std::string, std::shared_ptr<Texture>> m_savedTextures;
};

} // namespace PEANUT
