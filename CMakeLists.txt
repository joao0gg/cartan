cmake_minimum_required(VERSION 3.20)
project(cartan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(CARTAN_BUILD_GUI "Build the Qt application" ON)

enable_testing()

find_package(glm REQUIRED)
find_package(Eigen3 3.4 REQUIRED NO_MODULE)

add_subdirectory(src/core)
add_subdirectory(src/geometry)
add_subdirectory(src/discretization)
add_subdirectory(src/solvers)
add_subdirectory(src/app)
add_subdirectory(src/io)

if(CARTAN_BUILD_GUI)
  set(CMAKE_AUTOMOC ON)
  find_package(Qt6 REQUIRED COMPONENTS Core OpenGL Widgets OpenGLWidgets)

  add_subdirectory(src/render)
  add_subdirectory(src/gui)

  add_executable(cartan src/main.cpp)
  target_link_libraries(cartan PRIVATE cartan_gui)

  function(cartan_module name)
    cmake_parse_arguments(MODULE "" "" "SOURCES;LIBRARIES" ${ARGN})

    add_library(cartan_module_${name} OBJECT ${MODULE_SOURCES})
    target_link_libraries(cartan_module_${name} PRIVATE cartan_gui ${MODULE_LIBRARIES})
    target_link_libraries(cartan PRIVATE cartan_module_${name})
  endfunction()

  file(GLOB CARTAN_MODULES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/modules/*/CMakeLists.txt)

  foreach(module ${CARTAN_MODULES})
    get_filename_component(directory ${module} DIRECTORY)
    add_subdirectory(${directory})
  endforeach()
endif()

add_subdirectory(tests)
