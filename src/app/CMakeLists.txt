add_executable(cartan
    main.cpp
    MainWindow.cpp
    themes.cpp
    viewport.cpp
)

qt_add_resources(cartan "themes"
    PREFIX "/themes"
    BASE themes
    FILES
        themes/light.json
        themes/dark.json
)

target_link_libraries(cartan PRIVATE
    cartan_engine
    cartan_io
    cartan_render
    Qt6::Widgets
    Qt6::OpenGLWidgets
)
