#include <Window.h>
#include <Log.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// TODO: Remove this dependancy, probably once event system exists
#include "OpenGLRenderer/GLDebug.h"

Window::Window(const char* title, const int width, const int height)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, title, NULL, NULL);
    m_window = window;
    if (window == NULL)
    {
        const char* message;
        int code = glfwGetError(&message);
        LOG_ERROR("Failed to create GLFW Window. CODE: {0} MESSAGE: {1}", code, message);
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        LOG_ERROR("Failed to initialize GLAD");
    }

    glViewport(0, 0, 800, 600);

    glfwSetFramebufferSizeCallback(window, [](GLFWwindow*, int width, int height){
        GLCALL(glViewport(0, 0, width, height));
    });

    glfwSetErrorCallback([](int code, const char* message){
        LOG_ERROR("GLFW ERROR CODE: {0} MESSAGE: {1}", code, message);
    });

    glfwSetKeyCallback(window, [](GLFWwindow*, int key, int scancode, int action, int mods){
        LOG_INFO("Key Event | Key: {0} | Scancode: {1} | action: {2} | Mods: {3}", key, scancode, action, mods);
    });
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::WindowShouldClose() const
{
    return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window));
}

void Window::ProcessInput()
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window);
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
}

void Window::PollEvents()
{
    glfwPollEvents();
}