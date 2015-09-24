cmake_minimum_required(VERSION 2.6)
project(cbf)

set(CBF_VERSION_MAJOR 0)
set(CBF_VERSION_MINOR 2)
set(CBF_VERSION_PATCH 0)
set(CBF_VERSION "${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}")

# use versioned install directories?
set(CBF_VERSIONED_INSTALL 1 CACHE BOOL 
  "use versioned install paths, e.g. include/cbfx.y")
set(CBF_VERSIONED_NAME "${PROJECT_NAME}${CBF_VERSION_MAJOR}.${CBF_VERSION_MINOR}")
if(CBF_VERSIONED_INSTALL)
  set(CBF_PATH_PREFIX "${CBF_VERSIONED_NAME}")
  set(CBF_PKGCFG_FILE "${CBF_VERSIONED_NAME}.pc")
else(CBF_VERSIONED_INSTALL)
  set(CBF_PATH_PREFIX "")
  set(CBF_PKGCFG_FILE "${PROJECT_NAME}.pc")
endif(CBF_VERSIONED_INSTALL)
set(CBF_LIBRARY_NAME "${PROJECT_NAME}")

message(STATUS "==============================================================")
message(STATUS "Configuring CBF:")

# define default install prefix, if empty
IF(NOT CMAKE_INSTALL_PREFIX)
  SET(CMAKE_INSTALL_PREFIX "/usr/local/" CACHE PATH "Installation directory" FORCE)
  MESSAGE(STATUS "Setting installation directory to ${CMAKE_INSTALL_PREFIX}" )
ENDIF(NOT CMAKE_INSTALL_PREFIX)

# include cmake packages
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")
include(CheckIncludeFileCXX)
include(InstallFilesRecursive)
include(FindPkgConfig)
include(ParseArguments)

enable_testing()
include(CTest) 

# redefine pkg-config macro to enforce re-evaluation of PKG_CONFIG_PATH
macro(pkg_check_modules _prefix)
  _pkgconfig_parse_options(_pkg_modules _pkg_is_required _pkg_is_silent ${ARGN})
  _pkg_check_modules_internal("${_pkg_is_required}" "${_pkg_is_silent}" "${_prefix}" ${_pkg_modules})
endmacro(pkg_check_modules)

# define build type
IF (NOT CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
  MESSAGE(STATUS "Setting build type to '${CMAKE_BUILD_TYPE}'" )
ELSE (NOT CMAKE_BUILD_TYPE)
  MESSAGE(STATUS "Build type set to '${CMAKE_BUILD_TYPE}'" )
ENDIF (NOT CMAKE_BUILD_TYPE)

# and corresponding flags
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DCBF_NDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DCBF_DEBUG_COLOR")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DCBF_NDEBUG")

# add the automatically determined parts of the RPATH
# which point to directories outside the build tree to the install RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)


# define include directories for here, to ensure that local files are found first
include_directories("${PROJECT_SOURCE_DIR}/libcbf/")
include_directories("${PROJECT_BINARY_DIR}/libcbf/")

# Python
message(STATUS "Looking for python libs")
find_package(PythonInterp)
find_package(PythonLibs ${PYTHON_VERSION_STRING})
# check whether we can compile python
if(PYTHONLIBS_FOUND)
  include(CheckCSourceCompiles)
  set(CMAKE_REQUIRED_LIBRARIES ${PYTHON_LIBRARIES})
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_C_CFLAGS})
  set(CMAKE_REQUIRED_INCLUDES ${PYTHON_INCLUDE_DIRS})
  check_c_source_compiles("
#include <Python.h>
main() {Py_Initialize();}
" PYTHON_COMPILES)
  set(PYTHONLIBS_FOUND ${PYTHON_COMPILES})
endif()

# Boost libs
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREAD ON)
set(Boost_COMPONENTS "program_options")
if(PYTHONLIBS_FOUND)
  list(APPEND Boost_COMPONENTS "python")
endif()
message(STATUS "Looking for boost components: ${Boost_COMPONENTS}")
find_package(Boost 1.40.0 COMPONENTS ${Boost_COMPONENTS})

set(CBF_HAVE_BOOST ${Boost_FOUND})
set(CBF_HAVE_BOOST_PROGRAM_OPTIONS ${Boost_PROGRAM_OPTIONS_FOUND})
if(PYTHONLIBS_FOUND AND Boost_PYTHON_FOUND) 
  set(CBF_HAVE_PYTHON 1)
else()
  set(CBF_HAVE_PYTHON 0)
endif()

# cppAD
message(STATUS "Looking for CppAD automatic differentiation package")
find_path(CPPAD_PATH cppad/CppAD.h)
if(CPPAD_PATH)
  message(STATUS "  found: ${CPPAD_PATH}")
  set(CBF_HAVE_CPPAD 1)
else()
  set(CBF_HAVE_CPPAD 0)
endif()

# orocos KDL
message(STATUS "Looking for KDL")
pkg_check_modules(KDL orocos-kdl>=1.0)
if(KDL_FOUND)
  message(STATUS "  found: ${KDL_INCLUDE_DIRS}")
  set(CBF_HAVE_KDL 1)
  link_directories(${KDL_LIBRARY_DIRS})
  string(REPLACE ";" " " KDL_CFLAGS_SPACES "${KDL_CFLAGS}")
endif(KDL_FOUND)

# XCF
message(STATUS "Looking for XCF")
pkg_check_modules(XCF xcf>=0.9)
if (XCF_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XCF 1)
endif()

# XRI
message(STATUS "Looking for XRI")
pkg_check_modules(XRI xri)
if (XRI_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XRI 1)
endif()

# xmltio
message(STATUS "Looking for XMLTIO")
pkg_check_modules(XMLTIO xmltio>=1.4)
if (XMLTIO_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_XMLTIO 1)
endif()

# ActiveMemory
message(STATUS "Looking for ActiveMemory")
pkg_check_modules(MEMORY Memory>=0.21)
if (MEMORY_FOUND)
  message(STATUS "  found")
  set(CBF_HAVE_MEMORY 1)
endif()

# Eigen
if(CBF_HAVE_KDL)
  ## We need to ensure, that we use the same version of Eigen (2 vs. 3) as KDL.
  ## See: https://github.com/norro/CBF/issues/2
  if ("${KDL_INCLUDE_DIRS}" MATCHES ".*eigen3.*")
    set(CBF_HAVE_EIGEN 1)
    include_directories(${KDL_INCLUDE_DIRS})
  else()
    message(SEND_ERROR "KDL doesn't use Eigen3")
  endif()
else(CBF_HAVE_KDL)
  ## If we don't have KDL, we are free to choose Eigen
  message(STATUS "Looking for Eigen3")
  find_package(Eigen3 REQUIRED)
  if(EIGEN3_FOUND)
    message(STATUS "  ${EIGEN3_VERSION} found.")
    set(CBF_HAVE_EIGEN 1)
    include_directories(${EIGEN3_INCLUDE_DIR})
  endif()
endif(CBF_HAVE_KDL)

# Qt
message(STATUS "Looking for Qt5 or Qt4")
find_package(Qt54 COMPONENTS Core Gui)
if(QT_FOUND)
  # check whether we can compile+link against Qt
  include(CheckCXXSourceCompiles)
  set(CMAKE_REQUIRED_LIBRARIES ${QT_LIBRARIES})
  set(CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_CFLAGS})
  set(CMAKE_REQUIRED_INCLUDES ${QT_INCLUDES})
  check_cxx_source_compiles("
#include <QApplication>
main(int argc, char **argv) {QApplication app(argc, argv); return app.exec();}
" QT_COMPILES)
  set(QT_FOUND ${QT_COMPILES})
endif()
if(QT_FOUND)
  set(CBF_HAVE_QT 1)
endif(QT_FOUND)

# libqkdlview
message(STATUS "Looking for qkdlview")
find_library(QKDLVIEW_LIB qkdlview)
find_path(QKDLVIEW_INCLUDES qkdltreeview.h)
if(QKDLVIEW_LIB AND QKDLVIEW_INCLUDES)
  message(STATUS "  found qkdlview ${QKDLVIEW_LIB} ${QKDLVIEW_INCLUDES}")
  find_package(Qt54 COMPONENTS OpenGL)
  set(CBF_HAVE_QKDLVIEW 1)
endif()

# spacemouse
message(STATUS "Looking for spacemouse")
find_library(SPACEMOUSE_LIBS spacenavi)
find_path(SPACEMOUSE_INCLUDES spacenavi.h)
if(SPACEMOUSE_LIBS AND SPACEMOUSE_INCLUDES)
  message(STATUS "  found spacemouse ${SPACEMOUSE_LIBS} ${SPACEMOUSE_INCLUDES}")
  set(CBF_HAVE_SPACEMOUSE 1)
endif()

# xerces: required for header-only xsd
message(STATUS "Looking for Xerces-C")
find_library(XERCESC_LIB xerces-c)
find_path(XERCESC_INC xercesc/dom/DOM.hpp)
if(XERCESC_LIB AND XERCESC_INC)
  message(STATUS "  found Xerces-C ${XERCESC_LIB} ${XERCESC_INC}")
  set(CBF_HAVE_XERCESC 1)
endif()

message(STATUS "Looking for xmlstarlet")
find_program(XMLSTARLET_BIN NAMES xmlstarlet)

message(STATUS "Looking for XSD CXX")
find_program(XSD_BIN NAMES xsdcxx xsd)
find_path(XSD_INC xsd/cxx/config.hxx)
if (XSD_BIN AND XSD_INC)
  message(STATUS "  found XSD CXX: ${XSD_BIN} ${XSD_INC}")
  if(CBF_HAVE_XERCESC)
    set(CBF_HAVE_XSD 1)
  endif()
endif()

message(STATUS "Looking for pyxbgen")
find_program(PYXBGEN_BIN NAMES pyxbgen)
if (PYXBGEN_BIN AND EXISTS ${PYXBGEN_BIN})
  message(STATUS "  found pyxbgen: ${PYXBGEN_BIN}")
  get_filename_component(_pyxb_dir "${PYXBGEN_BIN}" PATH)
  find_path(PYXBGEN_PATH pyxb "${_pyxb_dir}/../lib/python2.7/site-packages")
  if(PYXBGEN_PATH)
    message(STATUS "  using PYTHONPATH=${PYXBGEN_PATH}")
    set(CBF_HAVE_PYXBGEN 1)
  endif()
endif()

configure_file(
  "${PROJECT_SOURCE_DIR}/libcbf/cbf/config.h.in"
  "${PROJECT_BINARY_DIR}/libcbf/cbf/config.h"
  )

add_subdirectory(schemas)
add_subdirectory(libcbf)
add_subdirectory(apps)
add_subdirectory(tests)
add_subdirectory(doc)

# --- doxygen: process after schemas subdir, because DOXYGEN_XSD_INPUTS is set there ---
message(STATUS "Looking for doxygen")
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(
    "${PROJECT_SOURCE_DIR}/Doxyfile.in"
    "${PROJECT_BINARY_DIR}/Doxyfile"
    )

  set(DOXYGEN_INPUT  ${PROJECT_BINARY_DIR}/Doxyfile)
  set(DOXYGEN_OUTPUT ${PROJECT_BINARY_DIR}/doc/html/index.html)
  file(GLOB_RECURSE DOXYGEN_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR} FOLLOW_SYMLINKS libcbf/*.[hc]* doc/* apps/*)
  add_custom_command(
    OUTPUT  ${DOXYGEN_OUTPUT}
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DOXYGEN_DEPENDS} ${DOXYGEN_INPUT}
    )
  add_custom_target(doc ALL DEPENDS ${DOXYGEN_OUTPUT})
endif(DOXYGEN_FOUND)

# --- pkgconfig file ---
configure_file(cbf.pc.in ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pc 
        DESTINATION lib/pkgconfig
        RENAME ${CBF_PKGCFG_FILE}
        )

# --- cmake config files ---
set(CONFIG_INSTALL_DIR "share/${CBF_PATH_PREFIX}")
file(RELATIVE_PATH CBF_REL_INCLUDE_DIR
  "${CMAKE_INSTALL_PREFIX}/${CONFIG_INSTALL_DIR}"
  "${CMAKE_INSTALL_PREFIX}/include/${CBF_PATH_PREFIX}")

configure_file(CBFConfig.cmake.in ${CMAKE_BINARY_DIR}/CBFConfig.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CBFConfig.cmake
        DESTINATION "${CONFIG_INSTALL_DIR}")
configure_file(CBFConfigVersion.cmake.in ${CMAKE_BINARY_DIR}/CBFConfigVersion.cmake @ONLY)
install(FILES ${CMAKE_BINARY_DIR}/CBFConfigVersion.cmake
        DESTINATION "${CONFIG_INSTALL_DIR}")

set(CBF_EXPORTS ${CBF_LIBRARY_NAME})
if(CBF_HAVE_XSD)
  set(CBF_EXPORTS ${CBF_EXPORTS} ${CBF_LIBRARY_NAME}_schemas)
endif()

EXPORT(TARGETS ${CBF_EXPORTS} 
       FILE "${CMAKE_BINARY_DIR}/CBFDepends.cmake")
INSTALL(EXPORT CBFDepends
        DESTINATION "${CONFIG_INSTALL_DIR}")

# --- package ---
set(CPACK_PACKAGE_VERSION_MAJOR ${CBF_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${CBF_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${CBF_VERSION_PATCH})
set(CPACK_PACKAGE_VENDOR "CoR-Lab Bielefeld University")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING.txt")

include(ProvideFlexibleCPack)
