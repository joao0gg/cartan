add_library(cartan_core STATIC
    Mesh.cpp
    Scene.cpp
)

target_include_directories(cartan_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(cartan_core PUBLIC Qt6::Core glm::glm)
