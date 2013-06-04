message(STATUS "In subdirectory doc/examples/cpp:")

include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")

if(CBF_HAVE_XSD)
  include_directories(${XSD_INC})
endif()

if(CBF_HAVE_MEMORY)
  include_directories(${MEMORY_INCLUDE_DIRS})
endif()

if(CBF_HAVE_KDL)
  include_directories(AFTER ${KDL_INCLUDE_DIRS})
endif()


message(STATUS "  adding executable cbf_simple_test")
add_executable(cbf_simple_test
  cbf_simple_test.cc
  )

target_link_libraries(cbf_simple_test
  ${CBF_LIBRARY_NAME}
  )

SET_TARGET_PROPERTIES(cbf_simple_test 
  PROPERTIES VERSION ${CBF_VERSION_MAJOR} 
  SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  )

if(CBF_HAVE_MEMORY AND CBF_HAVE_XSD)
  message(STATUS "  adding executable cbf_simple_test_memory")

  add_executable(cbf_simple_test_memory
    cbf_simple_test_memory.cc
    )

  target_link_libraries(cbf_simple_test_memory
    ${CBF_LIBRARY_NAME} ${MEMORY_LDFLAGS}
    )

  SET_TARGET_PROPERTIES(cbf_simple_test_memory
    PROPERTIES VERSION ${CBF_VERSION_MAJOR}
    SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
    )

else()
  message(STATUS "  not adding executable cbf_simple_test_memory")
  message(STATUS "  because Memory was not found")
endif()



if(CBF_HAVE_KDL)
 message(STATUS "  adding executable cbf_test_kdl_transforms")

 add_executable(cbf_test_kdl_transforms
   cbf_test_kdl_transforms.cc
   )

 target_link_libraries(cbf_test_kdl_transforms
   ${CBF_LIBRARY_NAME}
   )

 SET_TARGET_PROPERTIES(cbf_test_kdl_transforms
   PROPERTIES VERSION ${CBF_VERSION_MAJOR}
   SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
   )


 message(STATUS "  adding executable cbf_test_axis_potential")
 add_executable(cbf_test_axis_potential
   cbf_test_axis_potential.cc
   )

 target_link_libraries(cbf_test_axis_potential
   ${CBF_LIBRARY_NAME}
   )

 SET_TARGET_PROPERTIES(cbf_test_axis_potential
   PROPERTIES VERSION ${CBF_VERSION_MAJOR}
   SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
   )

 message(STATUS "  adding executable cbf_test_axis_potential2")
 add_executable(cbf_test_axis_potential2
   cbf_test_axis_potential2.cc
   )

 target_link_libraries(cbf_test_axis_potential2
   ${CBF_LIBRARY_NAME}
   )

 SET_TARGET_PROPERTIES(cbf_test_axis_potential2
   PROPERTIES VERSION ${CBF_VERSION_MAJOR}
   SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
   )


#  if(${CBF_HAVE_KDLCHAINVIEW})
#    message(STATUS "adding executable cbf_test_kdl_transforms_view")
#
#    add_executable(cbf_test_kdl_transforms_view
#      cbf_test_kdl_transforms_view.cc
#      )
#	
#    include_directories(${KDLCHAINVIEW_INCLUDES} ${QT_INCLUDES})
#
#    target_link_libraries(cbf_test_kdl_transforms_view
#      ${KDLCHAINVIEW_LIBS}
#      ${QT_LIBS}
#      )
#
#    if(${HAVE_SPACEMOUSE})
#      include_directories(${SPACEMOUSE_INCLUDES})
#
#      target_link_libraries(cbf_test_kdl_transforms_view
#        ${SPACEMOUSE_LIBS}
#        )
#
#    else()
#      message(STATUS "  linking cbf_test_kdl_transforms without Spacemouse")
#    endif()
#
#    SET_TARGET_PROPERTIES(cbf_test_kdl_transforms_view
#      PROPERTIES VERSION ${CBF_VERSION_MAJOR}
#      SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR})
#
#  else()
#    message(STATUS "  not adding executable cbf_test_kdl_transforms_view because KDLCHAINVIZ was not found")
#  endif()

else()
 message(STATUS "  not adding executables:")
 message(STATUS "  -cbf_test_kdl_transforms")
 message(STATUS "  -cbf_test_kdl_transforms_view")
 message(STATUS "  because Kdl or Eigen2 was not found")
endif()


