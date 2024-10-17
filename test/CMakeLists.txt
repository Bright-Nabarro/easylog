file(GLOB SRC "*.cpp")

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
	message(FATAL_ERROR "gtest not found")
endif()

add_executable(log_test ${SRC})

target_link_libraries(log_test PRIVATE
	easylog
	GTest::gmock
	GTest::gtest
)

set_target_properties(log_test PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)

target_compile_options(log_test PRIVATE
	-DUNIT_TEST
)

add_test(NAME LogTest COMMAND log_test)


#include(GoogleTest)
#gtest_discover_tests(log_test)


