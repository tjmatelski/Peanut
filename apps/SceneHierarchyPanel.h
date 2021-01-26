#pragma once

#include "FileSelectionPanel.h"

#include <Peanut.h>
#include <functional>
#include <memory>

namespace PEANUT
{

class SceneHierarchyPanel
{
public:
    SceneHierarchyPanel(std::shared_ptr<Scene> scene);
    void UpdateGui();

private:
    std::shared_ptr<Scene> m_scene;
    Entity m_selectedEntity;
    FileSelectionPanel m_spriteRenderFileSelector;
    bool m_showSpriteRenderFileSelector = false;

    void UpdateMenuBar();
    void UpdatePropertiesPanel();
    template <typename Component>
    void DrawComponent(const std::string& componentName);
    template <typename Component>
    void DrawComponentSpecifics();
};

template <>
void SceneHierarchyPanel::DrawComponent<TagComponent>(const std::string &componentName);
template <>
void SceneHierarchyPanel::DrawComponent<TransformComponent>(const std::string &componentName);

}