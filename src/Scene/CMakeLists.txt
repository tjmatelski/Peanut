add_library(pn-scene
    src/Entity.cpp
    src/Scene.cpp
    src/SceneSerializer.cpp
)

target_include_directories(pn-scene PUBLIC include)

find_package(EnTT CONFIG REQUIRED)
find_package(yaml-cpp CONFIG REQUIRED)
target_link_libraries(pn-scene
    PUBLIC
        EnTT::EnTT
        pn-utils
    PRIVATE
        yaml-cpp
)