include_directories(${PROJECT_SOURCE_DIR}/libcbf/ ${PROJECT_BINARY_DIR}/libcbf/)

if(CBF_HAVE_XCF)
	include_directories(${XCF_INCLUDE_DIRS})
endif()

if(CBF_HAVE_XSD)
	include_directories(${XSD_INC})
endif()


if(CBF_HAVE_XCF AND CBF_HAVE_XSD)
  add_executable(cbf_test_xcf_reference
	 cbf_test_xcf_reference.cc
	 )

  target_link_libraries(cbf_test_xcf_reference
	 cbf
	 )
endif()

add_executable(cbf_test_functional
  cbf_test_functional.cc
  )

target_link_libraries(cbf_test_functional
  cbf
  )
