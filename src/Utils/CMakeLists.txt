add_library(pn-utils SHARED src/Settings.cpp src/stb_image_impl.cpp)

target_include_directories(pn-utils PUBLIC include)

find_package(spdlog CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
target_link_libraries(pn-utils
    PUBLIC
        spdlog::spdlog
        glm::glm
    PRIVATE
        yaml-cpp)