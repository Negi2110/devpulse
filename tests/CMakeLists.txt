find_package(GTest REQUIRED)
find_package(Qt5 REQUIRED COMPONENTS Core)

set(CMAKE_AUTOMOC ON)

set(CORE_SOURCES
    ${CMAKE_SOURCE_DIR}/src/core/Service.h
    ${CMAKE_SOURCE_DIR}/src/core/servicerepository.h
    ${CMAKE_SOURCE_DIR}/src/core/servicerepository.cpp
    ${CMAKE_SOURCE_DIR}/src/core/monitoring/latencystore.h
    ${CMAKE_SOURCE_DIR}/src/core/monitoring/latencystore.cpp
    ${CMAKE_SOURCE_DIR}/src/core/monitoring/uptimetracker.h
    ${CMAKE_SOURCE_DIR}/src/core/monitoring/uptimetracker.cpp
)

add_executable(devpulse_tests
    test_service.cpp
    test_latencystore.cpp
    test_uptimetracker.cpp
    test_servicerepository.cpp
    ${CORE_SOURCES}
)

target_include_directories(devpulse_tests PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(devpulse_tests PRIVATE
    GTest::GTest
    GTest::Main
    Qt5::Core
)

include(GoogleTest)
gtest_discover_tests(devpulse_tests)
