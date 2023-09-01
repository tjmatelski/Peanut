#pragma once

#include <filesystem>

namespace PEANUT {
class Settings {
public:
    static std::filesystem::path GetResourceDir() { return GetApplicationDir() / "res"; }
    static void SetApplication(std::filesystem::path path);
    static std::filesystem::path GetApplicationDir() { return GetInstance().m_applicationDir; }

private:
    static Settings& GetInstance()
    {
        static Settings instance;
        return instance;
    }
    std::filesystem::path m_applicationDir;
};
}