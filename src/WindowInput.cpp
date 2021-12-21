#include <Application.h>
#include <GLFW/glfw3.h>
#include <Input.h>

namespace PEANUT {

bool Input::IsKeyPressed(const KeyCode key)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetRawWindow());
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

}