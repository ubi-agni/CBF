message(STATUS "In subdirectory libcbf:")

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
  primitive_controller_resource.cc 
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
  convergence_criterion.cc
  generic_transform.cc
  object_list.cc
  kdl_chain_direction_sensor_transform.cc
  quaternion_potential.cc
  line_potential.cc
  direction_potential.cc
  filter.cc
  pid_filter.cc
  cddyn_filter.cc
  dmp_filter.cc
  bypass_filter.cc
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
  cbf/foreign_object.h
  cbf/functional.h
  cbf/generic_transform.h
  cbf/identity_transform.h
  cbf/kdl_transforms.h
  cbf/linear_transform.h
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
  cbf/xml_object_factory.h
  cbf/xsd_error_handler.h
  cbf/kdl_chain_direction_sensor_transform.h
  cbf/quaternion_potential.h
  cbf/line_potential.h
  cbf/direction_potential.h
  cbf/filter.h
  cbf/pid_filter.h
  cbf/cddyn_filter.h
  cbf/dmp_filter.h
  cbf/bypass_filter.h
  )

if(CBF_HAVE_PYTHON)
  list(APPEND CBF_SOURCES python_wrap.cc)
  list(APPEND CBF_HEADERS cbf/python_wrap.h)
endif(CBF_HAVE_PYTHON)

#add_definitions("-DCBF=CBF { } struct ERROR {NO_CONFIG_H_INCLUDED}\; namespace CBF")
 
#==============================================================================#
#                  Handling CBF- Libs, Includes and Linkdirs                   #
#==============================================================================#

set(CBF_LIBS "")

set(CBF_INCLUDES "")
set(CBF_LINKDIRS "")

if (CBF_HAVE_QKDLVIEW)
  message(STATUS "adding qkdl_chain_view_transforms.cc " ${QKDLVIEW_LIB})
  set(CBF_INCLUDES ${CBF_INCLUDES} ${QKDLVIEW_INCLUDES} ${QKDLTREEVIEW_INCLUDES})
  set(CBF_LIBS ${CBF_LIBS} ${QKDLVIEW_LIB})
  
  set(CBF_SOURCES ${CBF_SOURCES} kdl_chain_view_transforms.cc)
endif()

if(CBF_HAVE_CPPAD)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${CPPAD_PATH})
endif()

if(CBF_HAVE_PYTHON)
  set(CBF_INCLUDES ${CBF_INCLUDES} ${PYTHON_INCLUDE_DIRS})
  set(CBF_LIBS ${CBF_LIBS} ${Boost_PYTHON_LIBRARIES} ${PYTHON_LIBRARIES})
endif()

if(CBF_HAVE_KDL)
  set(CBF_LIBS ${CBF_LIBS} ${KDL_LIBRARIES})
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
  set(CBF_LIBS ${CBF_LIBS} ${CBF_LIBRARY_NAME}_schemas)
endif()

if(CBF_HAVE_CDDYN)
  set(CBF_LIBS ${CBF_LIBS} ${cd_dynamics_LIBRARIES})
  set(CBF_INCLUDES ${cd_dynamics_INCLUDE_DIRS} ${CBF_INCLUDES})
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
  set(CBF_INCLUDES ${CBF_INCLUDES} ${SPACEMOUSE_INCLUDES})
  set(CBF_LIBS ${CBF_LIBS} ${SPACEMOUSE_LIBS})
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

add_library(${CBF_LIBRARY_NAME} SHARED
    ${CBF_SOURCES})
set_target_properties(${CBF_LIBRARY_NAME}
  PROPERTIES 
  PUBLIC_HEADER "${CBF_HEADERS}"
  VERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib;${MEMORY_LIBRARY_DIRS}"
  )
target_link_libraries(${CBF_LIBRARY_NAME} ${CBF_LIBS})

install(TARGETS ${CBF_LIBRARY_NAME}
  EXPORT CBFDepends
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/${CBF_PATH_PREFIX}/cbf
  )

# generated files: config.h, schemas*
install(
  DIRECTORY ${PROJECT_BINARY_DIR}/libcbf/cbf 
  DESTINATION include/${CBF_PATH_PREFIX}
  )

