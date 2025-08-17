cmake_minimum_required(VERSION 3.15)

target_sources(peanut PRIVATE
    BufferLayout.cpp
    FrameBuffer.cpp
    GLDebug.cpp
    IndexBuffer.cpp
    Material.cpp
    Mesh.cpp
    Model.cpp
    ModelLibrary.cpp
    OrthoCamera.cpp
    PerspectiveCamera.cpp
    RenderBuffer.cpp
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

target_link_libraries(peanut PRIVATE
    OpenGL::GL
    glm::glm
    assimp::assimp
)
