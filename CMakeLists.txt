cmake_minimum_required(VERSION 3.20)
project(cartan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(cartan
    src/main.cpp
    src/MainWindow.cpp
    src/MainWindow.h
)

target_link_libraries(cartan PRIVATE Qt6::Widgets)
