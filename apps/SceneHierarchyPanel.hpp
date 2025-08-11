#pragma once

#include "Panel.hpp"
#include <peanut/Entity.hpp>
#include <peanut/Scene.hpp>

// stl

namespace PEANUT {

class SceneHierarchyPanel : public Panel {
public:
    using Panel::Panel;
    SceneHierarchyPanel(class Engine* p_engine)
        : Panel("Scene", p_engine)
    {
    }
    void Update() override;
    [[nodiscard]] Entity GetSelectedEntity() const { return m_selectedEntity; }

private:
    Entity m_selectedEntity;
};

}