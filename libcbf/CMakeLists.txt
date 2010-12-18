message(STATUS "In subdirectory libcbf:")

include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")

set(CBF_SOURCES
  xml_factory.cc
  object.cc
  sensor_transform.cc 
  composite_transform.cc
  control_basis.cc
  controller.cc 
  composite_reference.cc
  masking_resource.cc
  controller_sequence.cc
  identity_transform.cc 
  primitive_controller.cc 
  resource.cc 
  dummy_resource.cc 
  effector_transform.cc 
  potential.cc 
  utilities.cc
  combination_strategy.cc
  axis_angle_potential.cc
  axis_potential.cc
  composite_potential.cc
  square_potential.cc
  linear_transform.cc 
  composite_resource.cc
  reference.cc
  dummy_reference.cc
  gnuplot_i.cpp
  quaternion.cc
  difference_sensor_transform.cc
  weighted_sum_transforms.cc
  task_space_plan.cc
  norm_transform.cc
  transpose_transform.cc
  functional.cc
  svd.cc
  convergence_criterion.cc
  generic_transform.cc
  )
 
set(CBF_LIBS "")

set(CBF_INCLUDES "")
set(CBF_LINKDIRS "")

if(CBF_HAVE_XSD)
  link_directories("${PROJECT_BINARY_DIR}/schemas")
  set(CBF_INCLUDES ${CBF_INCLUDES} "${PROJECT_BIN_DIR}/libcbf" ${XSD_INC})
  set(CBF_LIBS ${CBF_LIBS} cbf_schemas ${XERCESC_LIB})
else()
  message(STATUS "  excluding plugin_pool.cc because xsd was not found")
endif()

if(CBF_HAVE_CPPAD)
  set(CBF_SOURCES ${CBF_SOURCES} cppad_sensor_transform.cc)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${CPPAD_PATH})
endif()


if(CBF_HAVE_KDL)
  set(CBF_SOURCES ${CBF_SOURCES} kdl_transforms.cc)
  if(CBF_HAVE_XSD)
    set(CBF_SOURCES ${CBF_SOURCES} c_api.cc)
  endif()
  set(CBF_LIBS ${CBF_LIBS} ${KDL_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${KDL_INCLUDE_DIRS})
  set(CBF_INCLUDES ${CBF_INCLUDES} "${PROJECT_BINARY_DIR}/libcbf/cbf/")
else()
  message(STATUS "  excluding kdl_transforms.cc and c_api.cc because KDL was not found")
endif()

if (CBF_HAVE_SPACEMOUSE)
  set(CBF_SOURCES ${CBF_SOURCES} spacenavi_reference.cc)
else()
  message(STATUS "  excluding spacenavi_reference.cc because spacemouse was not found")
endif()

if (CBF_HAVE_MEMORY)
  set(CBF_SOURCES ${CBF_SOURCES} memory_resource.cc)
  set(CBF_LIBS ${CBF_LIBS} ${MEMORY_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${MEMORY_INCLUDE_DIRS})
else()
  message(STATUS "  excluding memory_ressource.cc because memory was not found")
endif()

if (CBF_HAVE_XCF)
  set(CBF_LIBS ${CBF_LIBS} ${XCF_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${XCF_INCLUDE_DIRS}) 

  if (CBF_HAVE_XSD)
    set(CBF_SOURCES ${CBF_SOURCES} xcf_vector_reference.cc)
  else()
    message(STATUS "  excluding xcf_vector_reference.cc because xsd was not found")
  endif()

  if (CBF_HAVE_XMLTIO)
    set(CBF_SOURCES ${CBF_SOURCES} xcf_resource.cc)
    set(CBF_LIBS ${CBF_LIBS} ${XMLTIO_LDFLAGS})
    set(CBF_INCLUDES ${CBF_INCLUDES} ${XMLTIO_INCLUDE_DIRS})

    if(CBF_HAVE_MEMORY)
      set(CBF_SOURCES ${CBF_SOURCES} xcf_memory_sensor_transform.cc xcf_memory_reference.cc)
      set(CBF_LIBS ${CBF_LIBS} ${MEMORY_LDFLAGS})
      set(CBF_INCLUDES ${CBF_INCLUDES} ${MEMORY_INCLUDE_DIRS})
    else()
      message(STATUS "  excluding xcf_memoy_sensor_transform.cc and xcf_memory_reference.cc because memory was not found")
    endif() 

  else()
    message(STATUS "  excluding xcf_resource.cc and xcf_memoy_sensor_transform.cc because xmltio was not found")
  endif()

else()
  message(STATUS "  excluding xcf_vector_reference.cc, xcf_memory_sensor_transform and xcf_ressource.cc because xcf was not found")
endif()

if (CBF_HAVE_QT)
  set(CBF_SOURCES ${CBF_SOURCES} qt_reference.cc)
  set(CBF_SOURCES ${CBF_SOURCES} qt_sensor_transform.cc)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${QT_INCLUDES})
  set(CBF_LIBS ${CBF_LIBS} ${QT_LIBRARIES})
endif()

if (CBF_HAVE_XRI)
  set(CBF_SOURCES ${CBF_SOURCES} robotinterface_resource.cc)
  set(CBF_LIBS ${CBF_LIBS} ${XRI_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${XRI_INCLUDE_DIRS})
else()
  message(STATUS "  excluding roboterinterface_ressource.cc because xri was not found")
endif()

if (CBF_HAVE_EIGEN2)
 set(CBF_INCLUDES ${CBF_INCLUDES} ${EIGEN2_INC})
else()
message(STATUS "  excluding generic_transform.cc because eigen2 was not found")
endif()



include_directories(${CBF_INCLUDES})

add_library(cbf SHARED
  ${CBF_SOURCES}
  )

target_link_libraries(cbf ${CBF_LIBS})

SET_TARGET_PROPERTIES(cbf PROPERTIES VERSION ${CBF_VERSION_MAJOR} SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR})

install(TARGETS cbf
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
  )

