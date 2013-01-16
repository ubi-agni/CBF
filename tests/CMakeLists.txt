message(STATUS "In subdirectory tests:")


include_directories(${PROJECT_SOURCE_DIR}/libcbf/ ${PROJECT_BINARY_DIR}/libcbf/)

if(CBF_HAVE_CPPAD)
  include_directories(${CPPAD_PATH})
endif()

if(CBF_HAVE_XCF)
  include_directories(${XCF_INCLUDE_DIRS})
endif()

if(CBF_HAVE_XSD)
  include_directories(${XSD_INC})
endif()

if(CBF_HAVE_QT)
  include_directories(${QT_INCLUDE_DIRS})
  include_directories(${PROJECT_SOURCE_DIR}/tests)
endif()

if(CBF_HAVE_MEMORY)
  include_directories(${MEMORY_INCLUDE_DIRS})
endif()

if (CBF_HAVE_EIGEN2)
  include_directories(${EIGEN2_INCLUDE_DIRS})
endif()

message(STATUS "  adding executable: cbf_test_functional")
add_executable(cbf_test_functional
  cbf_test_functional.cc
  )
target_link_libraries(cbf_test_functional
  ${CBF_LIBRARY_NAME}
  )  
set_target_properties(cbf_test_functional
  PROPERTIES VERSION ${CBF_VERSION_MAJOR}
  SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  )
add_test(cbf_test_functional ${PROJECT_BINARY_DIR}/tests/cbf_test_functional cbf_test_functional)


if(CBF_HAVE_CPPAD)
  message(STATUS "  adding executable: cbf_test_cppad")
  add_executable(cbf_test_cppad
    cbf_test_cppad.cc
    )
  target_link_libraries(cbf_test_cppad
    ${CBF_VERSIONED_NAME}
    )
  set_target_properties(cbf_test_cppad
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )
else()
  message(STATUS "  not adding executable: cbf_test_cppad because cppad was not found.")
endif()


if(CBF_HAVE_MEMORY AND CBF_HAVE_XCF AND CBF_HAVE_XMLTIO AND CBF_HAVE_XSD)
  message(STATUS "  adding executable: cbf_test_xcf_reference")
  message(STATUS "  adding executable: cbf_test_xcf_reference_client")
  message(STATUS "  adding executable: cbf_test_xcf_memory_reference")
  message(STATUS "  adding executable: cbf_test_xcf_memory_reference_client")
  message(STATUS "  adding executable: cbf_test_xcf_memory_resource_adder")

  add_executable(cbf_test_xcf_reference
    cbf_test_xcf_reference.cc
    )
  target_link_libraries(cbf_test_xcf_reference
    ${CBF_LIBRARY_NAME}
    )

  add_executable(cbf_test_xcf_reference_client
    cbf_test_xcf_reference_client.cc
    )
  target_link_libraries(cbf_test_xcf_reference_client
    ${CBF_LIBRARY_NAME}
    )

  add_executable(cbf_test_xcf_memory_reference
    cbf_test_xcf_memory_reference.cc
    )
  target_link_libraries(cbf_test_xcf_memory_reference
    ${CBF_LIBRARY_NAME}
    )

  add_executable(cbf_test_xcf_memory_reference_client
    cbf_test_xcf_memory_reference_client.cc
    )
  target_link_libraries(cbf_test_xcf_memory_reference_client
    ${CBF_LIBRARY_NAME}
    )

  add_executable(cbf_test_xcf_memory_resource_adder
    cbf_test_xcf_memory_resource_adder.cc
    )
  target_link_libraries(cbf_test_xcf_memory_resource_adder
    ${CBF_LIBRARY_NAME}
    )

  set_target_properties(
    cbf_test_xcf_reference
    cbf_test_xcf_reference_client
    cbf_test_xcf_memory_reference
    cbf_test_xcf_memory_reference_client
    cbf_test_xcf_memory_resource_adder
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )

else()
  message(STATUS "  not adding executables:")
  message(STATUS "  -cbf_test_xcf_reference")
  message(STATUS "  -cbf_test_xcf_reference_client")
  message(STATUS "  -cbf_test_xcf_memory_reference")
  message(STATUS "  -cbf_test_xcf_memory_reference_client")
  message(STATUS "  -cbf_test_xcf_memory_resource_adder")
  message(STATUS "  because memory, xcf, xmltio or xsd was not found")
endif()


if(CBF_HAVE_XCF AND CBF_HAVE_XSD AND CBF_HAVE_QT)
  message(STATUS "  adding executable: cbf_q_xcf_vector_reference_client")

  QT4_WRAP_CPP(MOC_SOURCES_1 cbf_q_xcf_vector_reference_client.h)

  add_executable(cbf_q_xcf_vector_reference_client
    cbf_q_xcf_vector_reference_client.cc ${MOC_SOURCES_1}
    )
  target_link_libraries(cbf_q_xcf_vector_reference_client
    ${QT_LIBRARIES}
    cbf
    )
  set_target_properties(cbf_q_xcf_vector_reference_client  
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )

  else()
    message(STATUS "  not adding executable: cbf_q_xcf_vector_reference_client")
    message(STATUS "  because xcf, xsd or qt was not found")
endif()


if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS AND CBF_HAVE_MEMORY AND CBF_HAVE_QT)
  message(STATUS "  adding executable: cbf_q_xcf_memory_run_controller_operator")

  QT4_WRAP_CPP(MOC_SOURCES_2 cbf_q_xcf_memory_run_controller_operator.h)

  add_executable(cbf_q_xcf_memory_run_controller_operator 
    cbf_q_xcf_memory_run_controller_operator.cc ${MOC_SOURCES_2}
    )
  target_link_libraries(cbf_q_xcf_memory_run_controller_operator
    ${CBF_LIBRARY_NAME}
    ${Boost_LIBRARIES}
    ${MEMORY_LDFLAGS}
    ${QT_LIBRARIES}
    )
  set_target_properties(cbf_q_xcf_memory_run_controller_operator
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR})

else()
  message(STATUS "  not adding executable ")
  message(STATUS "  -xcf_memory_controller_publisher")
  message(STATUS "   because xsd, boost-program-options, memory or qt was not found")
endif()




