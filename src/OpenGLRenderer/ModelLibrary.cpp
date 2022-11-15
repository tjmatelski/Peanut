#include "Log.h"
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
    if (!std::filesystem::exists(file)) {
        LOG_ERROR("Model file '{}' doesn't exist", file.c_str());
    }
    std::string fullpath = std::filesystem::canonical(file);
    if (auto search = m_cache.find(fullpath); search != m_cache.end()) {
        return search->second;
    }
    LOG_DEBUG("Loading model '{}' into cache", fullpath);
    m_cache.emplace(std::make_pair(fullpath, Model(fullpath)));
    return m_cache.at(fullpath);
}
}