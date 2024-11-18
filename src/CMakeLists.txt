add_library(easylog INTERFACE)

target_include_directories(easylog INTERFACE
	"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/easylog>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/easylog>"
)

if(is_main_project)
	include(Doxygen)
	Doxygen("${CMAKE_CURRENT_SOURCE_DIR}/easylog" "${CMAKE_SOURCE_DIR}/docs")
	
	include(Install)
endif()
