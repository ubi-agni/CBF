message(STATUS "In subdirectory apps:")

include_directories("${PROJECT_BINARY_DIR}/libcbf/")
include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_SOURCE_DIR}/apps/")

if(CBF_HAVE_XSD)
  include_directories(${XSD_INC})
endif()

if(CBF_HAVE_QT)
  include_directories(${QT_INCLUDES})
endif()

if(CBF_HAVE_MEMORY)
  include_directories(${MEMORY_INCLUDE_DIRS})
endif()


set(exe cbf_run_controller)
if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS)
  message(STATUS "  adding executable: ${exe}")
  add_executable(${exe} ${exe}_from_xml.cc)
  target_link_libraries(${exe} 
    ${CBF_LIBRARY_NAME}
    ${Boost_LIBRARIES}
    )
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})

  install(TARGETS 
	 RUNTIME DESTINATION bin
	 PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
	 )
else()
  message(STATUS "  not adding executable ${exe} because xsd or boost-program-options was not found")
endif()


set(exe cbf_xcf_memory_run_controller)
if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS AND CBF_HAVE_MEMORY)
  message(STATUS "  adding executable: ${exe}")
  add_executable(${exe} 
	 cbf_run_controller_from_xcf_memory.cc
    cbf_run_controller.cc
    xcf_memory_run_controller.cc
    )
  target_link_libraries(${exe}
    ${CBF_LIBRARY_NAME}
    ${Boost_LIBRARIES}
    ${MEMORY_LDFLAGS}
    )
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})

  install(TARGETS ${exe}
	 RUNTIME DESTINATION bin
	 PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
	 )
else()
  message(STATUS "  not adding executable ${exe}")
  message(STATUS "  because xsd, boost-program-options or memory was not found")
endif()

add_subdirectory(qcbf)
