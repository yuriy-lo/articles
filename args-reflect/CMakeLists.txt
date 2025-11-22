cmake_minimum_required(VERSION 3.16)
project(args_reflect VERSION 0.1 LANGUAGES C CXX)

# Always enable P2996 "reflection-latest" support for this project. This
# project assumes you are using a clang build that implements P2996 (e.g.
# clang-p2996). We force C++26 and add the compiler flags needed to enable
# the experimental reflection features.
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add compile options to the target enabling the unified experimental set.
add_compile_options(-freflection-latest)
add_compile_options(-std=c++26)

add_executable(args_reflect src/main.cpp)

target_include_directories(args_reflect PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

## Note: flags are added project-wide via add_compile_options() above. If you
## prefer target-local flags, switch these to target_compile_options() instead.

install(TARGETS args_reflect RUNTIME DESTINATION bin)
