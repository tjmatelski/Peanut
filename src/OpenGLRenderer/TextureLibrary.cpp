#include <Renderer/TextureLibrary.h>

namespace PEANUT {

std::shared_ptr<Texture> TextureLibrary::Load(const std::string& textureName)
{
    if (m_savedTextures.count(textureName) != 0) {
        return m_savedTextures.at(textureName);
    }
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(textureName.c_str());
    m_savedTextures[textureName] = texture;
    return texture;
}

} // namespace PEANUT
