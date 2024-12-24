#include "Settings.hpp"
#include <peanut/Log.hpp>

// external
#include <yaml-cpp/yaml.h>

namespace PEANUT {

void Settings::SetApplication(std::filesystem::path path)
{
    path = path.parent_path();
    if (!std::filesystem::exists(path)) {
        LOG_ERROR("Application path not valid: {}", path.c_str());
    }
    GetInstance().m_applicationDir = path;
}

}