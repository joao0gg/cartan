add_library(cartan_core STATIC
    Mesh.cpp
    SimplicialComplex.cpp
)

target_include_directories(cartan_core PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(cartan_core PUBLIC glm::glm Eigen3::Eigen)
