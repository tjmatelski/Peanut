#pragma once

namespace PEANUT {

struct FrameBufferConfig {
    enum class Type { COLOR, DEPTH };
    int width = 100;
    int height = 100;
    Type type = FrameBufferConfig::Type::COLOR;
};

class FrameBuffer {
public:
    FrameBuffer(const FrameBufferConfig& config);
    ~FrameBuffer();

    void Bind();
    void Unbind();
    void Resize(int width, int height);

    [[nodiscard]] unsigned int TextureID() const { return m_textureID; }

private:
    FrameBufferConfig m_config;
    unsigned int m_id;
    unsigned int m_textureID;
    unsigned int m_renderBufferID;

    void Create();
    void Destroy();
};

}