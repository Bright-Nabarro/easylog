# use test.sh as test script
function(ShellTest target_name source_file output_dir)
	add_executable(${target_name} ${source_file})
	target_link_libraries(${target_name} PRIVATE
		easylog
	)
	set_target_properties(${target_name} PROPERTIES
		OUTPUT_NAME "a.out"
		RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
	)
	add_test(NAME ${target_name}
		COMMAND ./test.sh
		WORKING_DIRECTORY "${output_dir}"
	)
endfunction()

