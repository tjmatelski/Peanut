#include "EngineImpl.hpp"
#include <peanut/Input.hpp>

// external
#include <GLFW/glfw3.h>

namespace PEANUT {

bool Input::IsKeyPressed(const KeyCode key)
{
    auto* window = static_cast<GLFWwindow*>(EngineImpl::Get().GetWindow().GetRawWindow());
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

}
