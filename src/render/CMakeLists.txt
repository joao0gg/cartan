find_package(glm REQUIRED)

set(SHADERS
    grid.vert
    grid.frag
    surfacemesh.vert
    surfacemesh.frag
)

foreach(shader ${SHADERS})
  set(input ${CMAKE_CURRENT_SOURCE_DIR}/shaders/${shader})
  set(output ${CMAKE_CURRENT_BINARY_DIR}/shaders/${shader}.h)

  add_custom_command(
      OUTPUT ${output}
      COMMAND ${CMAKE_COMMAND} -DINPUT=${input} -DOUTPUT=${output} -P ${PROJECT_SOURCE_DIR}/cmake/embed.cmake
      DEPENDS ${input} ${PROJECT_SOURCE_DIR}/cmake/embed.cmake
  )

  list(APPEND SHADER_HEADERS ${output})
endforeach()

add_library(cartan_render STATIC
    renderer.cpp
    grid.cpp
    gpumesh.cpp
    gl/buffer.cpp
    gl/program.cpp
    gl/vertexarray.cpp
    scene/scene.cpp
    scene/structure.cpp
    scene/surfacemesh.cpp
    camera/camera.cpp
    camera/orbitcontroller.cpp
    ${SHADER_HEADERS}
)

target_include_directories(cartan_render
    PUBLIC ${PROJECT_SOURCE_DIR}/src
    PRIVATE ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(cartan_render
    PUBLIC glm::glm
    PRIVATE glad
)
