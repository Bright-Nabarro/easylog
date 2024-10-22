include(GNUInstallDirs)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/easylog"
	DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

install(TARGETS easylog
	EXPORT EasyLog
	INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/easylog
)

install(EXPORT EasyLog
	NAMESPACE easylog::
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/easylog/cmake
)

install(FILES "${CMAKE_SOURCE_DIR}/cmake/EasyLogConfig.cmake"
	DESTINATION ${CMAKE_INSTALL_LIBDIR}/easylog/cmake
)

