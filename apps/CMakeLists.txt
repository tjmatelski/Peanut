add_executable(Application Application.cpp SceneHierarchyPanel.cpp)

target_link_libraries(Application PRIVATE Peanut dl imgui)
target_compile_options(Application PRIVATE -Wall -Wextra -Werror)