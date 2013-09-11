message(STATUS "In subdirectory tests:")

include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")
include_directories("${PROJECT_SOURCE_DIR}/libcbf/cbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/cbf/")

add_executable(cbf_test
  cbf_test.cc
  )
target_link_libraries(cbf_test
  ${CBF_LIBRARY_NAME}
  ${CBF_LIBS}
  cbf_schemas
  )  
set_target_properties(cbf_test
  PROPERTIES VERSION ${CBF_VERSION_MAJOR}
  SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  )
add_test(cbf_test ${PROJECT_BINARY_DIR}/tests/cbf_test cbf_test)

#if(CBF_HAVE_XSD)
#  message(STATUS "  adding executable: cbf_test_xsd_output")
#
#  add_executable(cbf_test_xsd_output 
#    cbf_test_xsd_output.cc
#    )
#  target_link_libraries(cbf_test_xsd_output
#    ${CBF_LIBRARY_NAME}
#    ${CBF_LIBS}
#    cbf_schemas
#    ${Boost_LIBRARIES}
#    )
#  set_target_properties(cbf_test_xsd_output
#    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
#    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR})
#endif()
