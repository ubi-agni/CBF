include_directories("${PROJECT_SOURCE_DIR}/libcbf/")

if(CBF_HAVE_XSD)
  link_directories("${PROJECT_BINARY_DIR}/schemas")
  include_directories("${PROJECT_BIN_DIR}/libcbf")
endif()

set(CBF_SOURCES
  control_basis.cc
  controller.cc 
  composite_reference.cc
  masking_resource.cc
  controller_sequence.cc
  identity_transform.cc 
  primitive_controller.cc 
  composite_primitive_controller.cc 
  resource.cc 
  dummy_resource.cc 
  sensor_transform.cc 
  effector_transform.cc 
  potential.cc 
  utilities.cc
  combination_strategy.cc
  composite_transform.cc
  axis_angle_potential.cc
  axis_potential.cc
  composite_potential.cc
  square_potential.cc
  linear_transform.cc 
  transform_chains.cc
  composite_resource.cc
  reference.cc
  dummy_reference.cc
  gnuplot_i.cpp
  quaternion.cc
  difference_sensor_transform.cc
  weighted_sum_transforms.cc
  )

set(CBF_LIBS "")

if (CBF_HAVE_SPACEMOUSE)
  set(CBF_SOURCES ${CBF_SOURCES} spacenavi_reference.cc)
endif()

if (CBF_HAVE_MEMORY)
  set(CBF_SOURCES ${CBF_SOURCES} memory_resource.cc)
endif()

if (CBF_HAVE_XCF AND CBF_HAVE_XMLTIO)
  set(CBF_SOURCES ${CBF_SOURCES} xcf_resource.cc)
endif()


if (CBF_HAVE_ROBOTINTERFACE)
  set(CBF_SOURCES ${CBF_SOURCES} robotinterface_resource.cc)
endif()

if (CBF_HAVE_EIGEN2)
  set(CBF_SOURCES ${CBF_SOURCES} generic_transform.cc)
endif()



if(CBF_HAVE_XSD)
  set(CBF_LIBS ${CBF_LIBS} cbf_schemas)
  set(CBF_SOURCES ${CBF_SOURCES} plugin_pool.cc)
endif()

add_library(cbf SHARED
  ${CBF_SOURCES}
  )

target_link_libraries(cbf ${CBF_LIBS})

install(TARGETS cbf
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib/static
  )

