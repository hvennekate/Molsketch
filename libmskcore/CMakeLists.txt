file(GLOB_RECURSE libmskcore_HDRS *.h)
file(GLOB_RECURSE libmskcore_SRCS *.cpp)

if(MSK_STATIC_LIB)
  add_library(mskcore STATIC ${libmskcore_SRCS} ${libmskcore_HDRS})
else(MSK_STATIC_LIB)
  add_library(mskcore SHARED ${libmskcore_SRCS} ${libmskcore_HDRS})
endif(MSK_STATIC_LIB)

set_target_properties(mskcore PROPERTIES
  AUTOMOC FALSE
  AUTOUIC FALSE
  AUTORCC FALSE
  )

install(TARGETS mskcore
    RUNTIME DESTINATION ${MSK_INSTALL_BINS}
    LIBRARY DESTINATION ${MSK_INSTALL_LIBS}
    ARCHIVE DESTINATION ${MSK_INSTALL_LIBS}/static)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} DESTINATION ${MSK_INSTALL_INCLUDES} FILES_MATCHING PATTERN "*.h" PATTERN)
