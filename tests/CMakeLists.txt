function(cartan_test name)
  cmake_parse_arguments(TEST "" "" "SOURCES;LIBRARIES" ${ARGN})

  add_executable(${name} ${TEST_SOURCES})
  target_link_libraries(${name} PRIVATE ${TEST_LIBRARIES})
  add_test(NAME ${name} COMMAND ${name})
endfunction()

cartan_test(cartan_mesh_tests SOURCES core/MeshTests.cpp LIBRARIES cartan_core)
cartan_test(cartan_complex_tests SOURCES core/SimplicialComplexTests.cpp LIBRARIES cartan_core)
cartan_test(cartan_io_tests SOURCES io/IoTests.cpp LIBRARIES cartan_io)
