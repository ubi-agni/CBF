#foo
if(NOT "${XMLSTARLET_BIN}" STREQUAL "XMLSTARLET_BIN-NOTFOUND")


  set(XML_SOURCES
	 composite_potential.xml
	 composite_sensor_transform.xml
	 error.xml
	 generic.xml
	 grasp_lab_tree_fingers.xml
	 grasp_lab_tree.xml
	 kdl_axis_angle.xml
	 kdl_kuka_composite_world_left.xml
	 kdl_kuka_composite_world_right_sing.xml
	 kdl_kuka_composite_world_right.xml
	 kdl_kuka_composite.xml
	 kdl_kuka_pos.xml
	 kdl_pos_only.xml
	 kdl_simple_pos.xml
	 kuka_chain_world_left.xml
	 kuka_chain_world_right.xml
	 kuka_chain_world.xml
	 kuka_chain.xml
	 linear.xml
	 memory_run_controller_test_controlbasis.xml
	 memory_run_controller_test_controller.xml
	 object_reference.xml
	 primitive_controller_resource.xml
	 python_wrap.xml
	 quaternion.xml
	 robotinterface_arm.xml
	 robotinterface_hand.xml
	 robotinterface_tree_diff_qt_reference.xml
	 robotinterface_tree_diff_qt.xml
	 robotinterface_tree_diff.xml
	 robotinterface_tree_norm_qt.xml
	 robotinterface_tree_norm.xml
	 robotinterface_tree.xml
	 robotinterface.xml
	 sensor_chain.xml
	 sequence.xml
	 simple_chain.xml
	 simple_diff.xml
	 simple_factor.xml
	 simple_memory_reference.xml
	 simple_memory_resource.xml
	 simple_memory_sensor_transform.xml
	 simple_multiply.xml
	 simple_negate.xml
	 simple_norm.xml
	 simple_sum.xml
	 simple_tree.xml
	 simple.xml
	 visual_servoing.xml
	 xcf_vector_reference_test.xml
	 )

  foreach(SOURCE ${XML_SOURCES})
	 MESSAGE("adding test " ${SOURCE} "${XMLSTARLET_BIN}" validate --err --xsd "${PROJECT_SOURCE_DIR}/schemas/schemas.xsd" "${SOURCE}")
	 ADD_TEST(${SOURCE} "${XMLSTARLET_BIN}" validate --err --xsd "${PROJECT_SOURCE_DIR}/schemas/schemas.xsd" "${SOURCE}")
  endforeach()

endif()