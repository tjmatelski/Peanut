#pragma once

// stl
#include <array>
#include <cstddef>
#include <filesystem>
#include <optional>

namespace PEANUT {

class Texture {
public:
    enum class Type { COLOR, DEPTH };

    enum class Wrapping { REPEAT, MIRRORED_REPEAT, CLAMP_TO_EDGE, CLAMP_TO_BORDER };

    enum class Filter { NEAREST, LINEAR };

    struct Config {
        Type type_ { Texture::Type::COLOR };
        size_t width_ { 256 };
        size_t height_ { 256 };
        Wrapping wrapping_ { Wrapping::REPEAT };
        Filter filter_ { Filter::NEAREST };
        std::optional<std::array<float, 4>> border_color_ {};
    };

    Texture(Config config, unsigned char* p_data = nullptr);
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other);
    Texture& operator=(Texture&& other);
    ~Texture();

    static Texture MakeTextureImage(std::filesystem::path file);
    static Texture MakeTextureCubeMap(std::filesystem::path dir);

    void Bind() const;
    auto GetConfig() const { return config_; }
    auto GetID() const { return id_; }

private:
    Texture();

    unsigned int id_;
    Config config_;
    bool is_cube_map_;
};

}
