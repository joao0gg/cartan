cmake_minimum_required(VERSION 3.20)
project(cartan VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CARTAN_BUILD_GUI "Build Qt application" ON)

add_subdirectory(src/engine)
add_subdirectory(src/cli)

if(CARTAN_BUILD_GUI)
  set(CMAKE_AUTOMOC ON)
  find_package(Qt6 REQUIRED COMPONENTS Widgets)

  add_subdirectory(src/app)
endif()
