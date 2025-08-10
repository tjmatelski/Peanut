#include "PropertyPanel.hpp"

#include <peanut/Component.hpp>
#include <peanut/Engine.hpp>
#include <peanut/Entity.hpp>
#include <peanut/FileSelectorDialog.hpp>
#include <peanut/Log.hpp>
#include <peanut/ModelLibrary.hpp>
#include <peanut/NativeScript.hpp>
#include <peanut/ShaderLibrary.hpp>

// external
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

// stl
#include <array>
#include <vector>

using namespace PEANUT;

namespace {
template <class... Ts> struct Overloaded : Ts... {
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts> Overloaded(Ts...) -> Overloaded<Ts...>;

void DrawRenderable(Renderable& renderable)
{
    ImGui::Text("Mesh Placeholder");

    ImGui::Separator();

    auto& material = renderable.material_;
    for (auto& [name, value] : material.Uniforms()) {

        // Display modifiable uniform name
        std::array<char, 256> name_buf;
        std::strncpy(name_buf.data(), name.c_str(), name_buf.size());
        const auto label = "Name##" + name; // Separate labels so one name is modified at a time
        if (ImGui::InputText(label.c_str(), name_buf.data(), name_buf.size(),
                ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
            LOG_DEBUG("renaming uniform [{}]", name);
            material.Uniforms().erase(name);
            material.SetUniform(std::string { name_buf.data() }, value);
            break;
        }

        // Button to remove the unform on same line
        ImGui::SameLine();
        if (ImGui::Button("X")) {
            LOG_DEBUG("removing uniform [{}]", name);
            material.Uniforms().erase(name);
            break;
        }

        // Modifiable value of the uniform
        std::visit(
            Overloaded { [&](std::same_as<bool> auto& value) {
                            value = ImGui::Button(value ? "True" : "False") ? !value : value;
                        },
                [&](std::same_as<int> auto& value) { ImGui::DragInt("##uniform", &value); },
                [&](std::same_as<unsigned int> auto& value) {
                    ImGui::DragScalar("##uniform", ImGuiDataType_::ImGuiDataType_U32, &value);
                },
                [&](std::same_as<float> auto& value) { ImGui::DragFloat("##uniform", &value); },
                [&](std::same_as<glm::vec2> auto& value) { ImGui::DragFloat2("##uniform", glm::value_ptr(value)); },
                [&](std::same_as<glm::vec3> auto& value) { ImGui::DragFloat3("##uniform", glm::value_ptr(value)); },
                [&](std::same_as<glm::vec4> auto& value) { ImGui::DragFloat4("##uniform", glm::value_ptr(value)); },
                [&](std::same_as<glm::mat2> auto& value) {
                    ImGui::DragScalarN("##uniform", ImGuiDataType_::ImGuiDataType_Float, glm::value_ptr(value), 4);
                },
                [&](std::same_as<glm::mat3> auto& value) {
                    ImGui::DragScalarN("##uniform", ImGuiDataType_::ImGuiDataType_Float, glm::value_ptr(value), 9);
                },
                [&](std::same_as<glm::mat4> auto& value) {
                    ImGui::DragScalarN("##uniform", ImGuiDataType_::ImGuiDataType_Float, glm::value_ptr(value), 16);
                } },
            value);
    }

    // Button to add new uniforms
    if (ImGui::Button("Add Uniform")) {
        ImGui::OpenPopup("AddUniform");
    }
    if (ImGui::BeginPopup("AddUniform")) {
        if (ImGui::MenuItem("bool")) {
            material.SetUniform("new_uniform", false);
        }
        if (ImGui::MenuItem("int")) {
            material.SetUniform("new_uniform", int(0));
        }
        if (ImGui::MenuItem("unsigned int")) {
            material.SetUniform("new_uniform", 0U);
        }
        if (ImGui::MenuItem("float")) {
            material.SetUniform("new_uniform", 0.0f);
        }
        if (ImGui::MenuItem("vec2")) {
            material.SetUniform("new_uniform", glm::vec2 {});
        }
        if (ImGui::MenuItem("vec3")) {
            material.SetUniform("new_uniform", glm::vec3 {});
        }
        if (ImGui::MenuItem("vec4")) {
            material.SetUniform("new_uniform", glm::vec4 {});
        }
        if (ImGui::MenuItem("mat2")) {
            material.SetUniform("new_uniform", glm::mat2 {});
        }
        if (ImGui::MenuItem("mat3")) {
            material.SetUniform("new_uniform", glm::mat3 {});
        }
        if (ImGui::MenuItem("mat4")) {
            material.SetUniform("new_uniform", glm::mat4 {});
        }
        ImGui::EndPopup();
    }

    ImGui::Separator();

    ImGui::Text("Shader: %s", renderable.shader_->ShaderFile().c_str());
    ImGui::SameLine();
    if (ImGui::Button("...")) {
        auto new_shader = CreateFileSelectorDialog()->OpenFile();
        if (new_shader) {
            renderable.shader_ = ShaderLibrary::Get(new_shader.value());
        }
    }
}
}

template <> void PropertyPanel::DrawComponent<TagComponent>(const std::string& componentName)
{
    ImGui::Text("%s", componentName.c_str());
    auto& tag = m_sh_panel->GetSelectedEntity().Get<TagComponent>();
    std::array<char, 256> buf;
    std::strncpy(buf.data(), tag.tag.c_str(), buf.size());
    if (ImGui::InputText(
            "Tag", buf.data(), buf.size(), ImGuiInputTextFlags_AutoSelectAll | ImGuiInputTextFlags_EnterReturnsTrue)) {
        tag.tag = buf.data();
    }
}

template <> void PropertyPanel::DrawComponent<TransformComponent>(const std::string& componentName)
{
    ImGui::Separator();
    ImGui::Text("%s", componentName.c_str());
    auto& transform = m_sh_panel->GetSelectedEntity().Get<TransformComponent>();
    ImGui::DragFloat3("Translation", glm::value_ptr(transform.translation), 0.2f);
    transform.rotation = glm::degrees(transform.rotation);
    ImGui::DragFloat3("Rotation", glm::value_ptr(transform.rotation), 1.0f, 0.0f, 360.0f, "%.2f deg");
    transform.rotation = glm::radians(transform.rotation);
    ImGui::DragFloat3("Scale", glm::value_ptr(transform.scale), 0.2, 0.0f, FLT_MAX);
}

template <typename Component> void PropertyPanel::DrawComponent(const std::string& componentName)
{
    if (m_sh_panel->GetSelectedEntity().Has<Component>()) {

        constexpr int treeNodeFlags
            = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
        if (ImGui::TreeNodeEx(componentName.c_str(), treeNodeFlags)) {
            ImGui::SameLine(); // TODO: Calc width so "X" is at end
            if (ImGui::Button("X")) {
                m_sh_panel->GetSelectedEntity().Remove<Component>();
            } else {
                DrawComponentSpecifics<Component>();
            }
            ImGui::TreePop();
        }
    }
}

template <> void PropertyPanel::DrawComponentSpecifics<SpriteRenderComponent>()
{
    auto& renderComp = m_sh_panel->GetSelectedEntity().Get<SpriteRenderComponent>();
    ImGui::ColorEdit3("Color", glm::value_ptr(renderComp.color));
    ImGui::Text("%s", renderComp.texture.c_str());
    if (ImGui::Button("...")) {
        renderComp.texture = CreateFileSelectorDialog()->OpenFile().value_or(renderComp.texture);
    }
}

template <> void PropertyPanel::DrawComponentSpecifics<PythonScriptComponent>()
{
    auto& scriptComp = m_sh_panel->GetSelectedEntity().Get<PythonScriptComponent>();
    ImGui::Text("%s", scriptComp.script.filename().c_str());
    if (ImGui::Button("Reload")) {
        Engine()->ReloadPythonScript(m_sh_panel->GetSelectedEntity());
    }
    auto& editor_fields = Engine()->GetScriptEditorMembers(scriptComp.script_obj);
    for (auto& field : editor_fields) {
        std::visit(
            [&](auto&& arg) {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<int, T>) {
                    ImGui::DragInt(field.first.c_str(), &arg);
                }
                if constexpr (std::is_same_v<float, T>) {
                    ImGui::DragFloat(field.first.c_str(), &arg);
                }
                if constexpr (std::is_same_v<PEANUT::Engine::EditorButton, T>) {
                    arg.pressed = ImGui::Button(field.first.c_str());
                }
            },
            field.second);
    }
}

template <> void PropertyPanel::DrawComponentSpecifics<ModelFileComponent>()
{
    auto& comp = m_sh_panel->GetSelectedEntity().Get<ModelFileComponent>();
    ImGui::Text("%s", comp.file.c_str());
    if (ImGui::Button("...")) {
        comp.file = CreateFileSelectorDialog()->OpenFile().value_or(comp.file);
    }

    for (auto& renderable : ModelLibrary::Get(comp.file).GetRenderables()) {
        constexpr int treeNodeFlags
            = ImGuiTreeNodeFlags_Selected | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow;
        if (ImGui::TreeNodeEx("renderable", treeNodeFlags)) {
            DrawRenderable(renderable);
            ImGui::TreePop();
        }
    }
}

template <> void PropertyPanel::DrawComponentSpecifics<DirectionalLightComponent>()
{
    auto& comp = m_sh_panel->GetSelectedEntity().Get<DirectionalLightComponent>();
    ImGui::DragFloat3("Direction", glm::value_ptr(comp.direction), 0.01f, -1.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Ambient", &comp.ambient, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Diffuse", &comp.diffuse, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Specular", &comp.specular, 0.01f, 0.0f, 1.0f, "%.2f");
}

template <> void PropertyPanel::DrawComponentSpecifics<PointLightComponent>()
{
    auto& comp = m_sh_panel->GetSelectedEntity().Get<PointLightComponent>();
    ImGui::DragFloat3("Color", glm::value_ptr(comp.color), 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Ambient", &comp.ambient, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Diffuse", &comp.diffuse, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Specular", &comp.specular, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Constant", &comp.constant, 0.01f, 0.0f, 1.0f, "%.2f");
    ImGui::DragFloat("Linear", &comp.linear, 0.001f, 0.0f, 1.0f, "%.3f");
    ImGui::DragFloat("Quadratic", &comp.quadratic, 0.001f, 0.0f, 2.0f, "%.3f");
}

template <> void PropertyPanel::DrawComponentSpecifics<SkyboxComponent>()
{
    auto& comp = m_sh_panel->GetSelectedEntity().Get<SkyboxComponent>();
    ImGui::Text("%s", comp.directory.c_str());
    if (ImGui::Button("...")) {
        comp.directory = CreateFileSelectorDialog()->OpenFile().value_or(comp.directory);
    }
}

template <> void PropertyPanel::DrawComponentSpecifics<Renderable>()
{
    auto& renderable = m_sh_panel->GetSelectedEntity().Get<Renderable>();
    DrawRenderable(renderable);
}

void PropertyPanel::DrawCustomComponents()
{
    for (const auto& comp : Engine()->GetPlugins()) {
        if (m_sh_panel->GetSelectedEntity().Has<NativeScript>(comp.name)) {
            ImGui::PushID(comp.name.c_str());
            ImGui::Separator();
            ImGui::Text("%s", comp.name.c_str());
            ImGui::SameLine();
            if (ImGui::Button("X")) {
                m_sh_panel->GetSelectedEntity().Get<NativeScript>(comp.name)->OnDestroy();
                m_sh_panel->GetSelectedEntity().Remove<NativeScript>(comp.name);
            } else {
                if (ImGui::Button("Reload")) {
                    m_sh_panel->GetSelectedEntity().Get<NativeScript>(comp.name)->OnDestroy();
                    m_sh_panel->GetSelectedEntity().Remove<NativeScript>(comp.name);
                    Engine()->ReloadPlugin(comp.name);
                    m_sh_panel->GetSelectedEntity().Add<NativeScript>(comp.name, comp.getNewComponent());
                    m_sh_panel->GetSelectedEntity().Get<NativeScript>(comp.name)->OnCreate();
                }
                for (const auto& member : m_sh_panel->GetSelectedEntity().Get<NativeScript>(comp.name)->GetMembers()) {
                    if (member.type == MemberVariable::Type::Bool) {
                        ImGui::Checkbox(member.name.c_str(), static_cast<bool*>(member.addr));
                    }
                    if (member.type == MemberVariable::Type::Float) {
                        ImGui::DragFloat(member.name.c_str(), static_cast<float*>(member.addr));
                    }
                    if (member.type == MemberVariable::Type::Double) {
                        ImGui::DragScalar(member.name.c_str(), ImGuiDataType_Double, static_cast<double*>(member.addr));
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

void PropertyPanel::Update()
{
    if (m_sh_panel->GetSelectedEntity()) {
        DrawComponent<TagComponent>("Tag");
        DrawComponent<TransformComponent>("Transform");
        DrawComponent<SpriteRenderComponent>("Sprite Render");
        DrawComponent<PythonScriptComponent>("Python Script");
        DrawComponent<ModelFileComponent>("Model File");
        DrawComponent<Renderable>("Renderable");
        DrawComponent<DirectionalLightComponent>("Directional Light");
        DrawComponent<PointLightComponent>("Point Light");
        DrawComponent<SkyboxComponent>("Skybox");
        DrawCustomComponents();

        ImGui::Separator();
        if (ImGui::Button("Add Component")) {
            ImGui::OpenPopup("AddComponent");
        }
        if (ImGui::BeginPopup("AddComponent")) {
            if (ImGui::MenuItem("Sprite Render Component")) {
                auto& comp = m_sh_panel->GetSelectedEntity().Add<SpriteRenderComponent>();
                comp.color = { 1.0, 1.0, 1.0 };
                comp.texture = "textures/BlankSquare.png";
            }
            if (ImGui::MenuItem("Python Script")) {
                auto scriptFile = CreateFileSelectorDialog()->OpenFile().value_or("");
                if (std::filesystem::exists(scriptFile)) {
                    auto& comp = m_sh_panel->GetSelectedEntity().Add<PythonScriptComponent>();
                    comp.script = scriptFile;
                    Engine()->LoadPythonScriptObj(m_sh_panel->GetSelectedEntity());
                }
            }
            if (ImGui::MenuItem("Model File")) {
                auto file = CreateFileSelectorDialog()->OpenFile().value_or("");
                if (std::filesystem::exists(file)) {
                    auto& comp = m_sh_panel->GetSelectedEntity().Add<ModelFileComponent>();
                    comp.file = file;
                }
            }
            if (ImGui::MenuItem("Directional Light")) {
                m_sh_panel->GetSelectedEntity().Add<DirectionalLightComponent>();
            }
            if (ImGui::MenuItem("Point Light")) {
                m_sh_panel->GetSelectedEntity().Add<PointLightComponent>();
            }
            if (ImGui::MenuItem("Skybox")) {
                auto directory = CreateFileSelectorDialog()->OpenDirectory().value_or("");
                if (std::filesystem::exists(directory)) {
                    auto& comp = m_sh_panel->GetSelectedEntity().Add<SkyboxComponent>();
                    comp.directory = directory;
                }
            }
            for (const auto& plugin : Engine()->GetPlugins()) {
                if (ImGui::MenuItem(plugin.name.c_str())) {
                    m_sh_panel->GetSelectedEntity().Add<NativeScript>(plugin.name, plugin.getNewComponent());
                    m_sh_panel->GetSelectedEntity().Get<NativeScript>(plugin.name)->OnCreate();
                }
            }
            ImGui::EndPopup();
        }
    }
}
