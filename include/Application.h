#pragma once

#include "Events/KeyEvent.h"
#include "Events/WindowEvents.h"
#include "TimeStep.h"
#include "Window.h"

#include <memory>

int main(int argc, char **argv);

namespace PEANUT
{
    class Application
    {
    public:
        Application();
        virtual void OnAttach() = 0;
        virtual void OnEvent(Event& event) {}
        virtual void OnImGuiUpdate() {}
        virtual void OnUpdate(TimeStep timeStep) = 0;
        virtual void OnRemove() = 0;

        static inline const Application& Get() { return *s_application; }
        inline const Window& GetWindow() const { return *m_window; }

    private:
        static Application* s_application;
        float m_lastFrameTime = 0.0f;
        bool m_shouldWindowClose;
        std::unique_ptr<Window> m_window;

    private:
        friend int ::main(int argc, char **argv);
        void ImGuiBeginFrame();
        void ImGuiEndFrame();
        void Run();
        void OnApplicationEvent(Event& event);
        void OnKeyEvent(KeyEvent& e);
        void OnWindowClose(WindowCloseEvent& e);
        void OnWindowResize(WindowResizeEvent& e);
        void UpdateWindow();
    };

    /**
     * Implement this function on the client side to return the custom
     * application to run.
     * @return Application to run the engine with.
     */
    extern Application *GetApplication();
} // namespace PEANUT