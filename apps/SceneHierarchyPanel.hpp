#pragma once

#include "Panel.hpp"
#include <peanut/Entity.hpp>
#include <peanut/Scene.hpp>

// stl
#include <memory>

namespace PEANUT {

class SceneHierarchyPanel : public Panel {
public:
    using Panel::Panel;
    void Update() override;
    [[nodiscard]] Entity GetSelectedEntity() const { return m_selectedEntity; }

private:
    Entity m_selectedEntity;
};

}