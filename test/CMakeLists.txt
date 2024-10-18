file(GLOB SRC "unit_test/*.cpp")

find_package(GTest REQUIRED)
if (NOT GTest_FOUND)
	message(FATAL_ERROR "gtest not found")
endif()

# unit test
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
	-DYQ_EASYLOG_UNIT_TEST
)

#include(GoogleTest)
#gtest_discover_tests(log_test)
add_test(NAME unit_test COMMAND log_test)

# shell test output
set (SH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/shell")
## logwrapper
set(logwrap log_wrapper.out)
add_executable(${logwrap} "${SH_DIR}/logwrapper/main.cpp")
target_link_libraries(${logwrap} PRIVATE
	easylog
)
set_target_properties(${logwrap} PROPERTIES
	RUNTIME_OUTPUT_DIRECTORY "${SH_DIR}/logwrapper"
)

