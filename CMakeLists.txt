cmake_minimum_required(VERSION 2.6)
project(cbf)

set(CBF_VERSION_MAJOR 0)
set(CBF_VERSION_MINOR 1)
set(CBF_VERSION_PATCH 1)
set(CBF_VERSION "${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}")
set(CBF_VERSIONED_NAME "${PROJECT_NAME}${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}")
set(CBF_PATH_PREFIX "${CBF_VERSIONED_NAME}")
set(CBF_LIBRARY_NAME "${CBF_VERSIONED_NAME}")

# module path
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "share/${CBF_PATH_PREFIX}")

include(CheckIncludeFileCXX)
include(InstallFilesRecursive)
include(FindPkgConfig)
include(ParseArguments)

enable_testing()
include(CTest) 

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREAD OFF)

IF(NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
ENDIF(NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DCBF_NDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCBF_DEBUG_COLOR")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DCBF_NDEBUG")

message(STATUS "==============================================================")
message(STATUS "Configuring CBF:")

message(STATUS "Looking for Boost components: program_options")
find_package(Boost 1.40.0 COMPONENTS program_options)

if(Boost_FOUND)
  set(CBF_HAVE_BOOST 1)
  set(CBF_HAVE_BOOST_PROGRAM_OPTIONS 1)
endif()

message(STATUS "Looking for CppAD automatic differentiation package")
find_path(CPPAD_PATH cppad/CppAD.h)
if(NOT ${CPPAD_PATH} STREQUAL "CPPAD_PATH-NOTFOUND")
  message(STATUS "  found")
  set(CBF_HAVE_CPPAD 1)
endif()
set(CBF_HAVE_CPPAD 0)

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

# find eigen2
message(STATUS "Looking for EIGEN2")
find_package(Eigen2 2.0 REQUIRED)
if(EIGEN2_FOUND)
    set(CBF_HAVE_EIGEN2 1)
    message(STATUS "  ${EIGEN2_VERSION} found")
    include_directories(SYSTEM ${EIGEN2_INCLUDE_DIR})
    
    # In case anything else includes eigen3 (e.g. kdl), we need eigen2 support
    add_definitions("-DEIGEN2_SUPPORT")
endif()

message(STATUS "Looking for QT4")
pkg_check_modules(QT QtCore QtGui)
if(QT_FOUND)
  message(STATUS "  found")
  find_package(Qt4)
  include(FindQt4)
  set(CBF_HAVE_QT 1)
endif()

message(STATUS "Looking for qkdlview")
find_library(QKDLVIEW_LIB qkdlview)
find_path(QKDLVIEW_INCLUDES qkdltreeview.h)
if(
	 NOT "${QKDLVIEW_LIB}" STREQUAL "QKDLVIEW_LIB-NOTFOUND" 
	 AND 
	 NOT "${QKDLVIEW_INCLUDES}" STREQUAL "QKDLVIEW_INCLUDES-NOTFOUND"
	 )
  message(STATUS "  found qkdlview ${QKDLVIEW_LIB} ${QKDLVIEW_INCLUDES}")
  set(CBF_HAVE_QKDLVIEW 1)
else()
  message(STATUS "  qkdlview not found")
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

message(STATUS "Looking for xmlstarlet")
find_program(XMLSTARLET_BIN NAMES xmlstarlet)

message(STATUS "Looking for XSD CXX")
find_program(XSD_BIN NAMES xsdcxx xsd)
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

configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
  )

configure_file(
  "${PROJECT_SOURCE_DIR}/Doxyfile.in"
  "${PROJECT_BINARY_DIR}/Doxyfile"
  ) 

# --- pkgconfig file ---
configure_file(cbf.pc.in ${CMAKE_BINARY_DIR}/${CBF_PATH_PREFIX}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${CBF_PATH_PREFIX}.pc DESTINATION lib/pkgconfig)

# --- cmake config file ---
set(LIB_SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})
if(CMAKE_LINK_LIBRARY_SUFFIX)
    set(LIB_SUFFIX ${CMAKE_LINK_LIBRARY_SUFFIX})
endif()
configure_file(CBFConfig.cmake.in ${CMAKE_BINARY_DIR}/CBFConfig.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CBFConfig.cmake DESTINATION
              share/${CBF_PATH_PREFIX})
configure_file(CBFConfigVersion.cmake.in ${CMAKE_BINARY_DIR}/CBFConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CBFConfigVersion.cmake DESTINATION
              share/${CBF_PATH_PREFIX})

EXPORT(TARGETS ${LIB_NAME} FILE "${CMAKE_BINARY_DIR}/CBFDepends.cmake")
INSTALL(EXPORT CBFDepends
        DESTINATION "share/${CBF_PATH_PREFIX}")

add_subdirectory(schemas)
add_subdirectory(libcbf)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(doc)

# --- package ---
set(CPACK_PACKAGE_VERSION_MAJOR ${CBF_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CBF_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CBF_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "CoR-Lab Bielefeld University")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")

include(ProvideFlexibleCPack)
