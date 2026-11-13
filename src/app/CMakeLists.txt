add_executable(cartan
    main.cpp
    MainWindow.cpp
)

target_link_libraries(cartan PRIVATE cartan_engine Qt6::Widgets)
