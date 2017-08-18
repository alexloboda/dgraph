cmake_minimum_required(VERSION 3.8)
project(dgraph)

set(CMAKE_CXX_STANDARD 14)

set(SOURCE_FILES main.cpp DynamicGraph.cpp DynamicGraph.h EulerTourForest.cpp EulerTourForest.h)
add_executable(dgraph ${SOURCE_FILES})