#include "Shader.hpp"
#include <memory>
#include <peanut/Log.hpp>
#include <peanut/ShaderLibrary.hpp>

namespace PEANUT {

bool ShaderLibrary::Load(const std::filesystem::path& shader)
{
    try {
        GetInstance().LoadImpl(shader);
        return true;
    } catch (...) {
        return false;
    }
}

bool ShaderLibrary::Reload(const std::filesystem::path& shader)
{
    const auto it = GetInstance().m_savedShaders.find(shader);
    if (it != GetInstance().m_savedShaders.end()) {
        GetInstance().m_savedShaders.erase(it);
    }
    return Load(shader);
}

const Shader& ShaderLibrary::Get(const std::filesystem::path& shaderName)
{
    return GetInstance().LoadImpl(shaderName);
}

ShaderLibrary& ShaderLibrary::GetInstance()
{
    static ShaderLibrary sl;
    return sl;
}

const Shader& ShaderLibrary::LoadImpl(const std::filesystem::path& shaderName)
{
    const auto it = m_savedShaders.find(shaderName);
    if (it != m_savedShaders.end()) {
        return *(it->second);
    }
    const auto pair = m_savedShaders.emplace(shaderName, std::make_unique<Shader>(shaderName));
    return *(pair.first->second);
}

} // namespace PEANUT
