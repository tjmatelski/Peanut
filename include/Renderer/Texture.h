#pragma once

namespace PEANUT
{

class Texture
{
public:
    Texture(const char* textureFile);
    void Bind() const;

private:
    unsigned int m_ID;
};

}