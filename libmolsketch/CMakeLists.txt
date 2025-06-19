cmake_policy(SET CMP0003 NEW)

include_directories(${CMAKE_SOURCE_DIR}/libmskcore)
file(GLOB_RECURSE libmolsketch_SRCS *.cpp)

include("../generateTranslations.cmake")
addTranslationResources(libmolsketch_TS_resource "libmolsketch_translations")
file(GLOB_RECURSE libmolsketch_RCS *.qrc)
qt_add_resources(libmolsketch_SRCS ${libmolsketch_RCS} ${libmolsketch_TS_resource})

file(GLOB_RECURSE libmolsketch_UIS *.ui)
qt_wrap_ui(libmolsketch_UIS_H ${libmolsketch_UIS})

link_directories(${CMAKE_BINARY_DIR}/lib)
if(MSK_QT6)
  message(STATUS "Building Qt6")
  if(MSK_STATIC_LIB)
    qt_add_library(libmolsketch STATIC ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  else(MSK_STATIC_LIB)
    qt_add_library(libmolsketch SHARED ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  endif(MSK_STATIC_LIB)
  target_link_libraries(libmolsketch ${QT_LIBRARIES})
else(MSK_QT6)
  message(STATUS "Building Qt5")
  if(MSK_STATIC_LIB)
    add_library(libmolsketch STATIC ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  else(MSK_STATIC_LIB)
    add_library(libmolsketch SHARED ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
  endif(MSK_STATIC_LIB)
  # target_link_libraries(libmolsketch ${package})
  target_link_libraries(libmolsketch ${QT_LIBRARIES})
endif(MSK_QT6)
set_target_properties(libmolsketch PROPERTIES OUTPUT_NAME "molsketch")

# Install the executable and the library
install(TARGETS libmolsketch
    RUNTIME DESTINATION ${MSK_INSTALL_BINS}
    LIBRARY DESTINATION ${MSK_INSTALL_LIBS}
    ARCHIVE DESTINATION ${MSK_INSTALL_LIBS}/static)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${MSK_INSTALL_INCLUDES} FILES_MATCHING PATTERN "*.h" PATTERN "tools" EXCLUDE)
