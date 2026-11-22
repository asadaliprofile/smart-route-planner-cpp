
cmake_minimum_required(VERSION 3.16)

project(SmartRoutePlanner VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 1. Create the reusable Graph library
add_library(route_graph
    src/Graph.cpp
    src/DirectedGraph.cpp
    src/NetworkIO.cpp
    src/InputUtils.cpp

    src/algorithms/Dijkstra.cpp
    src/algorithms/AStar.cpp
    src/algorithms/BFS.cpp
    src/algorithms/BellmanFord.cpp
    src/algorithms/FloydWarshall.cpp
    src/algorithms/Kruskal.cpp
    src/algorithms/DirectedBellmanFord.cpp
)

target_include_directories(route_graph
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# 2. Create the main application
add_executable(route_planner
    src/main.cpp
    src/DirectedGraphMenu.cpp
)

target_link_libraries(route_planner
    PRIVATE route_graph
)

# 3. Create the benchmark executable
add_executable(route_benchmark
    benchmarks/benchmark.cpp
)

target_link_libraries(route_benchmark
    PRIVATE route_graph
)

# 4. Enable automated testing
enable_testing()

add_executable(route_tests
    tests/test_graph.cpp
)

target_link_libraries(route_tests
    PRIVATE route_graph
)

add_test(
    NAME GraphUnitTests
    COMMAND route_tests
)

# 5. Enable compiler warnings
if(MSVC)
    set(WARNING_FLAGS /W4)
else()
    set(WARNING_FLAGS -Wall -Wextra -Wpedantic)
endif()

foreach(target
    route_graph
    route_planner
    route_benchmark
    route_tests
)
    target_compile_options(${target}
        PRIVATE ${WARNING_FLAGS}
    )
endforeach()




# Algorithm consistency tests
add_executable(route_consistency_tests
    tests/test_consistency.cpp
)

target_link_libraries(route_consistency_tests
    PRIVATE route_graph
)

target_compile_options(route_consistency_tests
    PRIVATE ${WARNING_FLAGS}
)

add_test(
    NAME AlgorithmConsistencyTests
    COMMAND route_consistency_tests
)


# Input validation tests
add_executable(route_input_tests
    tests/test_input.cpp
)

target_link_libraries(route_input_tests
    PRIVATE route_graph
)

add_test(
    NAME InputValidationTests
    COMMAND route_input_tests
)


