add_executable(Application
    Application.cpp
    SceneHierarchyPanel.cpp
    PropertyPanel.cpp
    RenderStatsPanel.cpp
    ViewportPanel.cpp
    ${CMAKE_BINARY_DIR}/imports/imgui_impl_glfw.cpp
    ${CMAKE_BINARY_DIR}/imports/imgui_impl_opengl3.cpp
    )

find_package(imgui CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
target_link_libraries(Application
    PRIVATE
        peanut
        dl
        imgui::imgui
        glfw
)
target_include_directories(Application PRIVATE ${CMAKE_BINARY_DIR}/imports)
target_compile_options(Application PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_features(Application PRIVATE cxx_std_17)
add_dependencies(Application copy-res)

add_custom_target(copy-res ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/res ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/res
)