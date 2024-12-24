#pragma once

// stl
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>

namespace PEANUT {

class Shader;

class ShaderLibrary {
public:
    static bool Load(const std::filesystem::path& shader);
    static bool Reload(const std::filesystem::path& shader);
    static const Shader& Get(const std::filesystem::path& shaderName);

private:
    ShaderLibrary() = default;
    std::unordered_map<std::string, std::unique_ptr<Shader>> m_savedShaders;

    static ShaderLibrary& GetInstance();
    const Shader& LoadImpl(const std::filesystem::path& shaderName);
};

} // namespace PEANUT
