#include "Utils/Settings.h"
#include <Utils/Log.h>
#include <yaml-cpp/yaml.h>

#include <fstream>

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