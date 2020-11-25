add_library(Peanut 
    Application.cpp
    Window.cpp
    stb_image_impl.cpp
# OpenGL Renderer
    OpenGLRenderer/Shader.cpp
    OpenGLRenderer/GLDebug.cpp
    OpenGLRenderer/VertexBuffer.cpp
    OpenGLRenderer/IndexBuffer.cpp
    OpenGLRenderer/BufferLayout.cpp
    OpenGLRenderer/VertexArray.cpp
    OpenGLRenderer/Renderer.cpp
    OpenGLRenderer/Texture.cpp)

target_include_directories(Peanut PUBLIC ../include)
find_package(OpenGL REQUIRED)
target_link_libraries(Peanut PUBLIC glfw OpenGL::GL glad spdlog imgui)