#include <peanut/Window.hpp>

#include <peanut/KeyEvent.hpp>
#include <peanut/Log.hpp>
#include <peanut/MouseEvents.hpp>
#include <peanut/WindowEvents.hpp>

// external
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

    GLFWwindow* window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    m_window = window;
    if (window == nullptr) {
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
    auto* window = static_cast<GLFWwindow*>(m_window);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window_, int width, int height) { // NOLINT(bugprone-easily-swappable-parameters): Callback has to match glfw API
        WindowResizeEvent event(width, height);
        auto* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window_));
        myWindow->m_width = width;
        myWindow->m_height = height;
        myWindow->m_eventCallback(event);
    });

    glfwSetErrorCallback([](int code, const char* message) {
        LOG_ERROR("GLFW ERROR CODE: {0} MESSAGE: {1}", code, message);
    });

    glfwSetKeyCallback(window, [](GLFWwindow* window_, int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) { // NOLINT(bugprone-easily-swappable-parameters): Callback has to match glfw API
        KeyEvent event(static_cast<KeyCode>(key));
        auto* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window_));
        myWindow->m_eventCallback(event);
    });

    glfwSetWindowCloseCallback(window, [](GLFWwindow* window_) {
        WindowCloseEvent windowCloseEvent;
        auto* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window_));
        myWindow->m_eventCallback(windowCloseEvent);
    });

    glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
        ScrollEvent event(xoffset, yoffset);
        auto* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myWindow->m_eventCallback(event);
    });

    glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
        MouseMovedEvent event(xpos, ypos);
        auto* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myWindow->m_eventCallback(event);
    });

    glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, [[maybe_unused]] int mods) { // NOLINT(bugprone-easily-swappable-parameters): Callback has to match glfw API
        MouseButtonEvent event(static_cast<MouseCode>(button), action == GLFW_PRESS);
        auto* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
        myWindow->m_eventCallback(event);
    });
}

bool Window::IsKeyPressed(KeyCode key) const
{
    return glfwGetKey(static_cast<GLFWwindow*>(m_window), static_cast<int>(key)) == GLFW_PRESS;
}

bool Window::IsMousePressed(MouseCode button) const
{
    return glfwGetMouseButton(static_cast<GLFWwindow*>(m_window), static_cast<int>(button)) == GLFW_PRESS;
}

} // namespace PEANUT