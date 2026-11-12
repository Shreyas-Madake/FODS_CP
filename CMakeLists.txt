cmake_minimum_required(VERSION 3.16)
project(RouteX VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE STREQUAL "Release")
    if(MSVC)
        add_compile_options(/W4)
    else()
        add_compile_options(-Wall -Wextra -Wpedantic)
    endif()
endif()

# Version number removed to prevent CMake strict-matching errors
find_package(SFML COMPONENTS graphics window system REQUIRED)

set(SOURCES
    main.cpp
    src/core/Graph.cpp
    src/core/CSVParser.cpp
    src/algorithms/Dijkstra.cpp
    src/algorithms/AStar.cpp
    src/algorithms/BFS.cpp
    src/ui/Visualizer.cpp
    src/ui/InfoPanel.cpp
    src/ui/Animator.cpp
)

add_executable(RouteX ${SOURCES})
target_include_directories(RouteX PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(RouteX PRIVATE sfml-graphics sfml-window sfml-system)

file(COPY ${CMAKE_SOURCE_DIR}/data DESTINATION ${CMAKE_BINARY_DIR})
file(COPY ${CMAKE_SOURCE_DIR}/assets DESTINATION ${CMAKE_BINARY_DIR})

install(TARGETS RouteX DESTINATION bin)
install(DIRECTORY data/ DESTINATION bin/data)
install(DIRECTORY assets/ DESTINATION bin/assets)