message(STATUS "In subdirectory schemas:")

if(CBF_HAVE_XSD)
  message(STATUS "  creating target for schemas.hxx and schemas.cxx")
  add_custom_command(
	 COMMAND ${XSD_BIN} cxx-tree --polymorphic-type-all --namespace-map 'http://www.cit-ec.uni-bielefeld.de/CBF=CBFSchema' --root-element-all --generate-serialization --generate-ostream --generate-doxygen --generate-polymorphic   --output-dir ${PROJECT_BINARY_DIR}/libcbf/cbf/ schemas.xsd
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
else()
  message(STATUS "  schemas.hxx and schemas.cxx will not be created, because XSD was not found")
endif()
