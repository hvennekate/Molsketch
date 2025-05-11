cmake_minimum_required(VERSION 3.5.0)
if (MSK_QT6)
  cmake_minimum_required(VERSION 3.21.1)
endif (MSK_QT6)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0003 NEW)
cmake_policy(SET CMP0005 NEW)
cmake_policy(SET CMP0020 NEW)
cmake_policy(SET CMP0071 NEW)

project(Molsketch)

# TODO
# disable AUTOMOC
# extract QT setup into functions
# clean up tests
# check QT 5 and 6 builds
# remvoe debug output
# fix compiler warnings
# check if transitioning to CTest is an option
# windows app icon
# translations
# icons
# licenses
# remove *.pri *.pro setupQt.cmake


# Add support for the "make uninstall" target
configure_file("${CMAKE_SOURCE_DIR}/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)
add_custom_target(uninstall "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake")

set(BUILD_SHARED_LIBS ON)

include(read_buildvars.cmake)
include(include_qt.cmake)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT APPLE )
        set(MSK_INSTALL_LIBS "${MSK_INSTALL_LIBS64}")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 AND NOT APPLE )

set(OBABEL_IFACE_NAME "libobabeliface.so")
if(WIN32 OR WIN64)
        set(OBABEL_IFACE_NAME "obabeliface.dll")
endif(WIN32 OR WIN64)
add_definitions(-DOBABEL_IFACE_LOCATION="${MSK_INSTALL_LIBS}/${OBABEL_IFACE_NAME}")

set(CMAKE_INSTALL_PREFIX ${MSK_PREFIX})

################ Adding subdirs
set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(libmskcore)
add_subdirectory(libmolsketch)
if (MSK_OBABELIFACE)
    add_subdirectory(obabeliface)
endif (MSK_OBABELIFACE)
add_subdirectory(molsketch)
if (MSK_BUILD_TESTS)
    add_subdirectory(tests)
else (MSK_BUILD_TESTS)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif(MSK_BUILD_TESTS)
