# CMakeLists.txt for the main sourcecode of molsKetch's library
cmake_policy(SET CMP0003 NEW)

include(../include_qt.cmake)

set(QT_USE_QTSVG TRUE) # TODO check if really required (also in other files)

# Ensure the Molsketch include directory is always first
get_directory_property(tmp_include_dirs INCLUDE_DIRECTORIES)
include_directories(${CMAKE_SOURCE_DIR}/libmskcore)

file(GLOB_RECURSE libmolsketch_HDRS *.h)
file(GLOB_RECURSE libmolsketch_SRCS *.cpp)
file(GLOB_RECURSE libmolsketch_UIS *.ui)
file(GLOB_RECURSE libmolsketch_RCS *.qrc)

include("../generateTranslations.cmake")
addTranslationResources(libmolsketch_TS_resource "libmolsketch_translations")
qt_add_resources(libmolsketch_SRCS ${libmolsketch_RCS} ${libmolsketch_TS_resource})
qt_wrap_ui(libmolsketch_UIS_H ${libmolsketch_UIS})

# Create the molsKetch libraries
link_directories(${CMAKE_BINARY_DIR}/lib)
if(MSK_QT6)
  if(MSK_STATIC_LIB)
    qt_add_library(libmolsketch STATIC ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  else(MSK_STATIC_LIB)
    qt_add_library(libmolsketch SHARED ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  endif(MSK_STATIC_LIB)
  foreach(package ${QtRequiredPackages})
    target_link_libraries(libmolsketch Qt6::${package})
  endforeach(package)
else(MSK_QT6)
  if(MSK_STATIC_LIB)
    add_library(libmolsketch STATIC ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  else(MSK_STATIC_LIB)
    add_library(libmolsketch SHARED ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  endif(MSK_STATIC_LIB)
  target_link_libraries(libmolsketch ${package})
  target_link_libraries(libmolsketch ${QT_LIBRARIES})
endif(MSK_QT6)

# Install the executable and the library
install(TARGETS libmolsketch
    RUNTIME DESTINATION ${MSK_INSTALL_BINS}
    LIBRARY DESTINATION ${MSK_INSTALL_LIBS}
    ARCHIVE DESTINATION ${MSK_INSTALL_LIBS}/static)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${MSK_INSTALL_INCLUDES} FILES_MATCHING PATTERN "*.h" PATTERN "tools" EXCLUDE)
