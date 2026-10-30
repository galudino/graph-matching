#
# Application executable.
#

add_executable(graph_matching_c
    main.c
)

target_link_libraries(graph_matching_c PRIVATE
    graph_matching_c::handshake
)

graph_matching_c_set_warnings(graph_matching_c)

add_executable(generate_matrix_c
    generate_matrix.cpp
)

target_link_libraries(generate_matrix_c PRIVATE
    graph_matching_c::generator
)

graph_matching_c_set_warnings(generate_matrix_c)

#
# Run targets: cmake --build --preset debug --target run_one_way_c (or run_n_ways_c,
# run_greedy_c).
# Pick the input with -DGRAPH_MATCHING_C_RUN_MATRIX=..., and so on, at configure time.
#

set(GRAPH_MATCHING_C_RUN_MATRIX "${GRAPH_MATCHING_C_DATA_DIR}/test.mtx"
    CACHE FILEPATH "Matrix that the run_* targets match")
set(GRAPH_MATCHING_C_RUN_THREADS 4
    CACHE STRING "Number of threads for the run_* targets")
set(GRAPH_MATCHING_C_RUN_HANDS 2
    CACHE STRING "Hands per vertex for run_n_ways_c")

add_custom_target(run_one_way_c
    COMMAND graph_matching_c
        "${GRAPH_MATCHING_C_RUN_MATRIX}"
        "${CMAKE_BINARY_DIR}/one_way_c.out"
        one-way
        "${GRAPH_MATCHING_C_RUN_THREADS}"
    DEPENDS graph_matching_c
    WORKING_DIRECTORY "${GRAPH_MATCHING_C_REPOSITORY_DIR}"
    COMMENT "One-way matching on ${GRAPH_MATCHING_C_RUN_MATRIX}; result in one_way_c.out"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(run_n_ways_c
    COMMAND graph_matching_c
        "${GRAPH_MATCHING_C_RUN_MATRIX}"
        "${CMAKE_BINARY_DIR}/n_ways_c.out"
        n-way
        "${GRAPH_MATCHING_C_RUN_HANDS}"
        "${GRAPH_MATCHING_C_RUN_THREADS}"
    DEPENDS graph_matching_c
    WORKING_DIRECTORY "${GRAPH_MATCHING_C_REPOSITORY_DIR}"
    COMMENT "${GRAPH_MATCHING_C_RUN_HANDS}-way matching on ${GRAPH_MATCHING_C_RUN_MATRIX}; result in n_ways_c.out"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(run_greedy_c
    COMMAND graph_matching_c
        "${GRAPH_MATCHING_C_RUN_MATRIX}"
        "${CMAKE_BINARY_DIR}/greedy_c.out"
        greedy
    DEPENDS graph_matching_c
    WORKING_DIRECTORY "${GRAPH_MATCHING_C_REPOSITORY_DIR}"
    COMMENT "Greedy matching on ${GRAPH_MATCHING_C_RUN_MATRIX}; result in greedy_c.out"
    USES_TERMINAL
    VERBATIM
)

#
# Generated test matrices: cmake --build --preset debug --target generate_matrices_c
# writes small, medium, and large matrices to data/generated/.
#

set(GRAPH_MATCHING_C_GENERATED_DIR "${GRAPH_MATCHING_C_DATA_DIR}/generated")

add_custom_target(generate_matrices_c
    COMMAND generate_matrix_c "${GRAPH_MATCHING_C_GENERATED_DIR}/small.mtx" 1000 8
    COMMAND generate_matrix_c "${GRAPH_MATCHING_C_GENERATED_DIR}/medium.mtx" 100000 16
    COMMAND generate_matrix_c "${GRAPH_MATCHING_C_GENERATED_DIR}/large.mtx" 1000000 16
    DEPENDS generate_matrix_c
    COMMENT "Writing generated test matrices to data/generated/"
    USES_TERMINAL
    VERBATIM
)
