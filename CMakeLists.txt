cmake_minimum_required(VERSION 3.20)
project(cartan LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets OpenGLWidgets)
find_package(assimp REQUIRED)
find_package(glm REQUIRED)

add_executable(cartan
    src/main.cpp

    src/app/MainWindow.cpp
    src/app/Viewport.cpp

    src/render/Camera.cpp
    src/render/GpuMesh.cpp
    src/render/RenderMesh.cpp
    src/render/Renderer.cpp
    src/render/Shader.cpp

    src/io/ShaderIO.cpp
    src/io/SurfaceIO.cpp
)

target_include_directories(cartan PRIVATE src)
target_compile_definitions(cartan PRIVATE SHADER_DIR="${CMAKE_CURRENT_SOURCE_DIR}/shaders")
target_link_libraries(cartan PRIVATE Qt6::Widgets Qt6::OpenGLWidgets assimp::assimp glm::glm)