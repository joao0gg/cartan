add_executable(cartan
    main.cpp
    MainWindow.cpp
    viewport.cpp
)

target_link_libraries(cartan PRIVATE
    cartan_engine
    cartan_render
    Qt6::Widgets
    Qt6::OpenGLWidgets
)
