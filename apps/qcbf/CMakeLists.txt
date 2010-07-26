if(CBF_HAVE_PYXBGEN)
  add_custom_target(
	 CBF_Schema_Module
	 ALL
	 )

  add_custom_command(
	 COMMAND ${PYXBGEN_BIN} 
	 ARGS -m cbf -u ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
	 DEPENDS ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
	 WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/apps/qcbf
	 TARGET CBF_Schema_Module
	 OUTPUTS cbf.py
	 )
endif()
