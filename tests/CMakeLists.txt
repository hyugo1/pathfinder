include(FetchContent)
include(GoogleTest)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.15.2
)
FetchContent_MakeAvailable(googletest)

add_executable(run_tests
    GridTest.cpp
    AlgorithmTest.cpp
    GeneratorTest.cpp
)
# Link GoogleTest and the project library.
target_link_libraries(run_tests
    PRIVATE
        GTest::gtest_main
        pathfinder_lib
)

gtest_discover_tests(run_tests)

# cmake -S . -B build
# cmake --build build
# ctest --test-dir build --output-on-failure
# ./build/tests/run_tests