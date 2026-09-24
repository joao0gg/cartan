add_library(cartan_shell STATIC
    MainWindow.cpp
    Module.cpp
    Workbench.cpp
)

target_link_libraries(cartan_shell PUBLIC cartan_core Qt6::Widgets)
