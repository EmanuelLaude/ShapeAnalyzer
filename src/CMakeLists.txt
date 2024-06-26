cmake_minimum_required(VERSION 2.8)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

PROJECT(ShapeAnalyzer)

SET(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")

set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(VTK REQUIRED)
include(${VTK_USE_FILE})

if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  # Instruct CMake to run moc automatically when needed.
  set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTOMOC_RELAXED_MODE TRUE)
  find_package(Qt5Widgets REQUIRED QUIET)
else()
  find_package(Qt4 REQUIRED)
  include(${QT_USE_FILE})
endif()

find_package(PETSc REQUIRED)
find_package(SLEPc REQUIRED)


include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${VTK_USE_FILE} ${SLEPC_INCLUDES} ${PETSC_INCLUDES})

file(GLOB_RECURSE UI_FILES *.ui)
file(GLOB_RECURSE QT_AUTO_GEN (ui_*.h) | (moc*) | (*automoc) )

file(GLOB DOMAIN_METRIC_3RDPARTY_GEODESIC domain/metric/3rdparty/geodesic/*.* )
file(GLOB DOMAIN_METRIC domain/metric/*.* )
file(GLOB DOMAIN_SAMPLINGS domain/samplings/*.* )
file(GLOB DOMAIN_CORRESPONDENCES domain/correspondences/*.* )
file(GLOB DOMAIN_ATTRIBUTES domain/attributes/*.* )
file(GLOB DOMAIN_COLORING domain/coloring/*.* )
file(GLOB DOMAIN_SEGMENTATION domain/segmentation/*.* )
file(GLOB DOMAIN_LAPLACEBELTRAMI domain/laplaceBeltrami/*.* )
file(GLOB DOMAIN_SIGNATURES domain/signatures/* )
file(GLOB DOMAIN_TETGEN domain/3rdparty/tetgen/* )
file(GLOB DOMAIN domain/* )

file(GLOB IO io/*.* )

file(GLOB UTIL util/*.*)

file(GLOB VIEW view/*.* )

file(GLOB CUSTOM custom/*.* )
file(GLOB CUSTOM_TABS custom/tabs/*.* )
file(GLOB CUSTOM_CONTEXTMENUITEMS custom/contextMenuItems/*.* )



set(HEADERS_HAVING_Q_OBJECT)
foreach(header ${QT_WRAP})
  file(STRINGS "${header}" lines REGEX "Q_OBJECT")
  if(lines)
    list(APPEND HEADERS_HAVING_Q_OBJECT "${header}")
  endif()
endforeach()



if(${VTK_VERSION} VERSION_GREATER "6" AND VTK_QT_VERSION VERSION_GREATER "4")
  qt5_wrap_ui(UISrcs ${UI_FILES} )
# qt5_wrap_cpp(ShapeAnalyzerMocs ${HEADERS_HAVING_Q_OBJECT})
# CMAKE_AUTOMOC in ON so the MocHdrs will be automatically wrapped.
add_executable(ShapeAnalyzer MACOSX_BUNDLE ${UISrcs} ${DOMAIN_METRIC_3RDPARTY_GEODESIC} ${DOMAIN_METRIC} ${DOMAIN_SAMPLINGS} ${DOMAIN_CORRESPONDENCES} ${DOMAIN_ATTRIBUTES} ${DOMAIN_COLORING} ${DOMAIN_LAPLACEBELTRAMI} ${DOMAIN_SEGMENTATION} ${DOMAIN_SIGNATURES} ${DOMAIN_TETGEN} ${DOMAIN} ${IO} ${VIEW} ${UTIL} ${CUSTOM} ${CUSTOM_TABS} ${CUSTOM_CONTEXTMENUITEMS})
  qt5_use_modules(ShapeAnalyzer Core Gui)
  target_link_libraries(ShapeAnalyzer ${VTK_LIBRARIES} ${PETSC_LIBRARIES} ${SLEPC_LIBRARIES})
else()
  # might not work
  QT4_WRAP_UI(UISrcs ${UI_FILES})
  QT4_WRAP_CPP(MOCSrcs ${QT_WRAP})
  add_executable(ShapeAnalyzer MACOSX_BUNDLE ${UISrcs} ${DOMAIN_METRIC_3RDPARTY_GEODESIC} ${DOMAIN_METRIC} ${DOMAIN_SAMPLINGS} ${DOMAIN_CORRESPONDENCES} ${DOMAIN_ATTRIBUTES} ${DOMAIN_COLORING} ${DOMAIN_LAPLACEBELTRAMI} ${DOMAIN_SEGMENTATION} ${DOMAIN_SIGNATURES} ${DOMAIN} ${IO} ${VIEW} ${UTIL} ${CUSTOM} ${CUSTOM_TABS} ${CUSTOM_CONTEXTMENUITEMS})

  if(VTK_LIBRARIES)
    if(${VTK_VERSION} VERSION_LESS "6")
      target_link_libraries(ShapeAnalyzer ${PETSC_LIBRARIES} ${SLEPC_LIBRARIES} ${VTK_LIBRARIES} QVTK)
    else()
      target_link_libraries(ShapeAnalyzer ${PETSC_LIBRARIES} ${SLEPC_LIBRARIES} ${VTK_LIBRARIES})
    endif()
  else()
    target_link_libraries(ShapeAnalyzer ${PETSC_LIBRARIES} ${SLEPC_LIBRARIES} vtkHybrid QVTK vtkViews ${QT_LIBRARIES})
  endif()
endif()



source_group("qt_auto_generated" FILES ${QT_AUTO_GEN})

source_group("domain\\metric\\3rdparty\\geodesic" FILES ${DOMAIN_METRIC_3RDPARTY_GEODESIC})
source_group("domain\\metric" FILES ${DOMAIN_METRIC})
source_group("domain\\samplings" FILES ${DOMAIN_SAMPLINGS})
source_group("domain\\correspondences" FILES ${DOMAIN_CORRESPONDENCES})
source_group("domain\\attributes" FILES ${DOMAIN_ATTRIBUTES})
source_group("domain\\coloring" FILES ${DOMAIN_COLORING})
source_group("domain\\segmentation" FILES ${DOMAIN_SEGMENTATION})
source_group("domain\\signatures" FILES ${DOMAIN_SIGNATURES})
source_group("domain\\laplaceBeltrami" FILES ${DOMAIN_LAPLACEBELTRAMI})
source_group("domain\\3rdparty\\tetgen" FILES ${DOMAIN_TETGEN})
source_group("domain" FILES ${DOMAIN})

source_group("io" FILES ${IO})

source_group("view" FILES ${VIEW})

source_group("util" FILES ${UTIL})


source_group("custom" FILES ${CUSTOM})
source_group("custom\\tabs" FILES ${CUSTOM_TABS})
source_group("custom\\contextMenuItems" FILES ${CUSTOM_CONTEXTMENUITEMS})
