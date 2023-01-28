add_library(Peanut SHARED
    Application.cpp
    Settings.cpp
    stb_image_impl.cpp
    Window.cpp
    WindowInput.cpp
    # OpenGL Renderer
    OpenGLRenderer/BufferLayout.cpp
    OpenGLRenderer/FrameBuffer.cpp
    OpenGLRenderer/GLDebug.cpp
    OpenGLRenderer/IndexBuffer.cpp
    OpenGLRenderer/Mesh.cpp
    OpenGLRenderer/Model.cpp
    OpenGLRenderer/ModelLibrary.cpp
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
    Platform/Linux/GtkFileSelectorDialog.cpp
)

find_package(OpenGL REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(EnTT CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Lua CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)

find_package(PkgConfig)
pkg_check_modules(GTKMM_LIB REQUIRED IMPORTED_TARGET gtkmm-3.0)

target_include_directories(Peanut
    PUBLIC ../include
    PRIVATE ${LUA_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS_DEBUG}
)
target_link_libraries(Peanut
    PUBLIC
        glfw
        OpenGL::GL
        glad::glad
        spdlog::spdlog
        EnTT::EnTT
        glm::glm
    PRIVATE 
        yaml-cpp
        assimp::assimp
        PkgConfig::GTKMM_LIB
        sol2::sol2
        ${LUA_LIBRARIES}
)
target_compile_features(Peanut PUBLIC cxx_std_17)
target_compile_options(Peanut PUBLIC -fPIC PRIVATE -Wall -Wextra -Wpedantic -Werror)
