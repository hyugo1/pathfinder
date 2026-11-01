cmake_minimum_required(VERSION 3.16)
project(pathfinder)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Find SFML and the requested components (Case-sensitive depending on version, e.g., Graphics or graphics)
find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics)

# Build the grid and algorithms as a reusable library.
add_library(pathfinder_lib
    src/grid.cpp
    src/algorithms.cpp
)

target_include_directories(pathfinder_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

add_executable(main
    src/main.cpp
    src/grid.cpp
    src/algorithms.cpp
    src/searchworker.cpp
    src/appstate.cpp
    src/renderer.cpp
    src/console.cpp
    src/terminal.cpp
    src/generator.cpp
)

# 2. Link against the modern targets using PRIVATE
target_link_libraries(main PRIVATE SFML::System SFML::Window SFML::Graphics)

enable_testing()

add_subdirectory(tests)

# cmake -S . -B build
# cmake --build build --clean-first
# ./build/main

# g++ -std=c++17 -Wall -Wextra \
#       -I/opt/homebrew/include -L/opt/homebrew/lib \
#       main.cpp terminal.cpp console.cpp appstate.cpp renderer.cpp \
#       SearchWorker.cpp algorithms.cpp grid.cpp \
#       -o pathfinder \
#       -lsfml-graphics -lsfml-window -lsfml-system -pthread