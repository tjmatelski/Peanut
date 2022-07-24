add_executable(Application
    Application.cpp
    SceneHierarchyPanel.cpp
    PropertyPanel.cpp
    ViewportPanel.cpp
    )

target_link_libraries(Application PRIVATE Peanut dl imgui)
target_compile_options(Application PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_features(Application PRIVATE cxx_std_17)
