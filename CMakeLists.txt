cmake_minimum_required(VERSION 3.16)
project(island_hopping LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CGAL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(island-hopping
    main.cpp
    mainwindow.cpp
    canvas.cpp
    solver.cpp
)

target_include_directories(island-hopping PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(island-hopping PRIVATE CGAL::CGAL Qt6::Widgets)
