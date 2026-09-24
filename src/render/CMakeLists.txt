add_library(cartan_render STATIC
    Camera.cpp
    GpuMesh.cpp
    Renderer.cpp
    Shader.cpp
)

qt_add_resources(cartan_render "shaders"
    PREFIX "/render"
    FILES
        shaders/surface.vert
        shaders/surface.frag
)

target_link_libraries(cartan_render PUBLIC cartan_core Qt6::OpenGL)
