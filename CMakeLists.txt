cmake_minimum_required(VERSION 3.0.0)
project(cobra-map-editor VERSION 0.1.0)

# Get a specific version of wzmaplib (from the warzone2100 repo, currently)
include(FetchContent)
message(STATUS "maptools: Fetching wzmaplib (from warzone2100 repo - this may take a while)")
FetchContent_Declare(
	warzone2100_repo
	GIT_REPOSITORY https://github.com/Warzone2100/warzone2100.git
	GIT_TAG        9584ad4bf442bde738b700c8d9bb44bce7a9cf9d		# pre-4.3.0
	GIT_PROGRESS   TRUE
	USES_TERMINAL_DOWNLOAD	TRUE
)
FetchContent_GetProperties(warzone2100_repo)
if(NOT warzone2100_repo_POPULATED)
	FetchContent_Populate(warzone2100_repo)
endif()
message(STATUS "cobra-map-editor: Adding wzmaplib")
add_subdirectory("${warzone2100_repo_SOURCE_DIR}/lib/wzmaplib" "lib/wzmaplib" EXCLUDE_FROM_ALL)

set(OpenGL_GL_PREFERENCE LEGACY)
add_subdirectory(lib/raylib)

file(GLOB srcfiles "src/*.cpp" "src/*.c" "src/*.hpp" "src/*.h"
		"lib/imgui/*.cpp" "lib/imgui/*.h"
		"lib/rlImGui/*.h" "lib/rlImGui/*.cpp" 
		"lib/rlFPCamera/*.h" "lib/rlFPCamera/rlFPCamera.cpp")

add_executable(cobra-map-editor ${srcfiles})
include_directories(src/ lib/imgui/ lib/rlImGui/ lib/rlFPCamera/ ${RAYLIB_INCLUDE_DIR})
target_link_libraries(cobra-map-editor raylib wzmaplib)
if (TARGET ZipIOProvider)
	target_link_libraries(cobra-map-editor PRIVATE ZipIOProvider)
else()
	message(FATAL_ERROR "ZipIOProvider is not available - please ensure libzip is installed.")
endif()

add_custom_command( TARGET cobra-map-editor PRE_BUILD
					COMMAND ${CMAKE_COMMAND} -E copy_directory
					${CMAKE_SOURCE_DIR}/data/ $<TARGET_FILE_DIR:cobra-map-editor>/data/)
