cmake_minimum_required(VERSION 3.20)
project(cartan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core OpenGL Widgets OpenGLWidgets)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)

add_subdirectory(src/core)
add_subdirectory(src/io)
add_subdirectory(src/render)
add_subdirectory(src/shell)

add_executable(cartan src/main.cpp)
target_link_libraries(cartan PRIVATE cartan_shell)

function(cartan_module name)
  cmake_parse_arguments(MODULE "" "" "SOURCES;LIBRARIES" ${ARGN})

  add_library(cartan_module_${name} OBJECT ${MODULE_SOURCES})
  target_link_libraries(cartan_module_${name} PRIVATE cartan_shell ${MODULE_LIBRARIES})
  target_link_libraries(cartan PRIVATE cartan_module_${name})
endfunction()

file(GLOB CARTAN_MODULES CONFIGURE_DEPENDS ${PROJECT_SOURCE_DIR}/src/modules/*/CMakeLists.txt)

foreach(module ${CARTAN_MODULES})
  get_filename_component(directory ${module} DIRECTORY)
  add_subdirectory(${directory})
endforeach()
