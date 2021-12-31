function(findOpenBabel)
  find_path(OPENBABEL_INCLUDE_DIRS openbabel/obconversion.h
      ${OB_INCLUDE_DIRS}
      C:/openbabel/include
      C:/openbabel/include/openbabel3
      C:/openbabel/include/openbabel-2.0
      /usr/local/include
      /usr/local/include/openbabel3
      /usr/local/include/openbabel-2.0
      /usr/include
      /usr/include/openbabel3
      /usr/include/openbabel-2.0
      ${GNUWIN32_DIR}/include
  )

  find_library(OPENBABEL_LINK_LIBRARIES NAMES openbabel
      PATHS
      ${OB_LIBRARY_DIRS}
      C:/openbabel/lib
      /usr/lib
      /usr/lib64
      /usr/local/lib
      /usr/local/lib64
      ${GNUWIN32_DIR}/lib
  )

  if(NOT OPENBABEL_INCLUDE_DIRS)
    message(FATAL_ERROR "Could not find OpenBabel includes (try setting '-DOB_INCLUDE_DIRS=<DIR>' or '-DMSK_OBABELIFACE=false')")
  endif(NOT OPENBABEL_INCLUDE_DIRS)
  if(NOT OPENBABEL_LINK_LIBRARIES)
    message(FATAL_ERROR "Could not find OpenBabel libs (try setting '-DOB_LIBRARY_DIRS=<DIR>' or '-DMSK_OBABELIFACE=false')")
  endif(NOT OPENBABEL_LINK_LIBRARIES)

  set(OPENBABEL_FOUND true)
  message(STATUS "Found OpenBabel.  Includes: ${OPENBABEL_INCLUDE_DIRS} Libs: ${OPENBABEL_LINK_LIBRARIES}")
endfunction(findOpenBabel)

find_package(PkgConfig)

if(NOT WIN32 AND PkgConfig_FOUND)
  pkg_search_module(OPENBABEL openbabel-3)
  if(NOT OPENBABEL_FOUND)
    pkg_search_module(OPENBABEL openbabel-2.0)
    if(NOT OPENBABEL_FOUND)
      findOpenBabel()
    endif(NOT OPENBABEL_FOUND)
  endif(NOT OPENBABEL_FOUND)
else(NOT WIN32 AND PkgConfig_FOUND)
  findOpenBabel()
endif(NOT WIN32 AND PkgConfig_FOUND)
