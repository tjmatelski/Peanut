#include "SceneHierarchyPanel.h"
#include "../src/Settings.h"
#include "ScriptLibrary.h"

#include <cstdlib>

namespace PEANUT {

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<Scene> scene)
    : m_scene(scene)
    , m_selectedEntity()
{
}

void SceneHierarchyPanel::UpdateGui()
{
    UpdateMenuBar();

    ImGui::BeginChild("Scene");
    ImGui::Text("Scene Heirarchy");
    m_scene->ForEachEntity([&](Entity ent) {
        constexpr int treeNodeFlags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
        TagComponent& tag = ent.Get<TagComponent>();
        if (ImGui::TreeNodeEx(tag.tag.c_str(), treeNodeFlags)) {
            if (ImGui::TreeNodeEx("Test Sub Entity", treeNodeFlags)) {
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        if (ImGui::IsItemClicked()) {
            m_selectedEntity = ent;
            LOG_INFO("Clicked the item {0}", tag.tag);
        }
    });
    if (ImGui::BeginPopupContextWindow("Scene Hierarchy Popup")) {
        if (ImGui::MenuItem("Create Empty Entity")) {
            m_selectedEntity = m_scene->CreateEntity("New Entity");
        }
        ImGui::EndPopup();
    }
    ImGui::EndChild();

    UpdatePropertiesPanel();
}

void SceneHierarchyPanel::UpdateMenuBar()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save")) {
                LOG_INFO("Saving Scene...");
                SceneSerializer::Serialize(*m_scene, "./test.peanut");
            }
            if (ImGui::MenuItem("Open")) {
                std::string sceneFile = CreateFileSelectorDialog()->SelectFile().value_or("");
                if (sceneFile.find(".peanut") != std::string::npos) {
                    LOG_INFO("Opening Scene: {}", sceneFile);
                    SceneSerializer::Deserialize(sceneFile, *m_scene);
                } else {
                    LOG_ERROR("Invalid scene file: {}", sceneFile);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void SceneHierarchyPanel::UpdatePropertiesPanel()
{
    ImGui::Begin("Properties Panel");
    if (m_selectedEntity) {
        DrawComponent<TagComponent>("Tag");
        DrawComponent<TransformComponent>("Transform");
        DrawComponent<SpriteRenderComponent>("Sprite Render");
        DrawComponent<NativeScriptComponent>("Native Script");

        ImGui::Separator();
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponent");
        }
        if (ImGui::BeginPopup("AddComponent")) {
            if (ImGui::MenuItem("Sprite Render Component")) {
                auto& comp = m_selectedEntity.Add<SpriteRenderComponent>();
                comp.color = { 1.0, 1.0, 1.0 };
                comp.texture = Settings::GetResourceDir() / "textures" / "BlankSquare.png";
            }
            if (ImGui::MenuItem("Native Script Component")) {
                std::filesystem::path scriptFile = CreateFileSelectorDialog()->SelectFile().value_or("");
                if (!scriptFile.empty()) {
                    m_selectedEntity.Add<NativeScriptComponent>(ScriptLibrary::Load(scriptFile), scriptFile)
                        .m_script->m_entity
                        = m_selectedEntity;
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

template <>
void SceneHierarchyPanel::DrawComponent<TagComponent>(const std::string& componentName)
{
    char buf[256] = { 0 };
    TagComponent& tag = m_selectedEntity.Get<TagComponent>();
    std::strncpy(buf, tag.tag.c_str(), 256);
    if (ImGui::InputText("Tag", buf, 256)) {
        tag.tag = buf;
    }
}

template <>
void SceneHierarchyPanel::DrawComponent<TransformComponent>(const std::string& componentName)
{
    ImGui::Separator();
    ImGui::Text(componentName.c_str());
    TransformComponent& transform = m_selectedEntity.Get<TransformComponent>();
    ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.2f);
    transform.rotation = glm::degrees(transform.rotation);
    ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 1.0f, 0.0f, 360.0f, "%.2f deg");
    transform.rotation = glm::radians(transform.rotation);
    ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.2, 0.0f, FLT_MAX);
}

template <typename Component>
void SceneHierarchyPanel::DrawComponent(const std::string& componentName)
{
    if (m_selectedEntity.Has<Component>()) {
        ImGui::PushID(componentName.c_str());
        ImGui::Separator();
        ImGui::Text(componentName.c_str());
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            m_selectedEntity.Remove<Component>();
        } else {
            DrawComponentSpecifics<Component>();
        }
        ImGui::PopID();
    }
}

template <>
void SceneHierarchyPanel::DrawComponentSpecifics<SpriteRenderComponent>()
{
    auto& renderComp = m_selectedEntity.Get<SpriteRenderComponent>();
    ImGui::ColorEdit3("Color", glm::value_ptr(renderComp.color));
    ImGui::Text(renderComp.texture.c_str());
    if (ImGui::Button("...")) {
        renderComp.texture = CreateFileSelectorDialog()->SelectFile().value_or(renderComp.texture);
    }
}

template <>
void SceneHierarchyPanel::DrawComponentSpecifics<NativeScriptComponent>()
{
    auto& script = m_selectedEntity.Get<NativeScriptComponent>();
    ImGui::Text(script.filename.c_str());
    if (ImGui::Button("Reload Script")) {
        std::string command = "cmake --build \"./build/\" --target ";
        command += script.filename.stem().string();
        int result = std::system(command.c_str());
        if (result != 0) {
            LOG_ERROR("Something went wrong with command: {}", command);
        } else {
            script.m_script = ScriptLibrary::Load(script.filename);
            script.m_script->m_entity = m_selectedEntity;
        }
    }
}

}