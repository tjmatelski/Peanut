#pragma once

#include "Window.h"
#include "Event.h"

#include <memory>

int main(int argc, char **argv);

namespace PEANUT
{
    class Application
    {
    public:
        Application();
        virtual void OnAttach() = 0;
        virtual void OnUpdate() = 0;
        virtual void OnRemove() = 0;

    private:
        std::unique_ptr<Window> m_window;
        bool m_shouldWindowClose;

    private:
        friend int ::main(int argc, char **argv);
        void Run();
        void UpdateWindow();
        void OnEvent(Event& event);
        void OnWindowClose(WindowCloseEvent& e);
    };

    /**
     * Implement this function on the client side to return the custom
     * application to run.
     * @return Application to run the engine with.
     */
    extern Application *GetApplication();
} // namespace PEANUT