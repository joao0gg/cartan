add_library(cartan_io STATIC
    SurfaceIO.cpp
    PlyFormat.cpp
    ObjFormat.cpp
    OffFormat.cpp
    detail/Text.cpp
)

target_link_libraries(cartan_io PUBLIC cartan_core)
