cmake_minimum_required(VERSION 3.5)
project(dgraph)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        DynamicGraph.cpp
        DynamicGraph.h
        EulerTourForest.cpp
        EulerTourForest.h)

set(TEST_SOURCES
        test/catch.hpp
        test/DynamicGraphTests.cpp
        test/ETFtests.cpp)

add_executable(tests ${SOURCE_FILES} ${TEST_SOURCES})
add_executable(dgraph ${SOURCE_FILES} Main.cpp)
target_link_libraries(dgraph profiler)
