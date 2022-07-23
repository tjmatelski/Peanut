#pragma once

namespace PEANUT {

class FrameBuffer {
public:
    FrameBuffer();
    ~FrameBuffer();

    void Bind();
    void Unbind();

    [[nodiscard]] unsigned int GetColorbufferTextureID() const { return m_colorBufferID; }

private:
    unsigned int m_id;
    unsigned int m_colorBufferID;
    unsigned int m_renderBufferID;
};

}