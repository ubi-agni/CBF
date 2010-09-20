pkg_check_modules(QTGUI QtGui)
if(QTGUI_FOUND)
  find_package(Qt4)
  include(FindQt4)
  include_directories(${QTGUI_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR})
  include_directories(${XCF_INCLUDE_DIRS})

  QT4_WRAP_CPP(MOC_SOURCES test_xcf_reference_client_gui.h)
  add_executable(test_gui
	main.cc ${MOC_SOURCES} test_xcf_reference_client_gui.cc
	)

  target_link_libraries(test_gui
	${QT_LIBRARIES}
	cbf
	)
endif()
