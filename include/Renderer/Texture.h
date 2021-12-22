#pragma once

#include <filesystem>

namespace PEANUT {

class Texture {
public:
    Texture(const std::filesystem::path& textureFile);
    void Bind() const;

private:
    unsigned int m_ID;
};

}