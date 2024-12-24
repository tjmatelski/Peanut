cmake_minimum_required(VERSION 3.15)

target_sources(peanut PRIVATE
    BufferLayout.cpp
    FrameBuffer.cpp
    GLDebug.cpp
    IndexBuffer.cpp
    Mesh.cpp
    Model.cpp
    ModelLibrary.cpp
    OrthoCamera.cpp
    PerspectiveCamera.cpp
    Renderer.cpp
    Renderer2D.cpp
    Shader.cpp
    ShaderLibrary.cpp
    Texture.cpp
    TextureLibrary.cpp
    VertexArray.cpp
    VertexBuffer.cpp
)

find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(peanut PRIVATE
    OpenGL::GL
    glm::glm
    assimp::assimp
)
