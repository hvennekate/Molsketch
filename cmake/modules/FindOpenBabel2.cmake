# - Try to find OpenBabel2
# Once done this will define
#
#  OPENBABEL2_FOUND - system has OpenBabel2
#  OPENBABEL2_INCLUDE_DIR - the OpenBabel2 include directory
#  OPENBABEL2_LIBRARIES - Link these to use OpenBabel2

# Copyright (c) 2006, Carsten Niehaus, <cniehaus@gmx.de>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.

if (OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES)

  # in cache already
  set(OPENBABEL2_FOUND TRUE)

else (OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES)

  # not cached
  if(NOT WIN32)
#    set(PKG_CONFIG_PATH ${PKG_CONFIG_PATH} /usr/local)
    pkg_check_modules(OPENBABEL2 openbabel-3)

    # query pkg-config asking for a openbabel >= 3.0.0
    exec_program(${PKG_CONFIG_EXECUTABLE} ARGS --atleast-version=3.0.0 openbabel-3 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _pkgconfigDevNull )
    if(_return_VALUE STREQUAL "0")
      set(OPENBABEL_MINI_FOUND TRUE)
    endif(_return_VALUE STREQUAL "0")

    exec_program(${PKG_CONFIG_EXECUTABLE} ARGS --variable=pkgincludedir openbabel-3 RETURN_VALUE _return_VALUE OUTPUT_VARIABLE _obPkgIncDir )
    message(STATUS "Querying include dir: ${_obPkgIncDir}")
    if (_obPkgIncDir)
      set(OPENBABEL2_INCLUDE_DIR "${_obPkgIncDir}")
    endif (_obPkgIncDir)

  else(NOT WIN32)
    set(OPENBABEL_MINI_FOUND TRUE)
  endif(NOT WIN32)
  
  if(OPENBABEL_MINI_FOUND)

    # minimal installation on unix / all windows

    message(STATUS "OpenBabel include dir: ${OPENBABEL2_INCLUDE_DIR}")
    find_path(OPENBABEL2_INCLUDE_DIR openbabel/obconversion.h
      ${_obIncDir}
      /usr/local/include
      /usr/local/include/openbabel-2.0
      /usr/include
      ${GNUWIN32_DIR}/include
      $ENV{OPENBABEL2_INCLUDE_DIR}
    )

    message(STATUS "OpenBabel include dir2: ${OPENBABEL2_INCLUDE_DIR}")
    find_library(OPENBABEL2_LIBRARIES NAMES openbabel
      PATHS
      ${_obLinkDir}
      /usr/lib
      /usr/local/lib
      ${GNUWIN32_DIR}/lib
      $ENV{OPENBABEL2_LIBRARIES}
    )
    
    message(STATUS "OpenBabel include dir3: ${OPENBABEL2_INCLUDE_DIR}")

  endif(OPENBABEL_MINI_FOUND)

  if(OPENBABEL2_INCLUDE_DIRS AND OPENBABEL2_LIBRARIES AND OPENBABEL_MINI_FOUND)
    set(OPENBABEL2_FOUND TRUE)
  endif(OPENBABEL2_INCLUDE_DIRS AND OPENBABEL2_LIBRARIES AND OPENBABEL_MINI_FOUND)

  if (OPENBABEL2_FOUND)
    if (NOT OpenBabel2_FIND_QUIETLY)
      message(STATUS "Found OpenBabel2: ${OPENBABEL2_LIBRARIES}")
    endif (NOT OpenBabel2_FIND_QUIETLY)
  else (OPENBABEL2_FOUND)
    if (OpenBabel2_FIND_REQUIRED)
      message(FATAL_ERROR "Could NOT find OpenBabel2")
    endif (OpenBabel2_FIND_REQUIRED)
  endif (OPENBABEL2_FOUND)

  mark_as_advanced(OPENBABEL2_INCLUDE_DIRS OPENBABEL2_LIBRARIES)

endif (OPENBABEL2_INCLUDE_DIR AND OPENBABEL2_LIBRARIES)
