set(QtRequiredPackages
        Core
        Widgets
        Gui
        PrintSupport
        Svg
        Network
)
list(TRANSFORM QtRequiredPackages PREPEND "Qt6::" OUTPUT_VARIABLE QT_LIBRARIES)

if(MSK_QT6)
  set(QT_MIN_VERSION "6.0.0")
  find_package(Qt6 REQUIRED COMPONENTS LinguistTools ${QtRequiredPackages})
  qt_standard_project_setup()
  foreach(package ${QtRequiredPackages})
    set(QT_LIBRARIES ${QT_LIBRARIES} Qt6::${package})
  endforeach(package)

  list(TRANSFORM QtRequiredPackages PREPEND "Qt6::" OUTPUT_VARIABLE QT_LIBRARIES)

  function(setupQt projectName sources isLib)
    if(isLib)
      if(MSK_STATIC_LIB)
        qt_add_library(${projectName} STATIC ${sources})
      else(MSK_STATIC_LIB)
        qt_add_library(${projectName} SHARED ${sources})
      endif(MSK_STATIC_LIB)
    else(isLib)
      add_executable(${projectName} ${sources})
    endif(isLib)
    target_link_libraries(${projectName} ${QT_LIBRARIES})
  endfunction(setupQt)
else(MSK_QT6)
  set(QT_MIN_VERSION "5.0.0")
  foreach(package ${QtRequiredPackages})
          find_package(Qt5${package})
          INCLUDE_DIRECTORIES(SYSTEM ${Qt5${package}_INCLUDE_DIRS})
          add_definitions(${Qt5${package}_DEFINITIONS})
          set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5${package}_EXECUTABLE_COMPILE_FLAGS}")
          set(QT_LIBRARIES ${QT_LIBRARIES} Qt5::${package})
  endforeach(package)
  find_package(Qt5LinguistTools)

  list(TRANSFORM QtRequiredPackages PREPEND "Qt5::" OUTPUT_VARIABLE QT_LIBRARIES)
  set(CMAKE_AUTOMOC ON)

  function(setupQt projectName sources isLib)
    if(isLib)
      if(MSK_STATIC_LIB)
        add_library(${projectName} STATIC ${sources})
      else(MSK_STATIC_LIB)
        add_library(${projectName} SHARED ${sources})
      endif(MSK_STATIC_LIB)
    else(isLib)
      add_executable(${projectName} ${sources})
    endif(isLib)
    set_target_properties(${prjectName} PROPERTIES
      AUTOMOC TRUE
      AUTOUIC TRUE
      AUTORCC TRUE)
    target_link_libraries(${projectName} PRIVATE ${QT_LIBRARIES})
  endfunction(setupQt)
endif(MSK_QT6)
