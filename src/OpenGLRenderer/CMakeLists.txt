cmake_minimum_required(VERSION 3.15)

add_library(pn-renderer SHARED
    src/BufferLayout.cpp
    src/FrameBuffer.cpp
    src/GLDebug.cpp
    src/IndexBuffer.cpp
    src/Mesh.cpp
    src/Model.cpp
    src/ModelLibrary.cpp
    src/OrthoCamera.cpp
    src/PerspectiveCamera.cpp
    src/Renderer.cpp
    src/Renderer2D.cpp
    src/Shader.cpp
    src/Texture.cpp
    src/TextureLibrary.cpp
    src/VertexArray.cpp
    src/VertexBuffer.cpp
)

target_include_directories(pn-renderer PUBLIC include)

find_package(OpenGL REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(pn-renderer PRIVATE
    pn-utils
    pn-scene
    glfw
    OpenGL::GL
    glad::glad
    glm::glm
    assimp::assimp
)
