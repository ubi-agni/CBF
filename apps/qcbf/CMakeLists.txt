message(STATUS "In subdirectory apps/qcbf:")

#if(CBF_HAVE_PYXBGEN)
#  add_custom_target(
#	 CBF_Schema_Module
#	 ALL
#	 )
#
#  add_custom_command(
#	 COMMAND ${PYXBGEN_BIN} -m cbf -u ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
#	 DEPENDS ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
#	 WORKING_DIRECTORY ${PROJECT_BINARY_DIR}/apps/qcbf
#	 TARGET CBF_Schema_Module
#	 OUTPUTS cbf.py
#	 )
#endif()
#

if(CBF_HAVE_PYXBGEN)
  message(STATUS "  creating target for pycbf.py from schemas.xsd")
  add_custom_command(
    OUTPUT ${PROJECT_BINARY_DIR}/apps/qcbf/pycbf.py
    COMMAND ${PYXBGEN_BIN} -m pycbf -u ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
    DEPENDS ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
    )
  
  add_custom_target(
    CBF_Schema_Module
    ALL
    DEPENDS ${PROJECT_BINARY_DIR}/apps/qcbf/pycbf.py
    )
else()
  message(STATUS "  cbf.py will not be created, because pyxbgen was not found")
endif()
