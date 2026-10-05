add_executable(cartan_core_tests MeshTests.cpp)
target_link_libraries(cartan_core_tests PRIVATE cartan_core)

add_test(NAME cartan_core_tests COMMAND cartan_core_tests)

add_executable(cartan_io_tests IoTests.cpp)
target_link_libraries(cartan_io_tests PRIVATE cartan_io)

add_test(NAME cartan_io_tests COMMAND cartan_io_tests)
