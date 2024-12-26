#include "SceneHierarchyPanel.hpp"

#include <peanut/Component.hpp>
#include <peanut/Log.hpp>

// external
#include <imgui.h>

// stl
#include <cstdlib>

namespace PEANUT {

void SceneHierarchyPanel::Update()
{
    ImGui::Text("Scene Heirarchy");
    Engine()->GetScene()->ForEachEntity([&](Entity ent) {
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
            m_selectedEntity = Engine()->GetScene()->CreateEntity("New Entity");
        }
        ImGui::EndPopup();
    }
}

}
