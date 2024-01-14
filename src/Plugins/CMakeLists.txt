add_library(test-plugin SHARED TestPlugin.cpp)
target_link_libraries(test-plugin PRIVATE pn-scene)