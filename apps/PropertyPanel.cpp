#include "Engine.hpp"
#include "Input/FileSelectorDialog.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/NativeScript.h"
#include "Utils/Log.h"

#include <array>
#include <imgui.h>

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
void DrawComponentSpecifics<PythonScriptComponent>(Entity m_selectedEntity)
{
    auto& scriptComp = m_selectedEntity.Get<PythonScriptComponent>();
    ImGui::Text("%s", scriptComp.script.filename().c_str());
    if (ImGui::Button("Reload")) {
        ReloadPythonScript(m_selectedEntity);
    }
    auto& editor_fields = GetScriptEditorMembers(scriptComp.script_obj);
    for (auto& field : editor_fields) {
        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<int, T>) {
                ImGui::DragInt(field.first.c_str(), &arg);
            }
            if constexpr (std::is_same_v<float, T>) {
                ImGui::DragFloat(field.first.c_str(), &arg);
            }
            if constexpr (std::is_same_v<EditorButton, T>) {
                arg.pressed = ImGui::Button(field.first.c_str());
            }
        },
            field.second);
    }
}

template <>
void DrawComponentSpecifics<ModelFileComponent>(Entity ent)
{
    auto& comp = ent.Get<ModelFileComponent>();
    ImGui::Text("%s", comp.file.c_str());
    if (ImGui::Button("...")) {
        comp.file = CreateFileSelectorDialog()->OpenFile().value_or(comp.file);
    }
}

template <>
void DrawComponentSpecifics<DirectionalLightComponent>(Entity ent)
{
    auto& comp = ent.Get<DirectionalLightComponent>();
    ImGui::DragFloat3("Direction", glm::value_ptr(comp.direction), 0.01f, -1.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Ambient", &comp.ambient, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Diffuse", &comp.diffuse, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Specular", &comp.specular, 0.01f, 0.0f, 1.0f, "%.2f");
}

template <>
void DrawComponentSpecifics<PointLightComponent>(Entity ent)
{
    auto& comp = ent.Get<PointLightComponent>();
    ImGui::DragFloat3("Color", glm::value_ptr(comp.color), 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Ambient", &comp.ambient, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Diffuse", &comp.diffuse, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Specular", &comp.specular, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Constant", &comp.constant, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Linear", &comp.linear, 0.001f, 0.0f, 1.0f, "%.3f");
    ImGui::DragFloat("Quadratic", &comp.quadratic, 0.001f, 0.0f, 2.0f, "%.3f");
}

template <>
void DrawComponentSpecifics<SkyboxComponent>(Entity ent)
{
    auto& comp = ent.Get<SkyboxComponent>();
    ImGui::Text("%s", comp.directory.c_str());
    if (ImGui::Button("...")) {
        comp.directory = CreateFileSelectorDialog()->OpenFile().value_or(comp.directory);
    }
}

void DrawCustomComponents(Entity ent, Engine* engine)
{
    for (const auto& comp : engine->GetPlugins()) {
        if (ent.Has<NativeScript>(comp.name)) {
            ImGui::PushID(comp.name.c_str());
            ImGui::Separator();
            ImGui::Text("%s", comp.name.c_str());
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                ent.Remove<NativeScript>(comp.name);
            } else {
                for (const auto& member : ent.Get<NativeScript>(comp.name)->GetMembers()) {
                    if (member.type == MemberVariable::Type::Bool) {
                        ImGui::Checkbox(member.name.c_str(), static_cast<bool*>(member.addr));
                    }
                    if (member.type == MemberVariable::Type::Float) {
                        ImGui::DragFloat(member.name.c_str(), static_cast<float*>(member.addr));
                    }
                    if (member.type == MemberVariable::Type::Double) {
                        ImGui::InputDouble(member.name.c_str(), static_cast<double*>(member.addr));
                    }
                    if (member.type == MemberVariable::Type::Int) {
                        ImGui::DragInt(member.name.c_str(), static_cast<int*>(member.addr));
                    }
                    if (member.type == MemberVariable::Type::String) {
                        std::array<char, 256> buf = {};
                        if (ImGui::InputText(member.name.c_str(), buf.data(), 256)) {
                            *static_cast<std::string*>(member.addr) = buf.data();
                        }
                    }
                }
            }
            ImGui::PopID();
        }
    }
}

template <>
void DrawComponentSpecifics<CustomModelComponent>(Entity)
{
    ImGui::Text("Custom Model");
}

void UpdatePropertiesPanelImpl(Entity m_selectedEntity, Engine* engine)
{
    ImGui::Begin("Properties Panel");
    if (m_selectedEntity) {
        DrawComponent<TagComponent>("Tag", m_selectedEntity);
        DrawComponent<TransformComponent>("Transform", m_selectedEntity);
        DrawComponent<SpriteRenderComponent>("Sprite Render", m_selectedEntity);
        DrawComponent<PythonScriptComponent>("Python Script", m_selectedEntity);
        DrawComponent<ModelFileComponent>("Model File", m_selectedEntity);
        DrawComponent<DirectionalLightComponent>("Directional Light", m_selectedEntity);
        DrawComponent<PointLightComponent>("Point Light", m_selectedEntity);
        DrawComponent<SkyboxComponent>("Skybox", m_selectedEntity);
        DrawComponent<CustomModelComponent>("Custom Model", m_selectedEntity);
        DrawCustomComponents(m_selectedEntity, engine);

        ImGui::Separator();
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponent");
        }
        if (ImGui::BeginPopup("AddComponent")) {
            if (ImGui::MenuItem("Sprite Render Component")) {
                auto& comp = m_selectedEntity.Add<SpriteRenderComponent>();
                comp.color = { 1.0, 1.0, 1.0 };
                comp.texture = "textures/BlankSquare.png";
            }
            if (ImGui::MenuItem("Python Script")) {
                auto scriptFile = CreateFileSelectorDialog()->OpenFile().value_or("");
                if (std::filesystem::exists(scriptFile)) {
                    auto& comp = m_selectedEntity.Add<PythonScriptComponent>();
                    comp.script = scriptFile;
                    LoadPythonScriptObj(m_selectedEntity);
                }
            }
            if (ImGui::MenuItem("Model File")) {
                auto file = CreateFileSelectorDialog()->OpenFile().value_or("");
                if (std::filesystem::exists(file)) {
                    auto& comp = m_selectedEntity.Add<ModelFileComponent>();
                    comp.file = file;
                }
            }
            if (ImGui::MenuItem("Directional Light")) {
                m_selectedEntity.Add<DirectionalLightComponent>();
            }
            if (ImGui::MenuItem("Point Light")) {
                m_selectedEntity.Add<PointLightComponent>();
            }
            if (ImGui::MenuItem("Skybox")) {
                auto directory = CreateFileSelectorDialog()->OpenDirectory().value_or("");
                if (std::filesystem::exists(directory)) {
                    auto& comp = m_selectedEntity.Add<SkyboxComponent>();
                    comp.directory = directory;
                }
            }
            if (ImGui::MenuItem("Custom Model")) {
                auto& model = m_selectedEntity.Add<CustomModelComponent>();
                model.mesh = GetCubeMesh();
                CreateCustomModel(model);
            }
            for (const auto& plugin : engine->GetPlugins()) {
                if (ImGui::MenuItem(plugin.name.c_str())) {
                    m_selectedEntity.Add<NativeScript>(plugin.name, plugin.getNewComponent());
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

}

namespace PEANUT {
void UpdatePropertiesPanel(Entity selectedEntity, Engine* engine)
{
    UpdatePropertiesPanelImpl(selectedEntity, engine);
}
}