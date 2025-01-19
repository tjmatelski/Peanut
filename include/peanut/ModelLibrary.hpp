#pragma once

#include <peanut/Model.hpp>

// stl
#include <filesystem>
#include <unordered_map>

namespace PEANUT {
class ModelLibrary {
public:
    static auto Get(const std::filesystem::path& model) -> Model&;

private:
    ModelLibrary() = default;
    static ModelLibrary& Instance();
    Model& GetImpl(const std::filesystem::path& file);

    std::unordered_map<std::string, Model> m_cache;
};
}