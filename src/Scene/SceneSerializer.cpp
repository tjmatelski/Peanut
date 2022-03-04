#include "../Settings.h"
#include <Scene/Component.h>
#include <Scene/Entity.h>
#include <Scene/Scene.h>
#include <Scene/SceneSerializer.h>
#include <ScriptLibrary.h>
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

namespace PEANUT {
static YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& vec)
{
    out << YAML::Flow;
    out << YAML::BeginSeq << vec.x << vec.y << vec.z << YAML::EndSeq;
    return out;
}

static void SerializeEntity(YAML::Emitter& out, Entity ent)
{
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

    if (ent.Has<NativeScriptComponent>()) {
        out << YAML::Key << "NativeScriptComponent" << YAML::Value << YAML::BeginMap;
        const NativeScriptComponent& script = ent.Get<NativeScriptComponent>();
        out << YAML::Key << "ScriptFile" << YAML::Value << std::filesystem::relative(script.filename, Settings::GetResourceDir());
        out << YAML::EndMap;
    }

    out << YAML::EndMap;
}

void SceneSerializer::Serialize(Scene& scene, const std::string& filepath)
{
    YAML::Emitter out;
    out << YAML::BeginMap;
    out << YAML::Key << "Scene" << YAML::Value << "MyTempSceneName";
    out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
    scene.ForEachEntity([&](Entity ent) {
        SerializeEntity(out, ent);
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

        if (entity["NativeScriptComponent"]) {
            std::filesystem::path scriptFile = projectDir / entity["NativeScriptComponent"]["ScriptFile"].as<std::string>();
            auto& script = sceneEnt.Add<NativeScriptComponent>(ScriptLibrary::Load(scriptFile), scriptFile);
            script.m_script->m_entity = sceneEnt;
        }
    }
}

} // namespace PEANUT