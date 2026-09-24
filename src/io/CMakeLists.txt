add_library(cartan_io STATIC
    SurfaceIO.cpp
)

target_link_libraries(cartan_io PUBLIC cartan_core PRIVATE assimp::assimp)
