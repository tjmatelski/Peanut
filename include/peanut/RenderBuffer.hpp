#pragma once

namespace PEANUT {
class RenderBuffer {
public:
    struct Config {
        unsigned int width_ { 0 };
        unsigned int height_ { 0 };
    };

    RenderBuffer(Config config);
    ~RenderBuffer();
    RenderBuffer(const RenderBuffer&) = delete;
    RenderBuffer(RenderBuffer&& other);
    RenderBuffer& operator=(const RenderBuffer&) = delete;
    RenderBuffer& operator=(RenderBuffer&& other);

    void Bind();
    auto GetConfig() const { return config_; }
    auto GetID() const { return id_; }

private:
    Config config_ {};
    unsigned int id_ { 0 };
};
}
