#include "SceneHierarchyPanel.h"

namespace PEANUT
{

SceneHierarchyPanel::SceneHierarchyPanel(std::shared_ptr<Scene> scene) : m_scene(scene), m_selectedEntity() {}

void SceneHierarchyPanel::UpdateGui()
{
    UpdateMenuBar();

    ImGui::BeginChild("Scene");
    ImGui::Text("Scene Heirarchy");
    m_scene->ForEachEntity([&](Entity ent) {
        constexpr int treeNodeFlags = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
        TagComponent &tag = ent.Get<TagComponent>();
        if (ImGui::TreeNodeEx(tag.tag.c_str(), treeNodeFlags))
        {
            if (ImGui::TreeNodeEx("Test Sub Entity", treeNodeFlags))
            {
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

    UpdatePropertiesPanel();
}

void SceneHierarchyPanel::UpdateMenuBar()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
                LOG_INFO("Saving Scene...");
            }
            if (ImGui::MenuItem("Open"))
            {
                LOG_INFO("Opening Scene...");
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
}

void SceneHierarchyPanel::UpdatePropertiesPanel()
{
    ImGui::Begin("Properties Panel");
    if (m_selectedEntity)
    {
        DrawComponent<TagComponent>("Tag Component");
        DrawComponent<TransformComponent>("Transform");
        DrawComponent<SpriteRenderComponent>("Sprite Render Component");

        ImGui::Separator();
        if (ImGui::Button("Add Component"))
        {
            ImGui::OpenPopup("AddComponent");
        }
        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem("Sprite Render Component"))
            {
                m_selectedEntity.Add<SpriteRenderComponent>();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

template <>
void SceneHierarchyPanel::DrawComponent<TagComponent>(const std::string &componentName)
{
    char buf[256] = {0};
    TagComponent &tag = m_selectedEntity.Get<TagComponent>();
    std::strncpy(buf, tag.tag.c_str(), 256);
    if (ImGui::InputText("Tag", buf, 256))
    {
        tag.tag = buf;
    }
}

template <>
void SceneHierarchyPanel::DrawComponent<TransformComponent>(const std::string &componentName)
{
    ImGui::Separator();
    ImGui::Text(componentName.c_str());
    TransformComponent &transform = m_selectedEntity.Get<TransformComponent>();
    ImGui::SliderFloat3("Translation", glm::value_ptr(transform.translation), -1.0f, 1.0f);
    transform.rotation = glm::degrees(transform.rotation);
    ImGui::SliderFloat3("Rotation", glm::value_ptr(transform.rotation), 0.0f, 360.0f);
    transform.rotation = glm::radians(transform.rotation);
    ImGui::SliderFloat3("Scale", glm::value_ptr(transform.scale), 0.0f, 2.0f);
}

template <typename Component>
void SceneHierarchyPanel::DrawComponent(const std::string& componentName)
{
    if (m_selectedEntity.Has<Component>())
    {
        ImGui::Separator();
        ImGui::Text(componentName.c_str());
        ImGui::SameLine();
        if (ImGui::Button("X"))
        {
            m_selectedEntity.Remove<Component>();
        }
        else
        {
            DrawComponentSpecifics<Component>();
        }
    }
}

template <>
void SceneHierarchyPanel::DrawComponentSpecifics<SpriteRenderComponent>()
{
    ImGui::ColorEdit3("Color", glm::value_ptr(m_selectedEntity.Get<SpriteRenderComponent>().color));
}
}