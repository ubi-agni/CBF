message(STATUS "In subdirectory schemas:")

if(CBF_HAVE_XSD)
  message(STATUS "  creating target for xcfschemas.hxx and xcfschemas.cxx")
  add_custom_command(
	 COMMAND ${XSD_BIN} cxx-tree --polymorphic-type-all --namespace-map 'http://xcf.sf.net=XCFSchema' --root-element-all --generate-serialization --generate-ostream --generate-doxygen --generate-polymorphic   --output-dir ${PROJECT_BINARY_DIR}/libcbf/cbf/ ${PROJECT_SOURCE_DIR}/schemas/xcfschemas.xsd
	 OUTPUT ${PROJECT_BINARY_DIR}/libcbf/cbf/xcfschemas.hxx ${PROJECT_BINARY_DIR}/libcbf/cbf/xcfschemas.cxx
	 DEPENDS ${PROJECT_SOURCE_DIR}/schemas/xcfschemas.xsd
	 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/schemas
	 )

  message(STATUS "  creating target for schemas.hxx and schemas.cxx")
  add_custom_command(
	 COMMAND ${XSD_BIN} cxx-tree --polymorphic-type-all --namespace-map 'http://www.cit-ec.uni-bielefeld.de/CBF=CBFSchema' --namespace-map 'http://xcf.sf.net=XCFSchema' --root-element-all --generate-serialization --generate-ostream --generate-doxygen --generate-polymorphic   --output-dir ${PROJECT_BINARY_DIR}/libcbf/cbf/ ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
	 OUTPUT ${PROJECT_BINARY_DIR}/libcbf/cbf/schemas.hxx ${PROJECT_BINARY_DIR}/libcbf/cbf/schemas.cxx
	 DEPENDS ${PROJECT_SOURCE_DIR}/schemas/schemas.xsd
	 WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/schemas
	 )

  include_directories("${XSD_INC}")

  add_library(cbf_schemas SHARED
	 "${PROJECT_BINARY_DIR}/libcbf/cbf/schemas.cxx"
	 "${PROJECT_BINARY_DIR}/libcbf/cbf/xcfschemas.cxx"
	 )
  set_target_properties(cbf_schemas PROPERTIES 
	 VERSION ${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}
	 )

  install(TARGETS cbf_schemas
    EXPORT CBFDepends
	 RUNTIME DESTINATION bin
	 LIBRARY DESTINATION lib
	 ARCHIVE DESTINATION lib/static
	 PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ GROUP_WRITE
	 )
else()
  message(STATUS "  schemas.hxx and schemas.cxx will not be created, because XSD was not found")
endif()
