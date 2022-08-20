cmake_minimum_required(VERSION 3.0.0)
project(raylib-test VERSION 0.1.0)

set(OpenGL_GL_PREFERENCE LEGACY)
add_subdirectory(lib/raylib)

file(GLOB srcfiles "src/*.cpp" "src/*.c" "src/*.hpp" "src/*.h" "lib/imgui/*.cpp" "lib/imgui/*.h")

add_executable(raylib-test ${srcfiles})
include_directories(src/ lib/imgui/ ${RAYLIB_INCLUDE_DIR})
target_link_libraries(raylib-test raylib)

