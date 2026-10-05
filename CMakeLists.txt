cmake_minimum_required(VERSION 3.16)
project(pathfinder)

# 1. Find SFML and the requested components (Case-sensitive depending on version, e.g., Graphics or graphics)
find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics)

add_executable(main main.cpp)

# 2. Link against the modern targets using PRIVATE
target_link_libraries(main PRIVATE SFML::System SFML::Window SFML::Graphics)


# cmake -S . -B build
# cmake --build build --clean-first
# ./build/main