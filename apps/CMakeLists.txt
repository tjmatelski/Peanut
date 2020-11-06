add_executable(Application Application.cpp)

find_package(OpenGL REQUIRED)
target_link_libraries(Application PRIVATE glfw OpenGL::GL glad spdlog)