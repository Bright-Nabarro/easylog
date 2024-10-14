file(GLOB SRC "*.cpp")

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
	message(FATAL_ERROR "gtest not found")
endif()

add_executable(log_test ${SRC})

target_link_libraries(log_test
	easylog
	gtest
)

set_target_properties(log_test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

enable_testing()

add_test(NAME log_test COMMAND bin/log_test)
