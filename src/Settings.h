#pragma once

#include <filesystem>

namespace PEANUT {
class Settings {
public:
    static void Initialize();
    static std::filesystem::path GetResourceDir();
    static std::filesystem::path GetBuildDir();
};
}