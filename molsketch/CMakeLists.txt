# CMakeLists.txt for the main sourcecode of molsKetch
cmake_policy(SET CMP0003 NEW)

# set(QT_USE_QTSVG TRUE)

# Ensure the molsketch include directory is always first
# get_directory_property(tmp_include_dirs INCLUDE_DIRECTORIES)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/libmolsketch
                    ${CMAKE_SOURCE_DIR}/obabeliface
                    ${CMAKE_SOURCE_DIR}/libmskcore
)

file(GLOB_RECURSE molsketch_SRCS *.cpp)

include("../generateTranslations.cmake")
addTranslationResources(molsketch_TS_resource "molsketch_translations")
file(GLOB_RECURSE molsketch_RCS *.qrc)
qt_add_resources(molsketch_SRCS ${molsketch_RCS} ${molsketch_TS_resource})

file(GLOB_RECURSE molsketch_UIS *.ui)
qt_wrap_ui(molsketch_UIS_H ${molsketch_UIS})

# Add a program icon for windows
# if(WIN32) ## TODO check
#   set(molsketch_SRCS ${molsketch_SRCS} icon.rc)
# endif(WIN32)

# Create the main executable
add_definitions( -fexceptions )
# link_directories(${CMAKE_BINARY_DIR}/lib)
set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/images/molsketch.ico")
file(GLOB_RECURSE molsketch_MOC_HDRS *.h)
if(MSK_QT6)
  qt_add_executable(molsketch ${molsketch_SRCS} ${molsketch_UIS_H} ${molsketch_MOC_HDRS} ${APP_ICON_RESOURCE_WINDOWS})
  target_link_libraries(molsketch PRIVATE ${QT_LIBRARIES})
  # foreach(package ${QtRequiredPackages})
  #   target_link_libraries(molsketch PRIVATE Qt6::${package})
  # endforeach(package)
else(MSK_QT6)
  add_executable(molsketch ${molsketch_SRCS} ${molsketch_UIS_H} ${molsketch_MOC_HDRS} ${APP_ICON_RESOURCE_WINDOWS})
  target_link_libraries(molsketch PRIVATE
        Qt5::Core
        Qt5::Widgets
        Qt5::Gui
        Qt5::PrintSupport
        Qt5::Svg
        Qt5::Network)
  # foreach(package ${QtRequiredPackages})
  #   target_link_libraries(molsketch PRIVATE $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::${package})
  # endforeach(package)
endif(MSK_QT6)
target_link_libraries(molsketch PRIVATE libmolsketch)
target_link_libraries(molsketch PRIVATE mskcore)

install(TARGETS molsketch DESTINATION ${MSK_INSTALL_BINS})
set_property(TARGET molsketch PROPERTY INSTALL_RPATH ${MSK_INSTALL_LIBS})
set_target_properties(molsketch PROPERTIES OUTPUT_NAME "molsketch")

# Install menu entries on Linux
if(UNIX)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/images/molsketch.svg DESTINATION share/icons/hicolor/scalable/apps)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/images/molsketch.xpm DESTINATION share/pixmaps)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/images/application-x-molsketch.svg DESTINATION share/icons/hicolor/scalable/mimetypes)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/images/application-x-molsketch-molecule.svg DESTINATION share/icons/hicolor/scalable/mimetypes)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/molsketch.xml DESTINATION share/mime/packages)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/molsketch.desktop DESTINATION share/applications)
  install(FILES ${PROJECT_SOURCE_DIR}/molsketch/net.sourceforge.molsketch.appdata.xml DESTINATION share/metainfo)
endif(UNIX)
