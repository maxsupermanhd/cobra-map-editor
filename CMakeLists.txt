cmake_minimum_required(VERSION 3.0.0)
project(cobra-map-editor VERSION 0.1.0)

set(OpenGL_GL_PREFERENCE LEGACY)
add_subdirectory(lib/raylib)

file(GLOB srcfiles "src/*.cpp" "src/*.c" "src/*.hpp" "src/*.h"
		"lib/imgui/*.cpp" "lib/imgui/*.h"
		"lib/rlImGui/*.h" "lib/rlImGui/*.cpp" 
		"lib/rlFPCamera/*.h" "lib/rlFPCamera/rlFPCamera.cpp")

add_executable(cobra-map-editor ${srcfiles})
include_directories(src/ lib/imgui/ lib/rlImGui/ lib/rlFPCamera/ ${RAYLIB_INCLUDE_DIR})
target_link_libraries(cobra-map-editor raylib)

add_custom_command( TARGET cobra-map-editor PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:cobra-map-editor>/data/)