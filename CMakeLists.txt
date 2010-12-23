cmake_minimum_required(VERSION 2.6)


project(CBF)

set(CBF_VERSION_MAJOR 0)
set(CBF_VERSION_MINOR 1)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD OFF)

if (CMAKE_BUILD_TYPE STREQUAL Debug)
  set (CBF_DEBUG_COLOR 1)
endif()

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DNDEBUG -DCBF_NDEBUG")

message(STATUS "==============================================================")
message(STATUS "Configuring CBF:")

message(STATUS "Looking for Boost components: program_options")
find_package(Boost 1.40.0 COMPONENTS program_options)

if(Boost_FOUND)
  set(CBF_HAVE_BOOST 1)
  set(CBF_HAVE_BOOST_PROGRAM_OPTIONS 1)
else()
  message(FATAL_ERROR "boost_program_options and boost_python are required for CBF")
endif()

message(STATUS "Looking for CppAD automatic differentiation package")
find_path(CPPAD_PATH cppad/CppAD.h)
if(NOT ${CPPAD_PATH} STREQUAL "CPPAD_PATH-NOTFOUND")
  message(STATUS "  found")
  set(CBF_HAVE_CPPAD 1)
endif()

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(PythonInterp "Found Python" python2)

find_package(PkgConfig)

message(STATUS "Looking for KDL")
pkg_check_modules(KDL orocos-kdl>=1.0)
if (KDL_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_KDL 1)
endif()

message(STATUS "Looking for XCF")
pkg_check_modules(XCF xcf>=0.9)
if (XCF_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XCF 1)
endif()

message(STATUS "Looking for XRI")
pkg_check_modules(XRI xri)
if (XRI_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XRI 1)
endif()

message(STATUS "Looking for XMLTIO")
pkg_check_modules(XMLTIO xmltio>=1.4)
if (XMLTIO_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XMLTIO 1)
endif()

message(STATUS "Looking for MEMORY")
pkg_check_modules(MEMORY Memory>=0.21)
if (MEMORY_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_MEMORY 1)
endif()

message(STATUS "Looking for EIGEN2")
pkg_check_modules(EIGEN2 eigen2)
if (EIGEN2_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_EIGEN2 1)
else()
  set(CBF_HAVE_EIGEN2 1)
  include_directories(eigen)
endif()


message(STATUS "Looking for QT4")
pkg_check_modules(QT QtCore QtGui)
if(QT_FOUND)
  message(STATUS "  found")
  find_package(Qt4)
  include(FindQt4)
  set(CBF_HAVE_QT 1)
endif()

#find_path(EIGEN2_INC eigen2/Eigen/Core)
#if (NOT "${EIGEN2_INC}" STREQUAL "EIGEN2_INC-NOTFOUND")
#  set(CBF_HAVE_EIGEN2 1)
#endif()

message(STATUS "Looking for kdlchainview")
find_library(KDLCHAINVIEW kdlchainview)
find_path(KDLCHAINVIEW_INCLUDES kdlchainview.h)
if(NOT "${KDLCHAINVIEW_LIBS}" STREQUAL "KDLCHAINVIEW_LIBS-NOTFOUND" AND NOT "${KDLCHAINVIEW_INCLUDES}" STREQUAL "KDLCHAINVIEW_INCLUDES-NOTFOUND")
  message(STATUS "  found kdlchainview ${KDLCHAINVIEW_LIBS} ${KDLCHAINVIEW_INCLUDES}")
  set(CBF_HAVE_KDLCHAINVIEW 1)
else()
  message(STATUS "  kdlchainview not found")
endif()

message(STATUS "Looking for Xerces-C")
find_library(XERCESC_LIB xerces-c)
find_path(XERCESC_INC xercesc/dom/DOM.hpp)
if(NOT "${XERCESC_LIB}" STREQUAL "XERCESC_LIB-NOTFOUND" AND NOT "${XERCESC_INC}" STREQUAL "XERCESC_INC-NOTFOUND")
  message(STATUS "  found Xerces-C ${XERCESC_LIB} ${XERCESC_INC}")
  set(CBF_HAVE_XERCESC 1)
else()
  message(STATUS "  Xerces-C not found")
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
  message(STATUS "  XSD CXX not found")
endif()

message(STATUS "Looking for pyxbgen")
find_program(PYXBGEN_BIN NAMES pyxbgen)
if (NOT "${PYXBGEN_BIN}" STREQUAL "PYXBGEN_BIN-NOTFOUND")
  message(STATUS "  found pyxbgen: ${PYXBGEN_BIN}")
  set(CBF_HAVE_PYXBGEN 1)
else()
  message(STATUS "  pyxbgen not found")
endif()

message(STATUS "Looking for git")
find_program(GIT_BIN NAMES git)
if (NOT "${GIT_BIN}" STREQUAL "GIT_BIN-NOTFOUND")
  message(STATUS "  found git: ${GIT_BIN}")
  set(CBF_HAVE_GIT 1)
else()
  message(STATUS "  git not found!")
endif()

message(STATUS "Looking for doxygen")
find_program(DOXYGEN_BIN NAMES doxygen)
if(NOT "${DOXYGEN_BIN}" STREQUAL "DOXYGEN_BIN-NOTFOUND")
  message(STATUS "found Doxygen, a documentation can be built with 'make doxy'")
  add_custom_target(doxy
    COMMAND ${DOXYGEN_BIN} ${PROJECT_BINARY_DIR}/Doxyfile
    COMMENT "Creating html and tex documentation in doc/"
  )
  install(
    DIRECTORY ${PROJECT_BINARY_DIR}/doc/html ${PROJECT_BINARY_DIR}/doc/latex
    DESTINATION share/doc/cbf/
  )
else()
  message(STATUS "  doxygen not found, documentation will not be available")
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
  )

configure_file(
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_BINARY_DIR}/Doxyfile"
  ) 

add_subdirectory(schemas)
add_subdirectory(libcbf)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(doc)
