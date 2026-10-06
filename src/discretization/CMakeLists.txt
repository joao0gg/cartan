add_library(cartan_discretization STATIC
    HodgeStar.cpp
    Laplacian.cpp
)

target_link_libraries(cartan_discretization PUBLIC cartan_geometry)
