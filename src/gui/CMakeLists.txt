add_library(cartan_gui STATIC
    MainWindow.cpp
    Module.cpp
    Workbench.cpp
)

target_link_libraries(cartan_gui PUBLIC cartan_app Qt6::Widgets)
