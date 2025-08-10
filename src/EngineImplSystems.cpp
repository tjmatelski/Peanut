#include "EngineImpl.hpp"

// peanut
#include "PythonBindings.hpp"
#include "Renderer/Renderer.hpp"
#include "Renderer/TextureLibrary.hpp"
#include "peanut/ShaderLibrary.hpp"

// external

// stl

// Forward decls

namespace PEANUT {

void EngineImpl::SkyboxSystem(const SkyboxComponent& skybox)
{
    // Load data
    static const auto mesh = Renderer::GetSkyboxMesh();
    static const Material material {};
    const auto* p_tex = TextureLibrary::GetCubemap(skybox.directory);

    // Build command
    RenderCommand command;
    command.p_shader_ = ShaderLibrary::Get("./res/shaders/Skybox.shader");
    command.p_shader_->SetUniform({ "view", glm::mat4(glm::mat3(m_perspectiveCam.GetViewMatrix())) });
    command.p_shader_->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
    command.p_mesh_ = &mesh;
    command.p_material_ = &material;
    command.textures_.emplace_back("skybox", p_tex);

    // Draw
    Renderer::DisableDepthMask();
    Renderer::Draw(command);
    Renderer::EnableDepthMask();
}

void EngineImpl::DirLightSystem(const DirectionalLightComponent& comp)
{
    Renderer::SetDirectionalLight(
        { comp.direction, { comp.ambient, comp.ambient, comp.ambient }, { comp.diffuse, comp.diffuse, comp.diffuse },
            { comp.specular, comp.specular, comp.specular } },
        *ShaderLibrary::Get("./res/shaders/Lighting.shader"));
}

void EngineImpl::PointLightSystem(Entity ent, const PointLightComponent& comp)
{
    PointLight pl;
    pl.active = true;
    pl.position = ent.Get<TransformComponent>().translation;
    pl.ambient = comp.ambient * comp.color;
    pl.diffuse = comp.diffuse * comp.color;
    pl.specular = comp.specular * comp.color;
    pl.constant = comp.constant;
    pl.linear = comp.linear;
    pl.quadratic = comp.quadratic;
    pointLights_.push_back(pl);
}

void EngineImpl::RenderableSystem(Entity ent, Renderable& renderable, const glm::mat4& lightSpaceMatrix)
{
    RenderCommand command;
    command.p_shader_ = renderable.shader_;
    command.p_mesh_ = &renderable.mesh_;
    command.p_material_ = &renderable.material_;

    command.p_shader_->SetUniform({ "view", m_perspectiveCam.GetViewMatrix() });
    command.p_shader_->SetUniform({ "projection", m_perspectiveCam.GetProjectionMatrix() });
    command.p_shader_->SetUniform({ "viewPos", m_perspectiveCam.Position() });
    command.p_shader_->SetUniform({ "model", ent.Get<TransformComponent>() });
    command.p_shader_->SetUniform({ "lightSpaceMatrix", lightSpaceMatrix });
    command.textures_.emplace_back("shadowMap", &shadow_tex_);

    Renderer::Draw(command);
}

} // namespace PEANUT
