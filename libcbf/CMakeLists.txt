message(STATUS "In subdirectory libcbf:")

include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")
include_directories("${PROJECT_SOURCE_DIR}/libcbf/cbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/cbf/")

set(CBF_SOURCES c_bindings.cc
                resource.cc
                potential.cc
                dummy_resource.cc
                avoid_singularities.cc
                reference.cc
                primitive_controller.cc
                controller_sequence.cc
                quaternion.cc
                composite_primitive_controller.cc
                difference_sensor_transform.cc
                weighted_sum_transforms.cc
                composite_transform.cc
                combination_strategy.cc
                dummy_reference.cc
                sensor_transform.cc
                transform_chains.cc
                generic_transform.cc
                effector_transform.cc
                composite_resource.cc
                identity_transform.cc
#                python_wrap.cc
#                primitive_controller_resource.cc
                controller.cc
                orientation_potentials.cc
#                python_bindings.cc
                linear_transform.cc
                utilities.cc
                plugin_pool.cc
    )

set(CBF_HEADERS cbf/config.h
                cbf/composite_primitive_controller.h
                cbf/avoid_singularities.h
                cbf/utilities.h
                cbf/sensor_transform_wrapper.h
                cbf/sensor_transform.h
                cbf/reference.h
                cbf/difference_sensor_transform.h
                cbf/c_bindings.h
                cbf/effector_transform.h
                cbf/controller.h
                cbf/quaternion.h
                cbf/potential.h
                cbf/identity_transform.h
                cbf/spacenavi_reference.h
                cbf/combination_strategy.h
                cbf/transform_chains.h
                cbf/weighted_sum_transforms.h
                cbf/primitive_controller.h
                cbf/types.h
                cbf/kdl_transforms.h
                cbf/dummy_resource.h
                cbf/composite_transform.h
                cbf/mainpage.h
                cbf/orientation_potentials.h
                cbf/primitive_controller_resource.h
                cbf/generic_transform.h
                cbf/dummy_reference.h
                cbf/cbf.h
                cbf/linear_transform.h
                cbf/resource.h
                cbf/debug_macros.h
                cbf/controller_sequence.h
                cbf/composite_resource.h
                cbf/c_api.h 
                cbf/plugin_pool.h
                cbf/plugin_macros.h
                cbf/python_wrap.h
                cbf/plugin_decl_macros.h
                cbf/plugin_impl_macros.h
    ) 

#add_definitions("-DCBF=CBF { } struct ERROR {NO_CONFIG_H_INCLUDED}\; namespace CBF")
 

#=============================================================================#
#                            Handling CBF- Sources                            #
#=============================================================================#

if(CBF_HAVE_KDL)
  set(CBF_SOURCES ${CBF_SOURCES} kdl_transforms.cc)
else()
  message(STATUS "  excluding kdl_transforms.cc because KDL was not found")
endif()

if (CBF_HAVE_SPACEMOUSE)
  set(CBF_SOURCES ${CBF_SOURCES} spacenavi_reference.cc)
else()
  message(STATUS "  excluding spacenavi_reference.cc because spacemouse was not found")
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

#=============================================================================#
#                            Building CBF- Library                            #
#=============================================================================#

include_directories(system ${CBF_INCLUDES})

add_library(${CBF_LIBRARY_NAME} SHARED
    ${CBF_SOURCES})
target_link_libraries(${CBF_LIBRARY_NAME} ${CBF_LIBS})

SET_TARGET_PROPERTIES(${CBF_LIBRARY_NAME}
  PROPERTIES VERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  SOVERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
  )
set(CBF_LIBRARIES "${CBF_LIBRARIES} ${CBF_LIBRARY_NAME}" PARENT_SCOPE)

install(TARGETS ${CBF_LIBRARY_NAME}
  EXPORT CBFDepends
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  PUBLIC_HEADER DESTINATION include/${CBF_PATH_PREFIX}
  )

install(
  FILES ${CBF_HEADERS} 
  DESTINATION include/${CBF_PATH_PREFIX}/cbf
  )
