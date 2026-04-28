find_package(Qt5 REQUIRED COMPONENTS Core Network)

set(CMAKE_AUTOMOC ON)

add_library(ping_checker SHARED
    PingCheckerPlugin.h
    PingCheckerPlugin.cpp
)

target_include_directories(ping_checker PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(ping_checker PRIVATE
    Qt5::Core
    Qt5::Network
)

add_custom_command(TARGET ping_checker POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        ${CMAKE_SOURCE_DIR}/plugins/bin
    COMMAND ${CMAKE_COMMAND} -E copy
        $<TARGET_FILE:ping_checker>
        ${CMAKE_SOURCE_DIR}/plugins/bin/
)