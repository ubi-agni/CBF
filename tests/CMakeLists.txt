message(STATUS "In subdirectory tests:")

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
endif()

if(CBF_HAVE_MEMORY)
  include_directories(${MEMORY_INCLUDE_DIRS})
endif()


set(exe cbf_test_functional)
message(STATUS "  adding executable: ${exe}")
add_executable(${exe} ${exe}.cc)
target_link_libraries(${exe} ${CBF_LIBRARY_NAME})
add_dependencies(${exe} ${CBF_LIBRARY_NAME})
add_test(${exe} ${PROJECT_BINARY_DIR}/tests/${exe} ${exe})


set(exe cbf_test_cppad)
if(CBF_HAVE_CPPAD)
  message(STATUS "  adding executable: ${exe}")
  add_executable(${exe} ${exe}.cc)
  target_link_libraries(${exe} ${CBF_LIBRARY_NAME})
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})
else()
  message(STATUS "  not adding executable: ${exe} because cppad was not found.")
endif()


if(CBF_HAVE_MEMORY AND CBF_HAVE_XCF AND CBF_HAVE_XMLTIO AND CBF_HAVE_XSD)
  foreach (exe 
		cbf_test_xcf_reference 
		cbf_test_xcf_reference_client
		cbf_test_xcf_memory_reference
		cbf_test_xcf_memory_reference_client
		cbf_test_xcf_memory_resource_adder)
	 message(STATUS "  adding executable: ${exe}")
	 add_executable(${exe} ${exe}.cc)
	 target_link_libraries(${exe} ${CBF_LIBRARY_NAME})
	 add_dependencies(${exe} ${CBF_LIBRARY_NAME})
  endforeach(exe)
else()
  message(STATUS "  not adding XCF test executables")
  message(STATUS "  because memory, xcf, xmltio or xsd was not found")
endif()


set(exe cbf_q_xcf_vector_reference_client)
if(CBF_HAVE_XCF AND CBF_HAVE_XSD AND CBF_HAVE_QT)
  message(STATUS "  adding executable: ${exe}")
  QT4_WRAP_CPP(MOC_${exe} ${exe}.h)
  add_executable(${exe} ${exe}.cc ${MOC_${exe}})
  target_link_libraries(${exe} ${CBF_LIBRARY_NAME} ${QT_LIBRARIES})
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})
else()
    message(STATUS "  not adding executable: ${exe}")
    message(STATUS "  because xcf, xsd or qt was not found")
endif()

set(exe cbf_q_xcf_memory_run_controller_operator)
if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS AND CBF_HAVE_MEMORY AND CBF_HAVE_QT)
  message(STATUS "  adding executable: ${exe}")
  QT4_WRAP_CPP(MOC_${exe} ${exe}.h)
  add_executable(${exe} ${exe}.cc ${MOC_${exe}})
  target_link_libraries(${exe} 
	 ${CBF_LIBRARY_NAME} 
	 ${QT_LIBRARIES} 
	 ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${QT_LIBRARIES}
	 )
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})
else()
  message(STATUS "  not adding executable ${exe}")
  message(STATUS "  because xsd, boost-program-options, memory or qt was not found")
endif()

set(exe cbf_test_7dof_kdl_chain)
if(CBF_HAVE_KDL)
  message(STATUS "  adding executable: ${exe}")
  include_directories(SYSTEM ${KDL_INCLUDE_DIRS})
  add_executable(${exe} ${exe}.cpp)
  target_link_libraries(${exe} 
	${CBF_LIBRARY_NAME}
    ${Boost_PROGRAM_OPTIONS_LIBRARIES}
    ${KDL_LDFLAGS}
	 )
  add_dependencies(${exe} ${CBF_LIBRARY_NAME})
else()
  message(STATUS "  not adding executable ${exe}")
  message(STATUS "  because kdl was not found")
endif()
