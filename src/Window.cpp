#include <Window.h>
#include <Log.h>
#include <Events/KeyEvent.h>
#include <Events/WindowEvents.h>
#include <Events/MouseEvents.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace PEANUT
{

    Window::Window(const char *title, const int width, const int height) : m_window(nullptr), m_width(width), 
        m_height(height), m_eventCallback()
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
        m_window = window;
        if (window == NULL)
        {
            const char *message;
            int code = glfwGetError(&message);
            LOG_ERROR("Failed to create GLFW Window. CODE: {0} MESSAGE: {1}", code, message);
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            LOG_ERROR("Failed to initialize GLAD");
        }

        glfwSetWindowUserPointer(window, this);

        InitWindowCallbacks();

        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        ImGui::StyleColorsDark();

        // Setup Platform/Renderer backends
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        const char *glsl_version = "#version 130";
        ImGui_ImplOpenGL3_Init(glsl_version);
    }

    Window::~Window()
    {
        // Cleanup
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwTerminate();
    }

    bool Window::WindowShouldClose() const
    {
        return glfwWindowShouldClose(static_cast<GLFWwindow *>(m_window));
    }

    void Window::SwapBuffers()
    {
        glfwSwapBuffers(static_cast<GLFWwindow *>(m_window));
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
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window_, int width, int height) {
            LOG_INFO("GLFW Framebuffer resize, width = {0}, height = {1}", width, height);
            WindowResizeEvent event(width, height);
            Window *myWindow = static_cast<Window *>(glfwGetWindowUserPointer(window_));
            myWindow->m_width = width;
            myWindow->m_height = height;
            myWindow->m_eventCallback(event);
        });

        glfwSetErrorCallback([](int code, const char *message) {
            LOG_ERROR("GLFW ERROR CODE: {0} MESSAGE: {1}", code, message);
        });

        glfwSetKeyCallback(window, [](GLFWwindow *window_, int key, int scancode, int action, int mods) {
            LOG_INFO("Key Event | Key: {0} | Scancode: {1} | action: {2} | Mods: {3}", key, scancode, action, mods);
            KeyEvent event(static_cast<KeyCode>(key));
            Window *myWindow = static_cast<Window *>(glfwGetWindowUserPointer(window_));
            myWindow->m_eventCallback(event);
        });

        glfwSetWindowCloseCallback(window, [](GLFWwindow *window_) {
            LOG_INFO("GLFW Window close callback");
            WindowCloseEvent windowCloseEvent;
            Window *myWindow = static_cast<Window *>(glfwGetWindowUserPointer(window_));
            myWindow->m_eventCallback(windowCloseEvent);
        });

        glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset){
            ScrollEvent event(xoffset, yoffset);
            Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            myWindow->m_eventCallback(event);
        });

        glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
            MouseMovedEvent event(xpos, ypos);
            Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            myWindow->m_eventCallback(event);
        });

        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
            MouseButtonEvent event(static_cast<MouseCode>(button), action == GLFW_PRESS);
            Window* myWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));
            myWindow->m_eventCallback(event);
        });
    }

} // namespace PEANUT