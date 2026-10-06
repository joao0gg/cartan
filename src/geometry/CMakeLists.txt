add_library(cartan_geometry STATIC
    Metric.cpp
)

target_link_libraries(cartan_geometry PUBLIC cartan_core)
