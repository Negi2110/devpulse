cmake_minimum_required(VERSION 3.16)
project(devpulse LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Widgets Network)

add_executable(devpulse
    src/main.cpp
    src/ui/mainwindow.h
    src/ui/mainwindow.cpp
    src/core/Service.h
    src/core/servicerepository.h
    src/core/servicerepository.cpp
    src/ui/dialogs/addservicedialog.h
    src/ui/dialogs/addservicedialog.cpp
    src/models/servicetablemodel.h
    src/models/servicetablemodel.cpp
    src/core/checkers/IChecker.h
    src/core/checkers/httpchecker.h
    src/core/checkers/httpchecker.cpp
    src/core/checkers/tcpchecker.h
    src/core/checkers/tcpchecker.cpp
    src/core/checkers/checkerfactory.h
    src/core/checkers/checkerfactory.cpp
    src/core/monitorengine.h src/core/monitorengine.cpp
)

target_link_libraries(devpulse PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Network
)

target_include_directories(devpulse PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
