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



message(STATUS "  adding executable: cbf_test_functional")
add_executable(cbf_test_functional
  cbf_test_functional.cc
  )

target_link_libraries(cbf_test_functional
  cbf
  )  

SET_TARGET_PROPERTIES(cbf_test_functional
  PROPERTIES VERSION ${CBF_VERSION_MAJOR}
  SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  )



if(CBF_HAVE_CPPAD)
  message(STATUS "  adding executable: cbf_test_cppad")

  add_executable(cbf_test_cppad
    cbf_test_cppad.cc
    )

  target_link_libraries(cbf_test_cppad
    cbf
    )

  SET_TARGET_PROPERTIES(cbf_test_cppad
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )

else()
  message(STATUS "  not adding executable: cbf_test_cppad because cppad was not found.")
endif()



if(CBF_HAVE_XCF AND CBF_HAVE_XSD)
  message(STATUS "  adding executable: cbf_test_xcf_reference")
  message(STATUS "  adding executable: cbf_test_xcf_reference_client")
  message(STATUS "  adding executable: cbf_test_xcf_memory_reference")
  message(STATUS "  adding executable: cbf_test_xcf_memory_reference_client")

  add_executable(cbf_test_xcf_reference
    cbf_test_xcf_reference.cc
    )

  target_link_libraries(cbf_test_xcf_reference
    cbf
    )

  SET_TARGET_PROPERTIES(cbf_test_xcf_reference 
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )


  add_executable(cbf_test_xcf_reference_client
    cbf_test_xcf_reference_client.cc
    )

  target_link_libraries(cbf_test_xcf_reference_client
    cbf
    )

  SET_TARGET_PROPERTIES(cbf_test_xcf_reference_client
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )


  add_executable(cbf_test_xcf_memory_reference
    cbf_test_xcf_memory_reference.cc
    )

  target_link_libraries(cbf_test_xcf_memory_reference
    cbf
    )

  SET_TARGET_PROPERTIES(cbf_test_xcf_memory_reference
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )


  add_executable(cbf_test_xcf_memory_reference_client
    cbf_test_xcf_memory_reference_client.cc
    )

  target_link_libraries(cbf_test_xcf_memory_reference_client
    cbf
    )

  SET_TARGET_PROPERTIES(cbf_test_xcf_memory_reference_client
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )


else()
  message(STATUS "  not adding executables:")
  message(STATUS "  -cbf_test_xcf_reference")
  message(STATUS "  -cbf_test_xcf_reference_client")
  message(STATUS "  -cbf_test_xcf_memory_reference")
  message(STATUS "  -cbf_test_xcf_memory_reference_client")
  message(STATUS "  because xcf or xsd was not found")
endif()



if(CBF_HAVE_XCF AND CBF_HAVE_XSD AND CBF_HAVE_QT)
  message(STATUS "  adding executable: cbf_q_xcf_vector_reference_client")

  QT4_WRAP_CPP(MOC_SOURCES cbf_q_xcf_vector_reference_client.h)

  add_executable(cbf_q_xcf_vector_reference_client
    cbf_q_xcf_vector_reference_client.cc ${MOC_SOURCES}
    )

  target_link_libraries(cbf_q_xcf_vector_reference_client
    ${QT_LIBRARIES}
    cbf
    )

  SET_TARGET_PROPERTIES(cbf_q_xcf_vector_reference_client  
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )

  else()
    message(STATUS "  not adding executable: cbf_q_xcf_vector_reference_client")
    message(STATUS "  because xcf, xsd or qt was not found")
endif()

