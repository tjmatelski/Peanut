#include "SceneHierarchyPanel.h"

namespace PEANUT
{

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<Scene> scene) : m_scene(scene), m_selectedEntity() {}

void SceneHierarchyPanel::UpdateGui()
{
    ImGui::BeginChild("Scene", ImVec2(0, 200));
    ImGui::Text("Scene Heirarchy");
    m_scene->ForEachEntity([&](Entity ent) {
        TagComponent &tag = ent.Get<TagComponent>();
        if (ImGui::TreeNode(tag.tag.c_str()))
        {
            if (ImGui::TreeNode("Test Sub Entity"))
            {
                ImGui::Text("Test Sub Sub ent");
                ImGui::TreePop();
            }
            ImGui::TreePop();
        }
        if (ImGui::IsItemClicked())
        {
            m_selectedEntity = ent;
            LOG_INFO("Clicked the item {0}", tag.tag);
        }
    });
    if (ImGui::BeginPopupContextWindow("Scene Hierarchy Popup"))
    {
        if (ImGui::MenuItem("Create Empty Entity"))
        {
            m_selectedEntity = m_scene->CreateEntity("New Entity");
        }
        ImGui::EndPopup();
    }
    ImGui::EndChild();

    ImGui::Begin("Properties Panel");
    if (m_selectedEntity)
    {
        char buf[256] = {0};
        TagComponent &tag = m_selectedEntity.Get<TagComponent>();
        std::strncpy(buf, tag.tag.c_str(), 256);
        if (ImGui::InputText("Tag", buf, 256))
        {
            tag.tag = buf;
        }

        ImGui::Separator();
        
        ImGui::Text("Transform");
        TransformComponent& transform = m_selectedEntity.Get<TransformComponent>();
        ImGui::SliderFloat3("Translation", glm::value_ptr(transform.translation), -1.0f, 1.0f);
        transform.rotation = glm::degrees(transform.rotation);
        ImGui::SliderFloat3("Rotation", glm::value_ptr(transform.rotation), 0.0f, 360.0f);
        transform.rotation = glm::radians(transform.rotation);
        ImGui::SliderFloat3("Scale", glm::value_ptr(transform.scale), 0.0f, 2.0f);
    }
    ImGui::End();
}
}