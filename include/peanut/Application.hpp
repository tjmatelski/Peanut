#pragma once

#include <peanut/Event.hpp>
#include <peanut/KeyEvent.hpp>
#include <peanut/MouseEvents.hpp>
#include <peanut/WindowEvents.hpp>

namespace PEANUT {
class Engine;

class Application {
public:
    virtual ~Application() { }
    virtual void OnAttach() { }
    virtual void OnEvent([[maybe_unused]] Event& event) { }
    virtual void OnPreUpdate() { }
    virtual void OnUpdate([[maybe_unused]] double timeStep) { }
    virtual void OnPostUpdate() { }
    virtual void OnRemove() { }

    Engine* m_engine = nullptr;
};

/**
 * Implement this function on the client side to return the custom
 * application to run.
 * @return Application to run the engine with.
 */
extern Application* GetApplication();
} // namespace PEANUT