project(imgui)

add_library(imgui 
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_widgets.cpp
    imgui/imgui_tables.cpp
    imgui/backends/imgui_impl_glfw.cpp
    imgui/backends/imgui_impl_opengl3.cpp)

target_include_directories(imgui PUBLIC imgui imgui/backends)
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PRIVATE glfw OpenGL::GL glad)
target_compile_definitions(imgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLAD)
