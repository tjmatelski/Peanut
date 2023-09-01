#pragma once

namespace PEANUT {

struct FrameBufferConfig {
    int width;
    int height;
};

class FrameBuffer {
public:
    FrameBuffer(const FrameBufferConfig& config);
    ~FrameBuffer();

    void Bind();
    void Unbind();
    void Resize(int width, int height);

    [[nodiscard]] unsigned int GetColorbufferTextureID() const { return m_colorBufferID; }

private:
    FrameBufferConfig m_config;
    unsigned int m_id;
    unsigned int m_colorBufferID;
    unsigned int m_renderBufferID;

    void Create();
    void Destroy();
};

}