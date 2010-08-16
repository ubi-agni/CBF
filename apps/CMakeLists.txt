message(STATUS "In subdirectory apps:")

if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS)
  message(STATUS "  adding executable: cbf_run_controller")
  include_directories("${PROJECT_SOURCE_DIR}/libcbf/" ${XSD_INC})
  include_directories("${PROJECT_BINARY_DIR}/libcbf/" ${XSD_INC})

  add_executable(cbf_run_controller 
    cbf_run_controller_from_xml.cc
    )

  target_link_libraries(cbf_run_controller
    cbf
    ${Boost_LIBRARIES}
    )
else()
  message(STATUS "  not adding executable cbf_run_controller because xsd or boost-program-options was not found")
endif()

add_subdirectory(qcbf)
