# Find and setup Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(QtRequiredPackages
        Core
        Widgets
        Gui
        PrintSupport
        Svg
        Network
)

if(MSK_QT6)
  set(QT_MIN_VERSION "6.0.0")
  find_package(Qt6 REQUIRED COMPONENTS ${QtRequiredPackages})
  find_package(Qt6 REQUIRED COMPONENTS LinguistTools)
  qt_standard_project_setup()
  foreach(package ${QtRequiredPackages})
    set(QT_LIBRARIES ${QT_LIBRARIES} Qt6::${package})
  endforeach(package)
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

  if(NOT(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND))
          message(FATAL_ERROR "Could not find Qt5. Required parts include: Widgets, Gui, PrintSupport, Svg.")
  endif(NOT(Qt5Core_FOUND AND Qt5Widgets_FOUND AND Qt5Gui_FOUND AND Qt5PrintSupport_FOUND AND Qt5Svg_FOUND))
  set(CMAKE_AUTOMOC ON)
endif(MSK_QT6)
