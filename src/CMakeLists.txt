add_library(Peanut SHARED
    Application.cpp
    ScriptLibrary.cpp
    Settings.cpp
    stb_image_impl.cpp
    Window.cpp
    WindowInput.cpp
    # OpenGL Renderer
    OpenGLRenderer/BufferLayout.cpp
    OpenGLRenderer/GLDebug.cpp
    OpenGLRenderer/IndexBuffer.cpp
    OpenGLRenderer/Mesh.cpp
    OpenGLRenderer/Model.cpp
    OpenGLRenderer/OrthoCamera.cpp
    OpenGLRenderer/PerspectiveCamera.cpp
    OpenGLRenderer/Renderer.cpp
    OpenGLRenderer/Renderer2D.cpp
    OpenGLRenderer/Shader.cpp
    OpenGLRenderer/Texture.cpp
    OpenGLRenderer/TextureLibrary.cpp
    OpenGLRenderer/VertexArray.cpp
    OpenGLRenderer/VertexBuffer.cpp
    # Scene
    Scene/Entity.cpp
    Scene/Scene.cpp
    Scene/SceneSerializer.cpp
    # Platform Linux
    Platform/Linux/GtkFileSelectorDialog.cpp)

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

find_package(OpenGL REQUIRED)

target_include_directories(Peanut
    PUBLIC ../include
    PRIVATE ${GTKMM_INCLUDE_DIRS})
target_link_directories(Peanut PRIVATE ${GTKMM_LIBRARY_DIRS})
target_link_libraries(Peanut
    PUBLIC
        glfw
        OpenGL::GL
        glad
        spdlog
        EnTT
        glm
    PRIVATE 
        yaml-cpp
        assimp
        ${GTKMM_LIBRARIES})
target_compile_features(Peanut PUBLIC cxx_std_17)
target_compile_options(Peanut PUBLIC -fPIC PRIVATE -Wall -Wextra -Wpedantic -Werror)

