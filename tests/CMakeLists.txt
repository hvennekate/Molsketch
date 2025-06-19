
file(GLOB_RECURSE tests_HDRS *.h)
file(GLOB_RECURSE tests_SRCS *.cpp)
file(GLOB_RECURSE maincode_HDRS ../molsketch/*.h)
file(GLOB_RECURSE maincode_SRCS ../molsketch/*.cpp)
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

if (NOT DEFINED CXXTEST_PATH)
    message(FATAL_ERROR "Specify CXXTEST_PATH to cxx test library")
endif(NOT DEFINED CXXTEST_PATH)
add_definitions( -fexceptions )

file (GLOB_RECURSE tests *test.h)
foreach(test_h IN LISTS tests)
    get_filename_component(test_raw "${test_h}" NAME_WLE)
    set(test_cpp "${test_raw}.cpp")
    add_custom_command(
        COMMAND ${CXXTEST_PATH}/bin/cxxtestgen --have-eh --xunit-printer --part "${test_h}" -o "${test_cpp}"
        DEPENDS "${test_h}"
        OUTPUT "${test_cpp}"
        )
    list(APPEND tests_cpp "${test_cpp}")
endforeach()
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

if(MSK_QT6)
  qt_add_executable(TestRunner cxxrunner.cpp ${testcode_SRCS} )
  target_link_libraries(testcode PRIVATE Qt6::Test)
  target_link_libraries(TestRunner PRIVATE Qt6::Test)
else(MSK_QT6)
  add_executable(TestRunner cxxrunner.cpp ${testcode_SRCS})
  find_package(Qt5Test)
  target_link_libraries(TestRunner PRIVATE Qt5::Test)
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
