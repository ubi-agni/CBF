if(CBF_HAVE_XCF AND CBF_HAVE_XSD)
  include_directories(${PROJECT_SOURCE_DIR}/libcbf/  ${XCF_INCLUDE_DIRS} ${XSD_INC})

  add_executable(cbf_test_xcf_reference
	 cbf_test_xcf_reference.cc
	 )

  target_link_libraries(cbf_test_xcf_reference
	 cbf
	 )
endif()

