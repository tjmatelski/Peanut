#include <Scene/SceneSerializer.h>

#include "Utils/Settings.h"
#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Scene/Scene.h>

#include <glm/fwd.hpp>
#include <yaml-cpp/emittermanip.h>
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

static void SerializeEntity(YAML::Emitter& out, PEANUT::Entity ent, std::filesystem::path sceneFile)
{
    using namespace PEANUT;

    out << YAML::BeginMap;
    out << YAML::Key << "Entity" << YAML::Value << 123456789;

    if (ent.Has<TagComponent>()) {
        out << YAML::Key << "TagComponent" << YAML::Value << YAML::BeginMap;
        const TagComponent& tag = ent.Get<TagComponent>();
        out << YAML::Key << "Tag" << YAML::Value << tag.tag;
        out << YAML::EndMap;
    }

    if (ent.Has<TransformComponent>()) {
        out << YAML::Key << "TransformComponent" << YAML::Value << YAML::BeginMap;
        const TransformComponent& transform = ent.Get<TransformComponent>();
        out << YAML::Key << "Translation" << YAML::Value << transform.translation;
        out << YAML::Key << "Rotation" << YAML::Value << transform.rotation;
        out << YAML::Key << "Scale" << YAML::Value << transform.scale;
        out << YAML::EndMap;
    }

    if (ent.Has<SpriteRenderComponent>()) {
        out << YAML::Key << "SpriteRenderComponent" << YAML::Value << YAML::BeginMap;
        const SpriteRenderComponent& spriteRenderComp = ent.Get<SpriteRenderComponent>();
        out << YAML::Key << "Color" << YAML::Value << spriteRenderComp.color;
        std::filesystem::path texPath(spriteRenderComp.texture);
        out << YAML::Key << "Texture" << YAML::Value << std::filesystem::relative(texPath, Settings::GetResourceDir());
        out << YAML::EndMap;
    }

    if (ent.Has<PythonScriptComponent>()) {
        auto scriptPath = ent.Get<PythonScriptComponent>().script;
        out << YAML::Key << "PythonScriptComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Script" << YAML::Value << std::filesystem::relative(scriptPath, sceneFile.parent_path());
        out << YAML::EndMap;
    }

    if (ent.Has<ModelFileComponent>()) {
        auto scriptPath = ent.Get<ModelFileComponent>().file;
        out << YAML::Key << "ModelFileComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "File" << YAML::Value << std::filesystem::relative(scriptPath, Settings::GetResourceDir());
        out << YAML::EndMap;
    }

    if (ent.Has<DirectionalLightComponent>()) {
        auto& light = ent.Get<DirectionalLightComponent>();
        out << YAML::Key << "DirectionalLightComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Direction" << YAML::Value << light.direction;
        out << YAML::Key << "Ambient" << YAML::Value << light.ambient;
        out << YAML::Key << "Diffuse" << YAML::Value << light.diffuse;
        out << YAML::Key << "Specular" << YAML::Value << light.specular;
        out << YAML::EndMap;
    }

    if (ent.Has<PointLightComponent>()) {
        auto& light = ent.Get<PointLightComponent>();
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

    if (ent.Has<SkyboxComponent>()) {
        auto& comp = ent.Get<SkyboxComponent>();
        out << YAML::Key << "SkyboxComponent" << YAML::Value << YAML::BeginMap;
        out << YAML::Key << "Directory" << YAML::Value << std::filesystem::relative(comp.directory, Settings::GetResourceDir());
        out << YAML::EndMap;
    }

    out << YAML::EndMap;
}
}

namespace PEANUT {
void SceneSerializer::Serialize(Scene& scene, const std::string& filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "MyTempSceneName";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    scene.ForEachEntity([&](Entity ent) {
        SerializeEntity(out, ent, filepath);
    });
    out << YAML::EndSeq;
    out << YAML::EndMap;

    std::ofstream ofs(filepath);
    ofs << out.c_str();
}

void SceneSerializer::Deserialize(const std::string& file, Scene& scene)
{
    const auto projectDir = std::filesystem::path(file).parent_path();
    scene.Clear();
    YAML::Node node = YAML::LoadFile(file);
    for (YAML::const_iterator it = node["Entities"].begin(); it != node["Entities"].end(); ++it) {
        const YAML::Node entity = *it;
        Entity sceneEnt = scene.CreateEntity();

        if (entity["TagComponent"]) {
            sceneEnt.Get<TagComponent>().tag = entity["TagComponent"]["Tag"].as<std::string>();
        }

        if (entity["TransformComponent"]) {
            sceneEnt.Get<TransformComponent>().translation = entity["TransformComponent"]["Translation"].as<glm::vec3>();
            sceneEnt.Get<TransformComponent>().rotation = entity["TransformComponent"]["Rotation"].as<glm::vec3>();
            sceneEnt.Get<TransformComponent>().scale = entity["TransformComponent"]["Scale"].as<glm::vec3>();
        }

        if (entity["SpriteRenderComponent"]) {
            auto& comp = sceneEnt.Add<SpriteRenderComponent>();
            comp.color = entity["SpriteRenderComponent"]["Color"].as<glm::vec3>();
            std::filesystem::path relative(entity["SpriteRenderComponent"]["Texture"].as<std::string>());
            comp.texture = (projectDir / relative).string();
        }

        if (entity["PythonScriptComponent"]) {
            auto& component = sceneEnt.Add<PythonScriptComponent>();
            std::filesystem::path relative(entity["PythonScriptComponent"]["Script"].as<std::string>());
            component.script = projectDir / relative;
        }

        if (entity["ModelFileComponent"]) {
            auto& component = sceneEnt.Add<ModelFileComponent>();
            std::filesystem::path relative(entity["ModelFileComponent"]["File"].as<std::string>());
            component.file = Settings::GetResourceDir() / relative;
        }

        if (entity["DirectionalLightComponent"]) {
            auto& comp = sceneEnt.Add<DirectionalLightComponent>();
            comp.direction = entity["DirectionalLightComponent"]["Direction"].as<glm::vec3>();
            comp.ambient = entity["DirectionalLightComponent"]["Ambient"].as<float>();
            comp.diffuse = entity["DirectionalLightComponent"]["Diffuse"].as<float>();
            comp.specular = entity["DirectionalLightComponent"]["Specular"].as<float>();
        }

        if (entity["PointLightComponent"]) {
            auto& comp = sceneEnt.Add<PointLightComponent>();
            comp.color = entity["PointLightComponent"]["Color"].as<glm::vec3>();
            comp.ambient = entity["PointLightComponent"]["Ambient"].as<float>();
            comp.diffuse = entity["PointLightComponent"]["Diffuse"].as<float>();
            comp.specular = entity["PointLightComponent"]["Specular"].as<float>();
            comp.constant = entity["PointLightComponent"]["Constant"].as<float>();
            comp.linear = entity["PointLightComponent"]["Linear"].as<float>();
            comp.quadratic = entity["PointLightComponent"]["Quadratic"].as<float>();
        }

        if (entity["SkyboxComponent"]) {
            auto& comp = sceneEnt.Add<SkyboxComponent>();
            std::filesystem::path rel = entity["SkyboxComponent"]["Directory"].as<std::string>();
            comp.directory = Settings::GetResourceDir() / rel;
        }
    }
}

} // namespace PEANUT