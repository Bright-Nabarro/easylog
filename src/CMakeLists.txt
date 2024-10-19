add_library(easylog INTERFACE)

target_include_directories(easylog INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

include(Doxygen)
Doxygen(${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_SOURCE_DIR}/docs")

