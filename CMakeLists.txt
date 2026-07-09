cmake_minimum_required(VERSION 3.16)
project(FasdQuery LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent Svg)

qt_standard_project_setup()

qt_add_executable(FasdQuery
    main.cpp
    MainWindow.cpp
    MainWindow.h
    FileScanner.cpp
    FileScanner.h
    FileEntry.h
    FileTableModel.cpp
    FileTableModel.h
    CategoryFilter.h
    CategoryFilter.cpp
    icons.qrc
)

target_link_libraries(FasdQuery
    PRIVATE
        Qt6::Widgets
        Qt6::Concurrent
        Qt6::Svg
)

set_target_properties(FasdQuery PROPERTIES
    WIN32_EXECUTABLE OFF
    MACOSX_BUNDLE OFF
)

install(TARGETS FasdQuery DESTINATION bin)
