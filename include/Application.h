#pragma once

#include "Window.h"
#include "Events/KeyEvent.h"
#include "Events/WindowEvents.h"

#include <memory>

int main(int argc, char **argv);

namespace PEANUT
{
    class Application
    {
    public:
        Application();
        virtual void OnAttach() = 0;
        virtual void OnImGuiUpdate() {}
        virtual void OnUpdate() = 0;
        virtual void OnRemove() = 0;

    private:
        std::unique_ptr<Window> m_window;
        bool m_shouldWindowClose;

    private:
        friend int ::main(int argc, char **argv);
        void ImGuiBeginFrame();
        void ImGuiEndFrame();
        void Run();
        void OnEvent(Event& event);
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