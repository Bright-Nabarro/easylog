function(add_clang_tidy target)
	find_program(CLANG-TIDY_PATH clang-tidy REQUIRED)
	set(clang-tidy_args ${CLANG-TIDY_PATH} -checks=*
		-llvmlibc-implementation-in-namespace)
	set_target_properties(${target}
		PROPERTIES CXX_CLANG_TIDY ${clang-tidy_args}
	)
endfunction()

