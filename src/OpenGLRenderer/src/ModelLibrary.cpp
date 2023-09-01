#include "Utils/Log.h"
#include "Utils/Settings.h"
#include <Renderer/ModelLibrary.h>
#include <filesystem>
#include <utility>

namespace PEANUT {
ModelLibrary& ModelLibrary::Instance()
{
    static ModelLibrary library;
    return library;
}

const Model& ModelLibrary::Get(const std::filesystem::path& model)
{
    return ModelLibrary::Instance().GetImpl(model);
}

const Model& ModelLibrary::GetImpl(const std::filesystem::path& file)
{
    namespace fs = std::filesystem;
    fs::path fullPath = file;
    if (!fs::exists(fullPath)) {
        fullPath = Settings::GetResourceDir() / fullPath;
    }
    if (!fs::exists(fullPath)) {
        LOG_ERROR("Model File '{}' could not be found", file.c_str());
    }

    fullPath = std::filesystem::canonical(file);
    if (auto search = m_cache.find(fullPath); search != m_cache.end()) {
        return search->second;
    }
    LOG_DEBUG("Adding model to cache'{}'", fullPath.c_str());
    m_cache.emplace(std::make_pair(fullPath, Model(fullPath)));
    return m_cache.at(fullPath);
}
}