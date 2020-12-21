#include <Application.h>
#include <Input.h>
#include <GLFW/glfw3.h>

namespace PEANUT
{

bool Input::IsKeyPressed(const KeyCode key)
{
    GLFWwindow* window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetRawWindow());
    return glfwGetKey(window, static_cast<int>(key)) == GLFW_PRESS;
}

}