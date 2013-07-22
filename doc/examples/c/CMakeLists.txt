message(STATUS "In subdirectory doc/examples/c:")

if(CBF_HAVE_KDL AND CBF_HAVE_XSD) # no c api
if(CBF_HAVE_XSD AND CBF_HAVE_EIGEN2)
  include_directories("${PROJECT_SOURCE_DIR}/libcbf")
  #include_directories(${XSD_INC})

  message(STATUS "  adding executable cbf_test_c_bindings")
  add_executable(cbf_test_c_bindings
    cbf_test_c_bindings.c
    )

  target_link_libraries(cbf_test_c_bindings
    ${CBF_LIBRARY_NAME}
    )

else()
  message(STATUS "  not adding executable cbf_test_c_bindings because XSD or Eigen2 was not found")
endif()

else(CBF_HAVE_KDL AND CBF_HAVE_XSD)
  message(STATUS "  not adding executable cbf_test_c_bindings because XSD or KDL was not found")
endif(CBF_HAVE_KDL AND CBF_HAVE_XSD)