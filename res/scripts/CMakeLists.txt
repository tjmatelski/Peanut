add_library(TestScript SHARED TestScript.cpp)
target_link_libraries(TestScript PRIVATE Peanut)
target_compile_options(TestScript PRIVATE -fPIC)