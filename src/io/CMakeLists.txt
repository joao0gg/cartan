find_package(glm REQUIRED)

add_library(cartan_io STATIC
    obj.cpp
)

target_include_directories(cartan_io PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(cartan_io PUBLIC glm::glm)
