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
    ImGui::Begin("Scene");
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
    ImGui::End();
}

}
