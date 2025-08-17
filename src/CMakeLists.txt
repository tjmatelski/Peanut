add_library(peanut)
target_include_directories(peanut PUBLIC ../include)

target_sources(peanut PRIVATE
    Engine.cpp
    EngineImpl.cpp
    EngineImplSystems.cpp
    Entity.cpp
    FileSelectorDialog.cpp
    Main.cpp
    NativeScript.cpp
    PluginManager.cpp
    PythonBindings.cpp
    Scene.cpp
    SceneSerializer.cpp
    Settings.cpp
    stb_image_impl.cpp
    Window.cpp
    WindowInput.cpp
)

include(FindPkgConfig)
pkg_check_modules(GTKMM_LIB REQUIRED IMPORTED_TARGET gtkmm-3.0)
target_link_libraries(peanut
    PUBLIC
        EnTT::EnTT
        glm::glm
        spdlog::spdlog
    PRIVATE
        glfw
        glad
        PkgConfig::GTKMM_LIB
        pybind11::embed
        yaml-cpp
)

target_compile_definitions(peanut PRIVATE STBI_NO_SIMD)
target_compile_options(peanut PRIVATE -Wall -Wextra -Werror)
target_compile_features(peanut PUBLIC cxx_std_20)
target_precompile_headers(peanut PUBLIC "../include/peanut/Math.hpp")
target_precompile_headers(peanut PUBLIC "../include/peanut/Log.hpp")
target_precompile_headers(peanut PRIVATE "Pybind11.hpp")

add_subdirectory(Renderer)
