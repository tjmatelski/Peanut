#pragma once

#include "Panel.hpp"
#include "SceneHierarchyPanel.hpp"

class PropertyPanel : public Panel {
public:
    PropertyPanel(PEANUT::Engine* engine, const PEANUT::SceneHierarchyPanel* sh_panel)
        : Panel("Properties", engine)
        , m_sh_panel(sh_panel)
    {
    }

    void Update() override;

private:
    template <typename Component>
    void DrawComponent(const std::string& componentName);
    template <typename Component>
    void DrawComponentSpecifics();
    void DrawCustomComponents();

    const PEANUT::SceneHierarchyPanel* m_sh_panel = nullptr;
};
