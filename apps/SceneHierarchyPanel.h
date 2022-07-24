#pragma once

#include <Peanut.h>
#include <functional>
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

    void UpdateMenuBar();
};

}