file(GLOB SRC "*.cpp")

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

option(static_check OFF)

if (static_check)
	include(ClangTidy)
	add_clang_tidy(log_test)
endif()

include(Coverage)
set_coverage_build(log_test)
add_coverage(log_test)

#include(GoogleTest)
#gtest_discover_tests(log_test)
add_test(NAME unit_test COMMAND log_test)

