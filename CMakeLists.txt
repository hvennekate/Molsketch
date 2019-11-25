## CMakeLists.txt for Molsketch
cmake_minimum_required(VERSION 2.6.0) # Depend on CMake 2.6 or better
cmake_policy(SET CMP0005 OLD)
cmake_policy(SET CMP0020 OLD)

# The name of the project
project(Molsketch)

# Include the extra modules (OpenBabel/uninstall)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

###########################################################
# Here I am adding GCC magic from KDE. With these settings#
# compiler issues are fixed and more warnings are issued. #
###########################################################
if (CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wchar-subscripts")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute -fno-common")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-check-new -fno-common")
  add_definitions(-D_BSD_SOURCE)
endif (CMAKE_COMPILER_IS_GNUCXX)

# Enable CPack
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_SOURCE_PACKAGE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES "/build;/.git")
include(CPack)

# Add support for the "make uninstall" target
configure_file("${CMAKE_SOURCE_DIR}/cmake/modules/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

# Enabling debugging symbols
set(CMAKE_BUILD_TYPE Debug)

# Enabling shared libraries
set(BUILD_SHARED_LIBS ON)

# Setting paths
set(_obIncDir ${CMAKE_SOURCE_DIR}/include)
set(_obLinkDir ${CMAKE_SOURCE_DIR}/lib)

# Find and setup Qt4 or Qt5
set(QT_MIN_VERSION "4.5.0") # we need at least version 4.5 of Qt
if(NOT FORCEQt4)

#	CMAKE_MINIMUM_REQUIRED_VERSION(VERSION 2.8.11)
        set(CMAKE_INCLUDE_CURRENT_DIR ON)
        set(Qt5RequiredPackages
                Core
                Widgets
                Gui
                PrintSupport
                Svg
                Network
        )

        foreach(package ${Qt5RequiredPackages})
                find_package(Qt5${package})
                INCLUDE_DIRECTORIES(${Qt5${package}_INCLUDE_DIRS})
                add_definitions(${Qt5${package}_DEFINITIONS})
                set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5${package}_EXECUTABLE_COMPILE_FLAGS}")
                set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::${package})
        endforeach(package)

endif(NOT FORCEQt4)
# if Qt5 is not available, try Qt4
if(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND)
        message(STATUS "Using Qt5. Use option \"-D FORCEQt4=TRUE\" to switch to Qt4.")
        set(CMAKE_AUTOMOC ON)
else(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND)
        find_package(Qt4)
        if(NOT Qt4_FOUND)
                message(FATAL_ERROR "Could not find Qt4 or Qt5.")
        endif(NOT Qt4_FOUND)
        if(NOT FORCEQt4)
                message(STATUS "Could not find Qt5.")
        endif(NOT FORCEQt4)
        set(FORCEQt4 TRUE)
        message(STATUS "Using Qt4.")
endif(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND)


################ Reading installation settings

message(STATUS "======= Welcome to Molsketch build =======")
message(STATUS "Sources in: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "Installation settings and corresponding cmake variables (use \"-D <variableName>=<value>\" to change):")

file(STRINGS buildvariables BUILD_VARIABLES)
list(APPEND BUILD_VARIABLES "\"Build static application\"\;MSK_STATIC_LIB\;false\;false")
foreach(buildvar ${BUILD_VARIABLES})
        list(GET buildvar 0 descriptor)
        list(GET buildvar 1 variableName)
        list(GET buildvar 2 defaultValue)
        list(GET buildvar 3 winDefaultValue)
        if(NOT DEFINED ${variableName})
                set(${variableName} "${defaultValue}")
                if(WIN32 OR WIN64)
                        set(${variableName} "${winDefaultValue}")
                endif(WIN32 OR WIN64)
        endif(NOT DEFINED ${variableName})
        if(variableName MATCHES ".*INSTALL.*")
                set(${variableName} "${MSK_PREFIX}${${variableName}}")
        endif(variableName MATCHES ".*INSTALL.*")
        message(STATUS "    ${descriptor}: ${variableName} = ${${variableName}}")
        add_definitions(-D${variableName}="\\\"${${variableName}}\\\"")
endforeach(buildvar BUILD_VARIABLES)
if( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT APPLE )
        set(MSK_INSTALL_LIBS "${MSK_INSTALL_LIBS64}")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT APPLE )

if(NOT FORCEQt4)
        set(QTVERSIONSUFFIX "-qt5")
endif(NOT FORCEQt4)

set(OBABEL_IFACE_NAME "libobabeliface${QTVERSIONSUFFIX}.so")

if(WIN32 OR WIN64)
        set(OBABEL_IFACE_NAME "obabeliface${QTVERSIONSUFFIX}.dll")
endif(WIN32 OR WIN64)

add_definitions(-DOBABEL_IFACE_LOCATION="\\\"${MSK_INSTALL_LIBS}/${OBABEL_IFACE_NAME}\\\"")

set(CMAKE_INSTALL_PREFIX ${MSK_PREFIX})

################ Adding subdirs
add_subdirectory(libmolsketch)
add_subdirectory(obabeliface)
add_subdirectory(molsketch)
