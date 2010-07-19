if(CBF_HAVE_XSD AND CBF_HAVE_BOOST_PROGRAM_OPTIONS)
  include_directories("${PROJECT_SOURCE_DIR}/libcbf/" ${XSD_INC})

  add_executable(cbf_run_controller 
	 cbf_run_controller_from_xml.cc
	 )

  target_link_libraries(cbf_run_controller
	 cbf
	 ${Boost_LIBRARIES}
	 )
endif()
