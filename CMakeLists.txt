cmake_minimum_required(VERSION 3.20)
project(easylog LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

add_compile_options(
	-Wall
	-Wextra
	-Wuninitialized
)

enable_testing()

add_subdirectory("src")
add_subdirectory("test")

add_custom_target(cp_compile_commands ALL
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
			${CMAKE_BINARY_DIR}/compile_commands.json
			${CMAKE_SOURCE_DIR}/compile_commands.json
	DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
)


