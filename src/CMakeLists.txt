add_library(Peanut SHARED
    Application.cpp
    ScriptLibrary.cpp
    Settings.cpp
    stb_image_impl.cpp
    Window.cpp
    WindowInput.cpp
    # OpenGL Renderer
    OpenGLRenderer/Shader.cpp
    OpenGLRenderer/GLDebug.cpp
    OpenGLRenderer/VertexBuffer.cpp
    OpenGLRenderer/IndexBuffer.cpp
    OpenGLRenderer/BufferLayout.cpp
    OpenGLRenderer/VertexArray.cpp
    OpenGLRenderer/Renderer.cpp
    OpenGLRenderer/Renderer2D.cpp
    OpenGLRenderer/Texture.cpp
    OpenGLRenderer/TextureLibrary.cpp
    OpenGLRenderer/OrthoCamera.cpp
    # Scene
    Scene/Scene.cpp
    Scene/Entity.cpp
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
    PRIVATE 
        yaml-cpp
        ${GTKMM_LIBRARIES})
target_compile_features(Peanut PUBLIC cxx_std_17)
target_compile_options(Peanut PUBLIC -fPIC PRIVATE -Wall -Wextra -Wpedantic -Werror)

