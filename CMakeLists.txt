cmake_minimum_required(VERSION 3.16)
project(devpulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets)

add_executable(devpulse
    main.cpp
    mainwindow.cpp
    mainwindow.h
    Service.h
)

target_link_libraries(devpulse PRIVATE
    Qt5::Core
    Qt5::Widgets
)

target_include_directories(devpulse PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
