add_library(cartan_solvers STATIC
    LinearSolver.cpp
)

target_link_libraries(cartan_solvers PUBLIC cartan_core)
