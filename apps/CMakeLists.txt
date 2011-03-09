message(STATUS "In subdirectory apps:")

include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_SOURCE_DIR}/apps/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")

if(CBF_HAVE_XSD)
  include_directories(${XSD_INC})
endif()

if(CBF_HAVE_QT)
  include_directories(${QT_INCLUDES})
endif()

if(CBF_HAVE_MEMORY)
  include_directories(${MEMORY_INCLUDE_DIRS})
endif()


if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS)
  message(STATUS "  adding executable: cbf_run_controller")

  add_executable(cbf_run_controller 
    cbf_run_controller_from_xml.cc
    )
  target_link_libraries(cbf_run_controller
    cbf
    ${Boost_LIBRARIES}
    )
  set_target_properties(cbf_run_controller
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR})

else()
  message(STATUS "  not adding executable cbf_run_controller because xsd or boost-program-options was not found")
endif()


if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS AND CBF_HAVE_MEMORY)
  message(STATUS "  adding executable: cbf_xcf_memory_run_controller")

  add_executable(cbf_xcf_memory_run_controller 
    cbf_run_controller_from_xcf_memory.cc
    cbf_run_controller.cc
    xcf_memory_run_controller.cc
    )
  target_link_libraries(cbf_xcf_memory_run_controller
    cbf
    ${Boost_LIBRARIES}
    ${MEMORY_LDFLAGS}
    )
  set_target_properties(cbf_xcf_memory_run_controller
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR})

else()
  message(STATUS "  not adding executable ")
  message(STATUS "  -cbf_xcf_memory_run_controller")
  message(STATUS "   because xsd, boost-program-options or memory was not found")
endif()

add_subdirectory(qcbf)
