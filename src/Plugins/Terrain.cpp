#include "Input/Input.h"
#include "Scene/Component.h"
#include "Scene/Entity.h"
#include "Scene/NativeScript.h"
#include "Scene/Scene.h"
#include "Utils/Log.h"

#include <glm/fwd.hpp>

#include <algorithm>
#include <glm/geometric.hpp>
#include <iostream>
#include <linux/limits.h>
#include <memory>
#include <random>
#include <vector>

extern "C" const char* plugin_name()
{
    return "Terrain";
}

class Terrain : public PEANUT::NativeScript {
public:
    double width = 10.0;
    double height = 10.0;
    int width_nodes = 10;
    int height_nodes = 10;
    bool regenerate = false;

    Terrain()
    {
        m_members.emplace_back(PEANUT::MemberVariable { "width", PEANUT::MemberVariable::Type::Double, &width });
        m_members.emplace_back(PEANUT::MemberVariable { "height", PEANUT::MemberVariable::Type::Double, &height });
        m_members.emplace_back(PEANUT::MemberVariable { "width_nodes", PEANUT::MemberVariable::Type::Int, &width_nodes });
        m_members.emplace_back(PEANUT::MemberVariable { "height_nodes", PEANUT::MemberVariable::Type::Int, &height_nodes });
        m_members.emplace_back(PEANUT::MemberVariable { "regenerate", PEANUT::MemberVariable::Type::Bool, &regenerate });
    }

    Terrain(const Terrain& other) = delete;

    void OnCreate() override
    {
        Regenerate();
    }

    void EditorUpdate() override
    {
        if (regenerate) {
            Regenerate();
            regenerate = false;
        }
    }

private:
    std::vector<std::vector<double>> height_map;

    void Regenerate()
    {
        LOG_DEBUG("Generating terrain");

        using namespace PEANUT;
        auto& model = m_ent.Has<CustomModelComponent>() ? m_ent.Get<CustomModelComponent>() : m_ent.Add<CustomModelComponent>();
        model.mesh.indices.clear();
        model.mesh.vertices.clear();

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        height_map.clear();
        for (int i = 0; i < width_nodes; ++i) {
            height_map.emplace_back();
            for (int j = 0; j < height_nodes; ++j) {
                height_map[i].emplace_back(dis(gen));
            }
        }

        for (int i = 0; i < (width_nodes - 1); ++i) {
            for (int j = 0; j < (height_nodes - 1); ++j) {
                const glm::vec3 point_1 = {
                    static_cast<double>(i) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j) / static_cast<double>(height_nodes) * height,
                    height_map[i][j]
                };
                const glm::vec3 point_2 = {
                    static_cast<double>(i + 1) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j) / static_cast<double>(height_nodes) * height,
                    height_map[i + 1][j]
                };
                const glm::vec3 point_3 = {
                    static_cast<double>(i + 1) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j + 1) / static_cast<double>(height_nodes) * height,
                    height_map[i + 1][j + 1]
                };
                const glm::vec3 point_4 = {
                    static_cast<double>(i) / static_cast<double>(width_nodes) * width,
                    static_cast<double>(j + 1) / static_cast<double>(height_nodes) * height,
                    height_map[i][j + 1]
                };

                const auto normal_1 = glm::normalize(glm::cross(glm::vec3 { point_2 - point_1 }, glm::vec3 { point_3 - point_1 }));
                const auto normal_2 = glm::normalize(glm::cross(glm::vec3 { point_4 - point_3 }, glm::vec3 { point_1 - point_3 }));

                auto& mesh = m_ent.Get<PEANUT::CustomModelComponent>().mesh;
                mesh.vertices.emplace_back(PEANUT::Vertex { point_1, normal_1, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_2, normal_1, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_3, normal_1, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_3, normal_2, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_4, normal_2, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
                mesh.vertices.emplace_back(PEANUT::Vertex { point_1, normal_2, {} });
                mesh.indices.emplace_back(mesh.vertices.size() - 1);
            }
        }
    }
};

extern "C" PEANUT::NativeScript* get_new_component()
{
    LOG_DEBUG("Creating new TestComponent");
    return new Terrain {};
}
