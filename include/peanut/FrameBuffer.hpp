#pragma once

// stl
#include <variant>
#include <vector>

namespace PEANUT {

class RenderBuffer;
class Texture;

class FrameBuffer {
public:
    struct Config {
        static Config GetDefault() { return {}; }
        bool bind_draw_buf_ { true };
        bool bind_read_buf_ { true };
    };

    using Buffers = std::vector<std::variant<RenderBuffer*, Texture*>>;

    FrameBuffer(const Buffers& bufs, Config config = Config::GetDefault());
    FrameBuffer(const FrameBuffer&) = delete;
    FrameBuffer(FrameBuffer&& other);
    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&& other);
    ~FrameBuffer();

    void Bind();
    void Unbind();

private:
    unsigned int m_id;
};

}