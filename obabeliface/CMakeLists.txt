# CMakeLists.txt for the sourcecode of auxiliary interface to OpenBabel
cmake_policy(SET CMP0003 NEW)

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/libmolsketch)
set(obabeliface_HDRS obabeliface.h)
set(obabeliface_SRCS obabeliface.cpp)

include(findOpenBabel.cmake)

# Ensure the Molsketch include directory is always first
get_directory_property(tmp_include_dirs INCLUDE_DIRECTORIES)
set_directory_properties(PROPERTIES INCLUDE_DIRECTORIES
        "${CMAKE_CURRENT_BINARY_DIR}/include;${tmp_include_dirs}")
include_directories(${OPENBABEL_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_BINARY_DIR})

# Create the obabeliface library
add_library(obabeliface_LIB SHARED ${obabeliface_SRCS})
set_target_properties(obabeliface_LIB PROPERTIES OUTPUT_NAME "obabeliface")
set_property(TARGET obabeliface_LIB PROPERTY CXX_STANDARD 14)
if(WIN32 OR WIN64)
        set_target_properties(obabeliface_LIB PROPERTIES PREFIX "")
endif(WIN32 OR WIN64)

target_link_libraries(obabeliface_LIB molsketch_LIB ${QT_LIBRARIES} ${OPENBABEL_LINK_LIBRARIES})

# Install the executable and the library
install(TARGETS obabeliface_LIB
        RUNTIME DESTINATION ${MSK_INSTALL_BINS}
        LIBRARY DESTINATION ${MSK_INSTALL_LIBS}
        ARCHIVE DESTINATION ${MSK_INSTALL_LIBS}/static)
