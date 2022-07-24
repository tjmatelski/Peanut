#include "../src/Settings.h" // TODO: Handle settings properly.
#include "Scene/Entity.h"
#include <Peanut.h>

namespace {

using namespace PEANUT;

template <typename Component>
void DrawComponent(const std::string& componentName, Entity m_selectedEntity);
template <typename Component>
void DrawComponentSpecifics(Entity m_selectedEntity);

template <>
void DrawComponent<TagComponent>(const std::string& componentName, Entity m_selectedEntity)
{
    ImGui::Text("%s", componentName.c_str());
    auto& tag = m_selectedEntity.Get<TagComponent>();
    std::array<char, 256> buf;
    std::strncpy(buf.data(), tag.tag.c_str(), buf.size());
    if (ImGui::InputText("Tag", buf.data(), buf.size())) {
        tag.tag = buf.data();
    }
}

template <>
void DrawComponent<TransformComponent>(const std::string& componentName, Entity m_selectedEntity)
{
    ImGui::Separator();
    ImGui::Text("%s", componentName.c_str());
    auto& transform = m_selectedEntity.Get<TransformComponent>();
    ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.2f);
    transform.rotation = glm::degrees(transform.rotation);
    ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 1.0f, 0.0f, 360.0f, "%.2f deg");
    transform.rotation = glm::radians(transform.rotation);
    ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.2, 0.0f, FLT_MAX);
}

template <typename Component>
void DrawComponent(const std::string& componentName, Entity m_selectedEntity)
{
    if (m_selectedEntity.Has<Component>()) {
        ImGui::PushID(componentName.c_str());
        ImGui::Separator();
        ImGui::Text("%s", componentName.c_str());
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            m_selectedEntity.Remove<Component>();
        } else {
            DrawComponentSpecifics<Component>(m_selectedEntity);
        }
        ImGui::PopID();
    }
}

template <>
void DrawComponentSpecifics<SpriteRenderComponent>(Entity m_selectedEntity)
{
    auto& renderComp = m_selectedEntity.Get<SpriteRenderComponent>();
    ImGui::ColorEdit3("Color", glm::value_ptr(renderComp.color));
    ImGui::Text("%s", renderComp.texture.c_str());
    if (ImGui::Button("...")) {
        renderComp.texture = CreateFileSelectorDialog()->OpenFile().value_or(renderComp.texture);
    }
}

template <>
void DrawComponentSpecifics<LuaScriptComponent>(Entity m_selectedEntity)
{
    auto& scriptComp = m_selectedEntity.Get<LuaScriptComponent>();
    ImGui::Text("%s", scriptComp.script.filename().c_str());
    if (ImGui::Button("...")) {
        scriptComp.script = CreateFileSelectorDialog()->OpenFile().value_or(scriptComp.script);
    }
}

void UpdatePropertiesPanelImpl(Entity m_selectedEntity)
{
    ImGui::Begin("Properties Panel");
    if (m_selectedEntity) {
        DrawComponent<TagComponent>("Tag", m_selectedEntity);
        DrawComponent<TransformComponent>("Transform", m_selectedEntity);
        DrawComponent<SpriteRenderComponent>("Sprite Render", m_selectedEntity);
        DrawComponent<LuaScriptComponent>("LUA Script", m_selectedEntity);

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
            if (ImGui::MenuItem("LUA Script")) {
                auto scriptFile = CreateFileSelectorDialog()->OpenFile().value_or("");
                if (std::filesystem::exists(scriptFile)) {
                    auto& comp = m_selectedEntity.Add<LuaScriptComponent>();
                    comp.script = scriptFile;
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

}

namespace PEANUT {
void UpdatePropertiesPanel(Entity selectedEntity)
{
    UpdatePropertiesPanelImpl(selectedEntity);
}
}