cmake_minimum_required(VERSION 2.6)

project(CBF)

set(CBF_VERSION_MAJOR 1)
set(CBF_VERSION_MINOR 0)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD OFF)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (CBF_DEBUG_COLOR 1)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -DCBF_NDEBUG")

message(STATUS "==============================================================")
message(STATUS "Configuring CBF:")

message(STATUS "Looking for Boost components: program_options and python")
find_package(Boost 1.34.1 COMPONENTS program_options python)

if(Boost_FOUND)
  set(CBF_HAVE_BOOST 1)
  set(CBF_HAVE_BOOST_PROGRAM_OPTIONS 1)
  message(STATUS "  found poost_program_options and boost_python")
else()
  message(FATAL_ERROR "boost_program_options and boost_python are required for CBF")
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(PythonInterp "Found Python" python2)

find_package(PkgConfig)

message(STATUS "Looking for kdl")
pkg_check_modules(KDL orocos-kdl>=1.0)
if (KDL_FOUND)
  set(CBF_HAVE_KDL 1)
  message(STATUS "  found kdl")
else()
  message(STATUS "  kdl not found!")
endif()

message(STATUS "Looking for xcf")
pkg_check_modules(XCF xcf>=0.9)
if (XCF_FOUND)
  set(CBF_HAVE_XCF 1)
  message(STATUS "  found xcf")
else()
  message(STATUS "  xcf not found!")
endif()

message(STATUS "Looking for xri")
pkg_check_modules(XRI xri)
if (XRI_FOUND)
  set(CBF_HAVE_XRI 1)
  message(STATUS "  found xri")
else()
  message(STATUS "  xri not found!")
endif()

message(STATUS "Looking for xmltio")
pkg_check_modules(XMLTIO xmltio>=1.4)
if (XMLTIO_FOUND)
  set(CBF_HAVE_XMLTIO 1)
  message(STATUS "  found xmltio")
else()
  message(STATUS "  xmltio not found!")
endif()

message(STATUS "Looking for memory > 0.21")
pkg_check_modules(MEMORY Memory>=0.21)
if (MEMORY_FOUND)
  set(CBF_HAVE_MEMORY 1)
  message(STATUS "  found memory")
else()
  message(STATUS "  memory not found!")
endif()

message(STATUS "Looking for eigen2")
pkg_check_modules(EIGEN2 eigen2)
if (EIGEN2_FOUND)
  set(CBF_HAVE_EIGEN2 1)
  message(STATUS "  found eigen2")
else()
  message(STATUS "  eigen2s not found!")
endif()

#find_path(EIGEN2_INC eigen2/Eigen/Core)
#if (NOT "${EIGEN2_INC}" STREQUAL "EIGEN2_INC-NOTFOUND")
#  set(CBF_HAVE_EIGEN2 1)
#endif()

message(STATUS "Looking for Xerces-C")
find_library(XERCESC_LIB xerces-c)
find_path(XERCESC_INC xercesc/dom/DOM.hpp)
if(NOT "${XERCESC_LIB}" STREQUAL "XERCESC_LIB-NOTFOUND" AND NOT "${XERCESC_INC}" STREQUAL "XERCESC_INC-NOTFOUND")
  message(STATUS "  found Xerces-C ${XERCESC_LIB} ${XERCESC_INC}")
  set(CBF_HAVE_XERCESC 1)
else()
  message(STATUS "  Xerces-C not found!")
endif()


message(STATUS "Looking for XSD CXX")
find_program(XSD_BIN NAMES xsd xsdcxx)
find_path(XSD_INC xsd/cxx/config.hxx)
if (NOT "${XSD_BIN}" STREQUAL "XSD_BIN-NOTFOUND" AND NOT "${XSD_INC}" STREQUAL "XSD_INC-NOTFOUND" )
  message(STATUS "  found XSD CXX: ${XSD_BIN} ${XSD_INC}")
  if(CBF_HAVE_XERCESC)
	 set(CBF_HAVE_XSD 1)
  endif()
else()
  message(STATUS "  XSD CXX not found!")
endif()

message(STATUS "Looking for pyxbgen")
find_program(PYXBGEN_BIN NAMES pyxbgen)
if (NOT "${PYXBGEN_BIN}" STREQUAL "PYXBGEN_BIN-NOTFOUND")
  message(STATUS "  found pyxbgen: ${PYXBGEN_BIN}")
  set(CBF_HAVE_PYXBGEN 1)
else()
  message(STATUS "  pyxbgen not found!")
endif()

message(STATUS "Looking for doxygen")
find_program(DOXYGEN_BIN NAMES doxygen)
if(NOT "${DOXYGEN_BIN}" STREQUAL "DOXYGEN_BIN-NOTFOUND")
  message(STATUS "found Doxygen, a Documentation can be build with 'make doxy'")
  add_custom_target(doxy
    ALL
    COMMAND ${DOXYGEN_BIN} ${PROJECT_SOURCE_DIR}/Doxyfile
    COMMENT "Creating html and tex Documentation in doc/"
	 )
  install(
    DIRECTORY doc/html doc/latex
    DESTINATION share/doc/cbf/
    )
else()
  message(STATUS "  doxygen not found!")
endif()


configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
  )

message(STATUS "In subdirectory schemas:")
add_subdirectory(schemas)
message(STATUS "In subdirectory libcbf:")
add_subdirectory(libcbf)
message(STATUS "In subdirectory apps:")
add_subdirectory(apps)
message(STATUS "In subdirectory tests:")
add_subdirectory(tests)
message(STATUS "In subdirectory doc:")
add_subdirectory(doc)
