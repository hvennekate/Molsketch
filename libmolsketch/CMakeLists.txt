# CMakeLists.txt for the main sourcecode of molsKetch's library
cmake_policy(SET CMP0003 NEW)

set(QT_USE_QTSVG TRUE) # TODO check if really required (also in other files)

# Ensure the Molsketch include directory is always first
get_directory_property(tmp_include_dirs INCLUDE_DIRECTORIES)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/obabeliface)

file(GLOB_RECURSE libmolsketch_HDRS *.h)
file(GLOB_RECURSE libmolsketch_SRCS *.cpp)
file(GLOB_RECURSE libmolsketch_UIS *.ui)
file(GLOB_RECURSE libmolsketch_RCS *.qrc)

include("../generateTranslations.cmake")
addTranslationResources(libmolsketch_TS_resource "libmolsketch_translations")
qt5_add_resources(libmolsketch_SRCS ${libmolsketch_RCS} ${libmolsketch_TS_resource})
qt5_wrap_ui(libmolsketch_UIS_H ${libmolsketch_UIS})

# Create the molsKetch libraries
if(MSK_STATIC_LIB)
        add_library(molsketch_LIB STATIC ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
else(MSK_STATIC_LIB)
        add_library(molsketch_LIB SHARED ${libmolsketch_SRCS} ${libmolsketch_UIS_H} ${libmolsketch_QM})
endif(MSK_STATIC_LIB)
set_target_properties(molsketch_LIB PROPERTIES OUTPUT_NAME "molsketch")
set_property(TARGET molsketch_LIB PROPERTY CXX_STANDARD 14)
target_link_libraries(molsketch_LIB ${QT_LIBRARIES})

# Install the executable and the library
install(TARGETS molsketch_LIB
    RUNTIME DESTINATION ${MSK_INSTALL_BINS}
    LIBRARY DESTINATION ${MSK_INSTALL_LIBS}
    ARCHIVE DESTINATION ${MSK_INSTALL_LIBS}/static)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${MSK_INSTALL_INCLUDES} FILES_MATCHING PATTERN "*.h" PATTERN "tools" EXCLUDE)
