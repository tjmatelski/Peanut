add_library(peanut)
target_include_directories(peanut PUBLIC ../include)

target_sources(peanut PRIVATE
    Engine.cpp
    EngineImpl.cpp
    WindowInput.cpp
    FileSelectorDialog.cpp
    PluginManager.cpp
    Entity.cpp
    NativeScript.cpp
    Scene.cpp
    SceneSerializer.cpp
    Settings.cpp
    stb_image_impl.cpp
    Window.cpp
)

find_package(EnTT CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(PkgConfig)
find_package(pybind11 REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
pkg_check_modules(GTKMM_LIB REQUIRED IMPORTED_TARGET gtkmm-3.0)
target_link_libraries(peanut
    PUBLIC
        EnTT::EnTT
        glm::glm
        spdlog::spdlog
    PRIVATE
        glfw
        glad::glad
        PkgConfig::GTKMM_LIB
        pybind11::embed
        yaml-cpp
)

target_compile_options(peanut PRIVATE -Wall -Wextra -Werror)

add_subdirectory(Renderer)
