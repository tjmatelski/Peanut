#pragma once

// stl
#include <string>

namespace PEANUT {

class Texture {
public:
    enum class Type {
        None,
        Diffuse,
        Specular,
        CubeMap
    };

    /**
     * Constructs a blank (all white) texture */
    Texture(const Type type = Type::None);
    Texture(const std::string& textureFile, const Type type = Type::None);
    Texture(const Texture& texture) = default;
    Texture(const Texture& other, const Type type);
    Texture& operator=(const Texture& other) = default;

    void Bind() const;
    [[nodiscard]] inline Type GetType() const { return m_type; }
    [[nodiscard]] unsigned int GetID() const { return m_ID; }

private:
    unsigned int m_ID;
    Type m_type;
};

}
