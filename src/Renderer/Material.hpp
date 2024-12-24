#pragma once

#include "Texture.hpp"

// stl
#include <vector>

namespace PEANUT {

class Material {
public:
    Material(std::vector<Texture>&& textures)
        : m_textures(textures)
    {
    }
    inline const std::vector<Texture>& GetTextures() const { return m_textures; }
    inline float GetShininess() const { return m_shininess; }

private:
    std::vector<Texture> m_textures;
    float m_shininess = 32.0f;
};

}
