add_library(cartan_render STATIC
    renderer.cpp
)

target_include_directories(cartan_render PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(cartan_render PRIVATE glad)
