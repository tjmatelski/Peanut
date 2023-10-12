add_library(pn-core
    src/Engine.cpp
    src/Window.cpp
    src/WindowInput.cpp
    src/FileSelectorDialog.cpp
)

target_include_directories(pn-core PUBLIC include)

find_package(glfw3 CONFIG REQUIRED)
find_package(glad CONFIG REQUIRED)
find_package(sol2 CONFIG REQUIRED)
find_package(pybind11 REQUIRED)
find_package(PkgConfig)
pkg_check_modules(GTKMM_LIB REQUIRED IMPORTED_TARGET gtkmm-3.0)
target_link_libraries(pn-core
    PRIVATE
        pn-utils
        glfw
        pn-scene
        glad::glad
        pn-renderer
        sol2::sol2
        PkgConfig::GTKMM_LIB
        pybind11::embed
)