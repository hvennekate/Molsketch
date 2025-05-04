# TODO extract header together with main build script


# cmake_policy(SET CMP0005 NEW)
# cmake_policy(SET CMP0020 NEW)
# cmake_policy(SET CMP0171 NEW)
# cmake_policy(SET CMP0071 OLD)


###########################################################
# Here I am adding GCC magic from KDE. With these settings#
# compiler issues are fixed and more warnings are issued. #
###########################################################
# if (CMAKE_COMPILER_IS_GNUCXX)
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-long-long -std=iso9899:1990 -Wundef -Wcast-align")
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror-implicit-function-declaration -Wchar-subscripts")
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -W -Wpointer-arith -Wwrite-strings -Wformat-security")
#   set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-format-attribute -fno-common")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wnon-virtual-dtor -Wno-long-long -ansi -Wundef -Wcast-align")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wchar-subscripts -Wall -W -Wpointer-arith -Wformat-security")
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fno-check-new -fno-common")
#   add_definitions(-D_BSD_SOURCE)
# endif (CMAKE_COMPILER_IS_GNUCXX)

# set(CMAKE_BUILD_TYPE Debug) # TODO add flag

# Enabling shared libraries
# set(BUILD_SHARED_LIBS ON)

# Setting paths
# set(_obIncDir ${CMAKE_SOURCE_DIR}/include)
# set(_obLinkDir ${CMAKE_SOURCE_DIR}/lib)

# Find and setup Qt
# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# set(QtRequiredPackages
#         Core
#         Widgets
#         Gui
#         PrintSupport
#         Svg
#         Network
#         Test
# )

if(MSK_QT6)
  # set(QT_MIN_VERSION "6.0.0")
  # find_package(Qt6 REQUIRED COMPONENTS ${QtRequiredPackages})
  # find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
  # qt_standard_project_setup()
else(MSK_QT6)
  # set(QT_MIN_VERSION "5.0.0")
  # foreach(package ${QtRequiredPackages})
  #         find_package(Qt5${package})
  #         INCLUDE_DIRECTORIES(${Qt5${package}_INCLUDE_DIRS})
  #         add_definitions(${Qt5${package}_DEFINITIONS})
  #         set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5${package}_EXECUTABLE_COMPILE_FLAGS}")
  #         set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::${package})
  # endforeach(package)
  # find_package(Qt5LinguistTools)
  #
  # if(NOT(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND))
  #         message(FATAL_ERROR "Could not find Qt5. Required parts include: Widgets, Gui, PrintSupport, Svg.")
  # endif(NOT(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND))
  # set(CMAKE_AUTOMOC ON)
endif(MSK_QT6)

#####

file(GLOB_RECURSE tests_HDRS *.h)
file(GLOB_RECURSE tests_SRCS *.cpp)
# file(GLOB_RECURSE obabeliface_HDRS ../obabeliface/*.h)
file(GLOB_RECURSE maincode_HDRS ../molsketch/*.h)
# list(REMOVE_ITEM maincode_HDRS ${tests_HDRS})
# list(REMOVE_ITEM maincode_HDRS ${obabeliface_HDRS})

file(GLOB_RECURSE maincode_SRCS #../libmolsketch/*.cpp
                                ../molsketch/*.cpp
                                # ../obabeliface/*.cpp
                                # ../libmskcore/*.cpp
                                )
file(GLOB mainfile ../molsketch/main.cpp)
list(REMOVE_ITEM maincode_SRCS ${mainfile})

file(GLOB_RECURSE maincode_UIS ../molsketch/*.ui)
qt_wrap_ui(maincode_UIS_H ${maincode_UIS})

include_directories( ./
                     /usr/include/boost/stacktrace
                     ../libmolsketch
                     ../molsketch
                     ../obabeliface
                     ../libmskcore
                     ${CXXTEST_PATH}/)

add_definitions(-DOBABEL_IFACE_LOCATION="../obabeliface")
include(../obabeliface/findOpenBabel.cmake)
include_directories(SYSTEM ${OPENBABEL_INCLUDE_DIRS} ${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Wall -fprofile-arcs -ftest-coverage -O0")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -g -Wall -fprofile-arcs -ftest-coverage -O0")
add_library(maincode SHARED ${maincode_SRCS} ${maincode_UIS_H}) # ${libmolsketch_QM})
target_link_libraries(maincode PRIVATE ${QT_LIBRARIES})


##### Test code
if (NOT DEFINED CXXTEST_PATH)
    message(FATAL_ERROR "Specify CXXTEST_PATH to cxx test library")
endif(NOT DEFINED CXXTEST_PATH)
add_definitions( -fexceptions )

file (GLOB_RECURSE tests *test.h)
foreach(test_h IN LISTS tests)
    get_filename_component(test_raw "${test_h}" NAME_WLE)
    message(STATUS "current: ${test_h} ${test_raw}")
    set(test_cpp "${test_raw}.cpp")
    add_custom_command(
        COMMAND ${CXXTEST_PATH}/bin/cxxtestgen --have-eh --xunit-printer --part "${test_h}" -o "${test_cpp}"
        DEPENDS "${test_h}"
        OUTPUT "${test_cpp}"
        )
    list(APPEND tests_cpp "${test_cpp}")
endforeach()
message(STATUS "generated: ${tests_cpp}")
file(GLOB_RECURSE testcode_UIS ../tests/*.ui)
qt_wrap_ui(testcode_UIS_H ${testcode_UIS})
add_library(testcode SHARED ${tests_SRCS} ${tests_cpp} ${testcode_UIS_H})

file(GLOB runner_template ../tests/runnerTemplate.tpl)
message(STATUS "Runner template: ${runner_template}")
add_custom_command(
    COMMAND ${CXXTEST_PATH}/bin/cxxtestgen --have-eh --xunit-printer --root -o cxxrunner.cpp --template "${runner_template}"
    DEPENDS "${runner_template}"
    OUTPUT "cxxrunner.cpp"
    )
# file(GLOB_RECURSE testcode_SRCS *.cpp)
# file(GLOB_RECURSE testcode_HDRS *.h)
# list(REMOVE_ITEM testcode_HDRS ${tests_HDRS})

if(MSK_QT6)
  qt_add_executable(TestRunner cxxrunner.cpp ${testcode_SRCS} )
  target_link_libraries(testcode PRIVATE Qt6::Test)
  target_link_libraries(TestRunner PRIVATE Qt6::Test)
else(MSK_QT6)
  add_executable(TestRunner cxxrunner.cpp ${testcode_SRCS})
  find_package(Qt5Test)
  target_link_libraries(TestRunner PRIVATE
      # $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::Core
      # $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::Widgets
      $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::Test
      # $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::Svg
      # $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::Network
      # $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::PrintSupport
      )
  target_link_libraries(testcode PRIVATE $<IF:$<BOOL:${MSK_QT6}>,Qt6,Qt5>::Test)
endif(MSK_QT6)
target_link_libraries(testcode PRIVATE ${QT_LIBRARIES})
target_link_libraries(TestRunner PRIVATE ${QT_LIBRARIES})
target_link_libraries(TestRunner PRIVATE libmolsketch mskcore maincode testcode obabeliface_LIB ${OPENBABEL_LINK_LIBRARIES} )

set_target_properties(TestRunner PROPERTIES
  AUTOMOC TRUE
  AUTOUIC TRUE
  AUTORCC TRUE
  )
