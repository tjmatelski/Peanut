#include "SceneHierarchyPanel.h"
#include "../src/Settings.h"
#include "Scene/Component.h"
#include "imgui.h"

#include <cstdlib>
#include <filesystem>

namespace PEANUT {

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<Scene> scene)
    : m_scene(std::move(scene))
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
        auto& tag = ent.Get<TagComponent>();
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
}

void SceneHierarchyPanel::UpdateMenuBar()
{
    if (ImGui::BeginMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Save")) {
                auto saveFile = CreateFileSelectorDialog()->SaveFile().value_or("");
                if (!saveFile.empty()) {
                    LOG_INFO("Saving Scene to '{0}'", saveFile);
                    SceneSerializer::Serialize(*m_scene, saveFile);
                } else {
                    LOG_WARN("Failed to select save file. Not saving scene.");
                }
            }
            if (ImGui::MenuItem("Open")) {
                std::string sceneFile = CreateFileSelectorDialog()->OpenFile().value_or("");
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

}
