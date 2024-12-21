#include <Scene/SceneSerializer.h>

#include "Utils/Settings.h"
#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Scene/NativeScript.h>
#include <Scene/Scene.h>

#include <glm/fwd.hpp>
#include <string>
#include <type_traits>
#include <vector>
#include <yaml-cpp/emitter.h>
#include <yaml-cpp/emittermanip.h>
#include <yaml-cpp/node/node.h>
#include <yaml-cpp/yaml.h>

#include <filesystem>
#include <fstream>

namespace YAML {
template <>
struct convert<glm::vec3> {
    static Node encode(const glm::vec3& vec)
    {
        Node node;
        node.push_back(vec.x);
        node.push_back(vec.y);
        node.push_back(vec.z);
        node.SetStyle(EmitterStyle::Flow);
        return node;
    }

    static bool decode(const Node& node, glm::vec3& rhs)
    {
        if (!node.IsSequence() || node.size() != 3) {
            return false;
        }

        rhs.x = node[0].as<float>();
        rhs.y = node[1].as<float>();
        rhs.z = node[2].as<float>();
        return true;
    }
};
} // namespace YAML

namespace {
static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
    return out;
}

template <class T>
void Serialize(YAML::Emitter& out, const T& comp, std::filesystem::path sceneFile)
{
    assert(false && "Missing specialization for component");
}

void Serialize(YAML::Emitter& out, const PEANUT::TagComponent& tag, std::filesystem::path sceneFile)
{
    out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Tag" << YAML::Value << tag.tag;
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::TransformComponent& transform, std::filesystem::path sceneFile)
{
    out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Translation" << YAML::Value << transform.translation;
    out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
    out << YAML::Key << "Scale" << YAML::Value << transform.scale;
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::SpriteRenderComponent& spriteRenderComp, std::filesystem::path sceneFile)
{
    out << YAML::Key << "SpriteRenderComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Color" << YAML::Value << spriteRenderComp.color;
    std::filesystem::path texPath(spriteRenderComp.texture);
    out << YAML::Key << "Texture" << YAML::Value << std::filesystem::relative(texPath, PEANUT::Settings::GetResourceDir());
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::PythonScriptComponent& scriptComp, std::filesystem::path sceneFile)
{
    auto scriptPath = scriptComp.script;
    out << YAML::Key << "PythonScriptComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Script" << YAML::Value << std::filesystem::relative(scriptPath, sceneFile.parent_path());
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::ModelFileComponent& comp, std::filesystem::path sceneFile)
{
    auto scriptPath = comp.file;
    out << YAML::Key << "ModelFileComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "File" << YAML::Value << std::filesystem::relative(scriptPath, PEANUT::Settings::GetResourceDir());
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::DirectionalLightComponent& light, std::filesystem::path sceneFile)
{
    out << YAML::Key << "DirectionalLightComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Direction" << YAML::Value << light.direction;
    out << YAML::Key << "Ambient" << YAML::Value << light.ambient;
    out << YAML::Key << "Diffuse" << YAML::Value << light.diffuse;
    out << YAML::Key << "Specular" << YAML::Value << light.specular;
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::PointLightComponent& light, std::filesystem::path sceneFile)
{
    out << YAML::Key << "PointLightComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Color" << YAML::Value << light.color;
    out << YAML::Key << "Ambient" << YAML::Value << light.ambient;
    out << YAML::Key << "Diffuse" << YAML::Value << light.diffuse;
    out << YAML::Key << "Specular" << YAML::Value << light.specular;
    out << YAML::Key << "Constant" << YAML::Value << light.constant;
    out << YAML::Key << "Linear" << YAML::Value << light.linear;
    out << YAML::Key << "Quadratic" << YAML::Value << light.quadratic;
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, const PEANUT::SkyboxComponent& comp, std::filesystem::path sceneFile)
{
    out << YAML::Key << "SkyboxComponent" << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Directory" << YAML::Value << std::filesystem::relative(comp.directory, PEANUT::Settings::GetResourceDir());
    out << YAML::EndMap;
}

void Serialize(YAML::Emitter& out, std::pair<PEANUT::NativeScript*, std::string> p, std::filesystem::path sceneFile)
{
    const auto& comp = p.first;
    const auto& name = p.second;
    out << YAML::Key << name << YAML::Value << YAML::BeginMap;
    out << YAML::Key << "Members" << YAML::Value << YAML::BeginSeq;
    for (const auto& member : comp->GetMembers()) {
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << member.name;
        out << YAML::Key << "Type";
        switch (member.type) {
        case PEANUT::MemberVariable::Type::Bool:
            out << YAML::Value << "Bool";
            break;
        case PEANUT::MemberVariable::Type::Int:
            out << YAML::Value << "Int";
            break;
        case PEANUT::MemberVariable::Type::Float:
            out << YAML::Value << "Float";
            break;
        case PEANUT::MemberVariable::Type::Double:
            out << YAML::Value << "Double";
            break;
        case PEANUT::MemberVariable::Type::String:
            out << YAML::Value << "String";
            break;
        }
        out << YAML::Key << "Value";
        switch (member.type) {
        case PEANUT::MemberVariable::Type::Bool:
            out << YAML::Value << *static_cast<bool*>(member.addr);
            break;
        case PEANUT::MemberVariable::Type::Int:
            out << YAML::Value << *static_cast<int*>(member.addr);
            break;
        case PEANUT::MemberVariable::Type::Float:
            out << YAML::Value << *static_cast<float*>(member.addr);
            break;
        case PEANUT::MemberVariable::Type::Double:
            out << YAML::Value << *static_cast<double*>(member.addr);
            break;
        case PEANUT::MemberVariable::Type::String:
            out << YAML::Value << *static_cast<std::string*>(member.addr);
            break;
        }
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;
}

template <class T>
void Serialize(YAML::Emitter& out, PEANUT::Entity ent, std::filesystem::path sceneFile, const std::vector<std::string>& plugins)
{
    if constexpr (std::is_same_v<T, PEANUT::NativeScript>) {
        for (const auto& name : plugins) {
            if (ent.Has<PEANUT::NativeScript>(name)) {
                Serialize(out, std::pair { ent.Get<PEANUT::NativeScript>(name), name }, sceneFile);
            }
        }
    } else {
        if (ent.Has<T>()) {
            Serialize(out, ent.Get<T>(), sceneFile);
        }
    }
}

template <class T>
void Deserialize(T& comp, YAML::Node node, std::filesystem::path projectDir)
{
    assert(false && "Missing deserialization specialization");
}

void Deserialize(PEANUT::TagComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    comp.tag = node["Tag"].as<std::string>();
}

void Deserialize(PEANUT::TransformComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    comp.translation = node["Translation"].as<glm::vec3>();
    comp.rotation = node["Rotation"].as<glm::vec3>();
    comp.scale = node["Scale"].as<glm::vec3>();
}

void Deserialize(PEANUT::SpriteRenderComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    comp.color = node["Color"].as<glm::vec3>();
    std::filesystem::path relative(node["Texture"].as<std::string>());
    comp.texture = (projectDir / relative).string();
}

void Deserialize(PEANUT::PythonScriptComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    std::filesystem::path relative(node["Script"].as<std::string>());
    comp.script = projectDir / relative;
}

void Deserialize(PEANUT::ModelFileComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    std::filesystem::path relative(node["File"].as<std::string>());
    comp.file = PEANUT::Settings::GetResourceDir() / relative;
}

void Deserialize(PEANUT::DirectionalLightComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    comp.direction = node["Direction"].as<glm::vec3>();
    comp.ambient = node["Ambient"].as<float>();
    comp.diffuse = node["Diffuse"].as<float>();
    comp.specular = node["Specular"].as<float>();
}

void Deserialize(PEANUT::PointLightComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    comp.color = node["Color"].as<glm::vec3>();
    comp.ambient = node["Ambient"].as<float>();
    comp.diffuse = node["Diffuse"].as<float>();
    comp.specular = node["Specular"].as<float>();
    comp.constant = node["Constant"].as<float>();
    comp.linear = node["Linear"].as<float>();
    comp.quadratic = node["Quadratic"].as<float>();
}

void Deserialize(PEANUT::SkyboxComponent& comp, YAML::Node node, std::filesystem::path projectDir)
{
    std::filesystem::path rel = node["Directory"].as<std::string>();
    comp.directory = PEANUT::Settings::GetResourceDir() / rel;
}

template <class T>
void Deserialize(const char* name, PEANUT::Entity ent, YAML::Node node, std::filesystem::path projectDir, const std::vector<std::string>& plugins)
{
    if constexpr (std::is_same_v<T, PEANUT::NativeScript>) {
    } else if (node[name]) {
        if constexpr (std::is_same_v<T, PEANUT::TagComponent>) {
            Deserialize(ent.Get<PEANUT::TagComponent>(), node[name], projectDir);
        } else if constexpr (std::is_same_v<T, PEANUT::TransformComponent>) {
            Deserialize(ent.Get<PEANUT::TransformComponent>(), node[name], projectDir);
        } else {
            Deserialize(ent.Add<T>(), node[name], projectDir);
        }
    }
}
}

namespace PEANUT {
void SceneSerializer::Serialize(Scene& scene, const std::string& filepath, const std::vector<std::string>& plugins)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "MyTempSceneName";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    scene.ForEachEntity([&](Entity ent) {
        out << YAML::BeginMap;
        out << YAML::Key << "Entity" << YAML::Value << 123456789;

        ::Serialize<TagComponent>(out, ent, filepath, plugins);
        ::Serialize<TransformComponent>(out, ent, filepath, plugins);
        ::Serialize<SpriteRenderComponent>(out, ent, filepath, plugins);
        ::Serialize<PythonScriptComponent>(out, ent, filepath, plugins);
        ::Serialize<ModelFileComponent>(out, ent, filepath, plugins);
        ::Serialize<DirectionalLightComponent>(out, ent, filepath, plugins);
        ::Serialize<PointLightComponent>(out, ent, filepath, plugins);
        ::Serialize<SkyboxComponent>(out, ent, filepath, plugins);
        ::Serialize<NativeScript>(out, ent, filepath, plugins);

        out << YAML::EndMap;
    });
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(filepath);
    ofs << out.c_str();
}

void SceneSerializer::Deserialize(const std::string& file, Scene& scene, const std::vector<std::string>& plugins)
{
    const auto projectDir = std::filesystem::path(file).parent_path();
    scene.Clear();
    YAML::Node node = YAML::LoadFile(file);
    for (YAML::const_iterator it = node["Entities"].begin(); it != node["Entities"].end(); ++it) {
        const YAML::Node node = *it;
        Entity sceneEnt = scene.CreateEntity();

        ::Deserialize<TagComponent>("TagComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<TransformComponent>("TransformComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<SpriteRenderComponent>("SpriteRenderComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<PythonScriptComponent>("PythonScriptComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<ModelFileComponent>("ModelFileComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<DirectionalLightComponent>("DirectionalLightComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<PointLightComponent>("PointLightComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<SkyboxComponent>("SkyboxComponent", sceneEnt, node, projectDir, plugins);
        ::Deserialize<NativeScript>("NativeScript", sceneEnt, node, projectDir, plugins);
    }
}

} // namespace PEANUT