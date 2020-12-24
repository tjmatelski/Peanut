#pragma once

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
};

}