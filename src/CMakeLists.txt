file(GLOB SRC "*.cpp")

add_library(easylog ${SRC} INTERFACE)

target_include_directories(easylog INTERFACE
	"${CMAKE_CURRENT_SOURCE_DIR}"
)

