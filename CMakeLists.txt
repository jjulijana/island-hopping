cmake_minimum_required(VERSION 3.16)
project(island_hopping LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(CGAL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_executable(island-hopping
    src/main.cpp
    src/mainwindow.cpp
    src/canvas.cpp
    src/solver.cpp
    include/canvas.h
    include/mainwindow.h
)

target_include_directories(island-hopping PRIVATE include)
target_link_libraries(island-hopping PRIVATE CGAL::CGAL Qt6::Widgets)
