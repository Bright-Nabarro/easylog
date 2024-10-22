# shell test output
set(dir ${CMAKE_CURRENT_SOURCE_DIR})

include(ShellTest)

ShellTest(logwrap logwrapper/main.cpp ${dir}/logwrapper)

ShellTest(debug_disable debug_disable/main.cpp ${dir}/debug_disable)
target_compile_options(debug_disable PRIVATE
	-O2
	-DNDEBUG
)

ShellTest(all_disable all_disable/main.cpp ${dir}/all_disable)
target_compile_options(all_disable PRIVATE
	-DNO_YQ_LOG
)

