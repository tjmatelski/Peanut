add_library(Peanut Window.cpp
    Shader.cpp
    GLDebug.cpp
    VertexBuffer.cpp
    IndexBuffer.cpp
    BufferLayout.cpp)

target_include_directories(Peanut PUBLIC ../include)
find_package(OpenGL REQUIRED)
target_link_libraries(Peanut PUBLIC glfw OpenGL::GL glad spdlog)