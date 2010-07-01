if(CBF_HAVE_XSD)
  add_custom_command(
	 COMMAND ${XSD_BIN} cxx-tree --root-element-all --generate-ostream --generate-doxygen --generate-polymorphic --output-dir ${PROJECT_BINARY_DIR}/libcbf/cbf/ schemas.xsd
	 OUTPUT ${PROJECT_BINARY_DIR}/libcbf/cbf/schemas.hxx ${PROJECT_BINARY_DIR}/libcbf/cbf/schemas.cxx
	 DEPENDS ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
	 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/schemas
	 )

  include_directories("${XSD_INC}")

  add_library(cbf_schemas SHARED
	 "${PROJECT_BINARY_DIR}/libcbf/cbf/schemas.cxx"
	 )

  install(TARGETS cbf_schemas
	 RUNTIME DESTINATION bin
	 LIBRARY DESTINATION lib
	 ARCHIVE DESTINATION lib/static
	 )
endif()