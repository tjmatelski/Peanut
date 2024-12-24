#pragma once

#include "Model.hpp"

// stl
#include <filesystem>
#include <unordered_map>

namespace PEANUT {
class ModelLibrary {
public:
    static const Model& Get(const std::filesystem::path& model);

private:
    ModelLibrary() = default;
    static ModelLibrary& Instance();
    const Model& GetImpl(const std::filesystem::path& file);

    std::unordered_map<std::string, Model> m_cache;
};
}