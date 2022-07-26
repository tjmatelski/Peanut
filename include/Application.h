#pragma once

#include "Events/KeyEvent.h"
#include "Events/WindowEvents.h"
#include "Scene/Scene.h"
#include "TimeStep.h"
#include "Window.h"

#include <memory>

int main(int argc, char** argv);

namespace PEANUT {
class Application {
public:
    Application();
    virtual void OnAttach() = 0;
    virtual void OnEvent(Event& event) = 0;
    virtual void OnPreUpdate() { }
    virtual void OnUpdate(TimeStep timeStep) = 0;
    virtual void OnPostUpdate() { }
    virtual void OnRemove() = 0;

    static inline const Application& Get() { return *s_application; }
    [[nodiscard]] inline const Window& GetWindow() const { return *m_window; }

protected:
    void Terminate();
    bool m_runtime = false;
    std::shared_ptr<Scene> m_scene;

private:
    static Application* s_application;
    float m_lastFrameTime = 0.0f;
    bool m_shouldWindowClose;
    std::unique_ptr<Window> m_window;

    friend int ::main(int argc, char** argv);
    void Run();
    void OnApplicationEvent(Event& event);
    void UpdateWindow();
    void UpdateLuaScripts(TimeStep ts);
};

/**
 * Implement this function on the client side to return the custom
 * application to run.
 * @return Application to run the engine with.
 */
extern Application* GetApplication();
} // namespace PEANUT