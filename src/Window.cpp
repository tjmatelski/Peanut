#include <Events/KeyEvent.h>
#include <Events/MouseEvents.h>
#include <Events/WindowEvents.h>
#include <Log.h>
#include <Window.h>

// clang-format off
#include <glad/glad.h>
#include <GLFW/glfw3.h>
// clang-format on

namespace PEANUT {

Window::Window(const char* title, const int width, const int height)
    : m_window(nullptr)
    , m_width(width)
    , m_height(height)
    , m_eventCallback()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL);
    m_window = window;
    if (window == NULL) {
        const char* message;
        int code = glfwGetError(&message);
        LOG_ERROR("Failed to create GLFW Window. CODE: {0} MESSAGE: {1}", code, message);
        glfwTerminate();
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        LOG_ERROR("Failed to initialize GLAD");
    }

    glfwSetWindowUserPointer(window, this);

    InitWindowCallbacks();
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::WindowShouldClose() const
{
    return glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window));
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(static_cast<GLFWwindow*>(m_window));
}

void Window::PollEvents()
{
    glfwPollEvents();
}

float Window::GetTime() const
{
    return static_cast<float>(glfwGetTime());
}

void Window::InitWindowCallbacks()
{
    GLFWwindow* window = static_cast<GLFWwindow*>(m_window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window_, int width, int height) { // NOLINT(bugprone-easily-swappable-parameters): Callback has to match glfw API
        WindowResizeEvent event(width, height);
        Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window_));
        myWindow->m_width = width;
        myWindow->m_height = height;
        myWindow->m_eventCallback(event);
    });

    glfwSetErrorCallback([](int code, const char* message) {
        LOG_ERROR("GLFW ERROR CODE: {0} MESSAGE: {1}", code, message); // NOLINT(bugprone-lambda-function-name)
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window_, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) { // NOLINT(bugprone-easily-swappable-parameters): Callback has to match glfw API
        KeyEvent event(static_cast<KeyCode>(key));
        Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window_));
        myWindow->m_eventCallback(event);
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window_) {
        WindowCloseEvent windowCloseEvent;
        Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window_));
        myWindow->m_eventCallback(windowCloseEvent);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ScrollEvent event(xoffset, yoffset);
        Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myWindow->m_eventCallback(event);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        MouseMovedEvent event(xpos, ypos);
        Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myWindow->m_eventCallback(event);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) { // NOLINT(bugprone-easily-swappable-parameters): Callback has to match glfw API
        MouseButtonEvent event(static_cast<MouseCode>(button), action == GLFW_PRESS);
        Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myWindow->m_eventCallback(event);
    });
}

} // namespace PEANUT