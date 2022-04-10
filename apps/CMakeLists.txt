add_executable(Application Application.cpp SceneHierarchyPanel.cpp)

target_link_libraries(Application PRIVATE Peanut dl imgui)
target_compile_options(Application PRIVATE -Wall -Wextra -Wpedantic -Werror)
target_compile_features(Application PRIVATE cxx_std_17)

add_custom_command(TARGET Application PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/res/ $<TARGET_FILE_DIR:Application>/res/)

add_custom_command(TARGET Application POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/include/ $<TARGET_FILE_DIR:Application>/include/)

add_custom_command(TARGET Application POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/extern/entt/single_include/entt/ $<TARGET_FILE_DIR:Application>/include/entt/)

add_custom_command(TARGET Application POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/extern/glm/glm/ $<TARGET_FILE_DIR:Application>/include/glm/)
