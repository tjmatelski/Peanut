#pragma once

#include <peanut/Entity.hpp>
#include <peanut/Scene.hpp>

// stl
#include <memory>

namespace PEANUT {

class SceneHierarchyPanel {
public:
    SceneHierarchyPanel(std::shared_ptr<Scene> scene);
    void UpdateGui();
    [[nodiscard]] Entity GetSelectedEntity() const { return m_selectedEntity; }

private:
    std::shared_ptr<Scene> m_scene;
    Entity m_selectedEntity;
};

}