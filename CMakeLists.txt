cmake_minimum_required(VERSION 3.0.0)
project(raylib-test VERSION 0.1.0)

set(OpenGL_GL_PREFERENCE LEGACY)
add_subdirectory(lib/raylib)

add_executable(raylib-test src/main.cpp)
include_directories(${RAYLIB_INCLUDE_DIR})
target_link_libraries(raylib-test raylib)

