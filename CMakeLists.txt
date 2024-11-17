cmake_minimum_required(VERSION 3.20)

set(is_main_project FALSE)
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	set(is_main_project TRUE)
endif()

if(is_main_project)
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
	
	add_subdirectory("test")

endif()

add_subdirectory("src")

if(is_main_project)
	add_custom_target(cp_compile_commands ALL
		COMMAND ${CMAKE_COMMAND} -E copy_if_different
				${CMAKE_BINARY_DIR}/compile_commands.json
				${CMAKE_SOURCE_DIR}/compile_commands.json
		DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
	)
endif()


