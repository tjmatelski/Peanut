set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)

# add_library(test-plugin SHARED TestPlugin.cpp)
# target_link_libraries(test-plugin PRIVATE pn-scene pn-core)

# add_library(terrain-plugin SHARED Terrain.cpp)
# target_link_libraries(terrain-plugin PRIVATE pn-scene pn-core)

add_library(asteroids-plugin SHARED Asteroids.cpp)
target_link_libraries(asteroids-plugin PRIVATE peanut)
