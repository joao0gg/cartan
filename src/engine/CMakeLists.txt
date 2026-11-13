add_library(cartan_engine STATIC
    version.cpp
)

target_include_directories(cartan_engine PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_definitions(cartan_engine PRIVATE CARTAN_VERSION="${PROJECT_VERSION}")
