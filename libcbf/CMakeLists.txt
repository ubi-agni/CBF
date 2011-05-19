message(STATUS "In subdirectory libcbf:")

include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")

set(CBF_SOURCES
  namespace.cc
  xml_factory.cc
  xml_object_factory.cc
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
  object_list.cc
  )

set(CBF_HEADERS
  cbf/axis_angle_potential.h
  cbf/axis_potential.h
  cbf/c_api.h
  cbf/cbf.h
  cbf/combination_strategy.h
  cbf/common.h
  cbf/composite_potential.h
  cbf/composite_reference.h
  cbf/composite_resource.h
  cbf/composite_transform.h
  cbf/control_basis.h
  cbf/controller.h
  cbf/controller_sequence.h
  cbf/convergence_criterion.h
  cbf/cppad_sensor_transform.h
  cbf/debug_macros.h
  cbf/difference_sensor_transform.h
  cbf/dummy_reference.h
  cbf/dummy_resource.h
  cbf/effector_transform.h
  cbf/exceptions.h
  cbf/functional.h
  cbf/generic_transform.h
  cbf/identity_transform.h
  cbf/kdl_transforms.h
  cbf/linear_transform.h
  cbf/mainpage.h
  cbf/masking_resource.h
  cbf/memory_resource.h
  cbf/namespace.h
  cbf/norm_transform.h
  cbf/object.h
  cbf/object_reference.h
  cbf/pa10_joint_resource.h
  cbf/potential.h
  cbf/primitive_controller.h
  cbf/primitive_controller_resource.h
  cbf/python_wrap.h
  cbf/qt_reference.h
  cbf/qt_sensor_transform.h
  cbf/quaternion.h
  cbf/reference.h
  cbf/resource.h
  cbf/robotinterface_resource.h
  cbf/sensor_transform.h
  cbf/sensor_transform_wrapper.h
  cbf/spacenavi_reference.h
  cbf/square_potential.h
  cbf/svd.h
  cbf/task_space_plan.h
  cbf/transpose_transform.h
  cbf/types.h
  cbf/utilities.h
  cbf/weighted_sum_transforms.h
  cbf/xcf_memory_reference.h
  cbf/xcf_memory_resource.h
  cbf/xcf_memory_sensor_transform.h
  cbf/xcf_resource.h
  cbf/xcf_vector_reference.h
  cbf/xml_factory.h
  cbf/xsd_error_handler.h
  )

#add_definitions("-DCBF=CBF { } struct ERROR {NO_CONFIG_H_INCLUDED}\; namespace CBF")
 
#==============================================================================#
#                  Handling CBF- Libs, Includes and Linkdirs                   #
#==============================================================================#

set(CBF_LIBS "")

set(CBF_INCLUDES "")
set(CBF_LINKDIRS "")



if (CBF_HAVE_EIGEN2)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${EIGEN2_INC})
endif()


if (CBF_HAVE_QKDLVIEW)
  message(STATUS "adding qkdl_chain_view_transforms.cc " ${QKDLVIEW_LIB})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${QKDLVIEW_INCLUDES} ${QKDLTREEVIEW_INCLUDES})
  set(CBF_LIBS ${CBF_LIBS} ${QKDLVIEW_LIB} ${QKDLVIEW_LIB})
  
  set(CBF_SOURCES ${CBF_SOURCES} kdl_chain_view_transforms.cc)
  # set(CBF_HEADERS ${CBF_HEADERS} qkdl_chain_view_transforms.h)
endif()



if(CBF_HAVE_CPPAD)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${CPPAD_PATH})
endif()



if(CBF_HAVE_KDL)
  set(CBF_LIBS ${CBF_LIBS} ${KDL_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${KDL_INCLUDE_DIRS})
endif()



if (CBF_HAVE_MEMORY)
  set(CBF_LIBS ${CBF_LIBS} ${MEMORY_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${MEMORY_INCLUDE_DIRS})
endif()



if (CBF_HAVE_QT)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${QT_INCLUDES})
  set(CBF_LIBS ${CBF_LIBS} ${QT_LIBRARIES})
endif()



if (CBF_HAVE_XCF)
  set(CBF_LIBS ${CBF_LIBS} ${XCF_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${XCF_INCLUDE_DIRS}) 
endif()



if (CBF_HAVE_XMLTIO)
  set(CBF_LIBS ${CBF_LIBS} ${XMLTIO_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${XMLTIO_INCLUDE_DIRS})
endif()



if (CBF_HAVE_XRI)
  set(CBF_LIBS ${CBF_LIBS} ${XRI_LDFLAGS})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${XRI_INCLUDE_DIRS})
endif()



if(CBF_HAVE_XSD)
  link_directories("${PROJECT_BINARY_DIR}/schemas")
  set(CBF_INCLUDES ${CBF_INCLUDES} "${PROJECT_BIN_DIR}/libcbf" ${XSD_INC})
  set(CBF_LIBS ${CBF_LIBS} cbf_schemas ${XERCESC_LIB})
endif()



#=============================================================================#
#                            Handling CBF- Sources                            #
#=============================================================================#

if(CBF_HAVE_CPPAD)
  set(CBF_SOURCES ${CBF_SOURCES} cppad_sensor_transform.cc)
else()
  message(STATUS "  excluding cppad_sensor_transform.cc because cppad was not found")
endif()



if(CBF_HAVE_KDL)
  set(CBF_SOURCES ${CBF_SOURCES} kdl_transforms.cc)
else()
  message(STATUS "  excluding kdl_transforms.cc because KDL was not found")
endif()




if (CBF_HAVE_MEMORY)
  set(CBF_SOURCES ${CBF_SOURCES} memory_resource.cc)
else()
  message(STATUS "  excluding memory_ressource.cc because memory was not found")
endif()



if (CBF_HAVE_QT)
  set(CBF_SOURCES ${CBF_SOURCES} qt_reference.cc)
  set(CBF_SOURCES ${CBF_SOURCES} qt_sensor_transform.cc)
else()
  message(STATUS "  excluding qt_reference.cc and qt_sensor_transform.cc because qt was not found")
endif()



if (CBF_HAVE_SPACEMOUSE)
  set(CBF_SOURCES ${CBF_SOURCES} spacenavi_reference.cc)
else()
  message(STATUS "  excluding spacenavi_reference.cc because spacemouse was not found")
endif()


if (CBF_HAVE_XRI)
  set(CBF_SOURCES ${CBF_SOURCES} robotinterface_resource.cc)
else()
  message(STATUS "  excluding roboterinterface_ressource.cc because xri was not found")
endif()



if(CBF_HAVE_KDL AND CBF_HAVE_XSD)
  set(CBF_SOURCES ${CBF_SOURCES} c_api.cc)
else()
  message(STATUS "  excluding c_api.cc because KDL or XSD was not found")
endif()



if (CBF_HAVE_XCF AND CBF_HAVE_XMLTIO)
  set(CBF_SOURCES ${CBF_SOURCES} xcf_resource.cc)
else()
  message(STATUS "  excluding xcf_resource.cc because XCF or xmltio was not found")
endif()



if (CBF_HAVE_XCF AND CBF_HAVE_XSD)
  set(CBF_SOURCES ${CBF_SOURCES} xcf_vector_reference.cc)
else()
  message(STATUS "  excluding xcf_vector_reference.cc because XCF or XSD was not found")
endif()



if(CBF_HAVE_MEMORY AND CBF_HAVE_XCF AND CBF_HAVE_XMLTIO AND CBF_HAVE_XSD)
  set(CBF_SOURCES ${CBF_SOURCES} xcf_memory_sensor_transform.cc)
  set(CBF_SOURCES ${CBF_SOURCES} xcf_memory_reference.cc)
  set(CBF_SOURCES ${CBF_SOURCES} xcf_memory_resource.cc)
else()
  message(STATUS "  excluding")
  message(STATUS "  - xcf_memoy_sensor_transform.cc")
  message(STATUS "  - xcf_memory_reference.cc")
  message(STATUS "  - xcf_memory_resource.cc")
  message(STATUS "  because XCF, memory, xmltio or XSD was not found")
endif()

#=============================================================================#
#                            Building CBF- Library                            #
#=============================================================================#


include_directories(${CBF_INCLUDES})

add_library(cbf SHARED
  ${CBF_SOURCES}
  )
set_target_properties(cbf
  PROPERTIES PUBLIC_HEADER ${CBF_HEADERS})

target_link_libraries(cbf ${CBF_LIBS})

SET_TARGET_PROPERTIES(cbf
  PROPERTIES VERSION ${CBF_VERSION_MAJOR} 
  SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  )

install(TARGETS cbf
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
  PUBLIC_HEADER DESTINATION include/cbf
  )

install(
  FILES ${CBF_HEADERS} 
  DESTINATION include/cbf 
  PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
  )

install(
  DIRECTORY ${PROJECT_BINARY_DIR}/libcbf/cbf 
  DESTINATION include 
  DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE OWNER_EXECUTE GROUP_EXECUTE
  FILE_PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
  )
