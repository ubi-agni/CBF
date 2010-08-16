message(STATUS "In subdirectory doc/examples/cpp:")

if(CBF_HAVE_KDL AND CBF_HAVE_EIGEN2)
  include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
  include_directories("${PROJECT_BINARY_DIR}/libcbf/")
  include_directories(AFTER ${KDL_INCLUDE_DIRS})

  message(STATUS "  adding executable cbf_test_kdl_transforms")
  add_executable(cbf_test_kdl_transforms
    cbf_test_kdl_transforms.cc
    )

  target_link_libraries(cbf_test_kdl_transforms
    cbf
    )

  message(STATUS "  adding executable cbf_simple_test")
  add_executable(cbf_simple_test
    cbf_simple_test.cc
    )

  target_link_libraries(cbf_simple_test
    cbf
    )

  message(STATUS "  adding executable cbf_test_axis_potential")
  add_executable(cbf_test_axis_potential
    cbf_test_axis_potential.cc
    )

  target_link_libraries(cbf_test_axis_potential
    cbf
    )

  message(STATUS "  adding executable cbf_test_axis_potential2")
  add_executable(cbf_test_axis_potential2
    cbf_test_axis_potential2.cc
    )

  target_link_libraries(cbf_test_axis_potential2
    cbf
    )

  if(${CBF_HAVE_KDLCHAINVIZ})
    message(STATUS "adding executable cbf_test_kdl_transforms_view")
    add_executable(cbf_test_kdl_transforms_view
      cbf_test_kdl_transforms_view.cc
      )

    target_link_libraries(
      cbf
      KDLCHAINVIZ
      QKDLCHAINVIZ
      QTCORE
      QTGUI
      QTOPENGL
      OROCOS-KDL
      )

    if(${HAVE_SPACEMOUSE})
      target_link_libraries(cbf_test_kdl_transforms_view
        SPACEMOUSE
        )
    else()
      message(STATUS "  linking cbf_test_kdl_transforms without Spacemouse")
    endif()
  else()
    message(STATUS "  not adding executable cbf_test_kdl_transforms_view because KDLCHAINVIZ was not found")
  endif()
else()
  message(STATUS "  not adding executables:")
  message(STATUS "  -cbf_test_kdl_transforms")
  message(STATUS "  -cbf_simple_test")
  message(STATUS "  -cbf_test_axis_potential")
  message(STATUS "  -cbf_test_axis_potential2")
  message(STATUS "  -cbf_test_kdl_transforms_view")
  message(STATUS "  because Kdl or Eigen2 was not found")
endif()
