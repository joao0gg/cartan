add_library(cartan_app STATIC
    Scene.cpp
)

target_link_libraries(cartan_app PUBLIC cartan_core)
