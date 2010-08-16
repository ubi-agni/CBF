message(STATUS "In subdirectory tests:")

if(CBF_HAVE_XCF)
  include_directories("${PROJECT_SOURCE_DIR}/libcbf/"  ${XCF_INCLUDE_DIRS} ${XSD_INC})
  include_directories("${PROJECT_BINARY_DIR}/libcbf")


  message(STATUS "  adding executable: cbf_test_xcf_reference")
  add_executable(cbf_test_xcf_reference
	 cbf_test_xcf_reference.cc
	 )

  target_link_libraries(cbf_test_xcf_reference
	 cbf
	 )
else()
  message(STATUS "  not adding executable cbf_test_xcf_reference because xcf was not found")
endif()

