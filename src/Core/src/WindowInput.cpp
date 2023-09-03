#include <Engine.hpp>
#include <GLFW/glfw3.h>
#include <Input/Input.h>

namespace PEANUT {

bool Input::IsKeyPressed(const KeyCode key)
{
    auto* window = static_cast<GLFWwindow*>(Engine::Get().GetWindow().GetRawWindow());
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

}