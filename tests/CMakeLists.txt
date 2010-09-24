message(STATUS "In subdirectory tests:")

include_directories(${PROJECT_SOURCE_DIR}/libcbf/ ${PROJECT_BINARY_DIR}/libcbf/)

if(CBF_HAVE_XCF)
  include_directories(${XCF_INCLUDE_DIRS})
endif()

if(CBF_HAVE_XSD)
  include_directories(${XSD_INC})
endif()

if(CBF_HAVE_XCF AND CBF_HAVE_XSD)
  message(STATUS "  adding executable: cbf_test_xcf_reference")
  message(STATUS "  adding executable: cbf_test_xcf_reference_client")

  add_executable(cbf_test_xcf_reference
    cbf_test_xcf_reference.cc
    )

  target_link_libraries(cbf_test_xcf_reference
    cbf
    )

  add_executable(cbf_test_xcf_reference_client
    cbf_test_xcf_reference_client.cc
    )

  target_link_libraries(cbf_test_xcf_reference_client
    cbf
    )

  if(CBF_HAVE_QT)
    message(STATUS "  adding executable: cbf_q_xcf_vector_reference_client")

    include_directories(${QT_INCLUDE_DIRS})
    include_directories(${PROJECT_SOURCE_DIR}/tests)

    QT4_WRAP_CPP(MOC_SOURCES cbf_q_xcf_vector_reference_client.h)
    add_executable(cbf_q_xcf_vector_reference_client
	  cbf_q_xcf_vector_reference_client.cc ${MOC_SOURCES}
	  )

    target_link_libraries(cbf_q_xcf_vector_reference_client
	  ${QT_LIBRARIES}
	  cbf
	  )
  else()
    message(STATUS "  not adding executable: cbf_q_xcf_vector_reference_client")
  endif()
else()
  message(STATUS "  not adding executables:")
  message(STATUS "  -cbf_test_xcf_reference")
  message(STATUS "  -cbf_test_xcf_reference_client")
  message(STATUS "  -cbf_q_xcf_vector_reference_client")
  message(STATUS "  because xcf was not found")
endif()

message(STATUS "  adding executable: cbf_test_functional")
add_executable(cbf_test_functional
  cbf_test_functional.cc
  )

target_link_libraries(cbf_test_functional
  cbf
  )  

