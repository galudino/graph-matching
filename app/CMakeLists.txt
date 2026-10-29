#
# Application executable.
#

add_executable(graph_matching
    main.c
)

target_link_libraries(graph_matching PRIVATE
    graph_matching::match
)

set_project_warnings(graph_matching)

#
# Run targets: cmake --build --preset debug --target run_one_way (or run_n_ways).
# Pick the input with -DGRAPH_MATCHING_RUN_MATRIX=..., and so on, at configure time.
#

set(GRAPH_MATCHING_RUN_MATRIX "${PROJECT_SOURCE_DIR}/data/test.mtx"
    CACHE FILEPATH "Matrix that the run_* targets match")
set(GRAPH_MATCHING_RUN_THREADS 4
    CACHE STRING "Number of threads for the run_* targets")
set(GRAPH_MATCHING_RUN_HANDS 2
    CACHE STRING "Hands per vertex for run_n_ways")

add_custom_target(run_one_way
    COMMAND graph_matching
        "${GRAPH_MATCHING_RUN_MATRIX}"
        "${PROJECT_BINARY_DIR}/one_way.out"
        oneWay
        "${GRAPH_MATCHING_RUN_THREADS}"
    DEPENDS graph_matching
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "One-way matching on ${GRAPH_MATCHING_RUN_MATRIX}; result in one_way.out"
    USES_TERMINAL
    VERBATIM
)

add_custom_target(run_n_ways
    COMMAND graph_matching
        "${GRAPH_MATCHING_RUN_MATRIX}"
        "${PROJECT_BINARY_DIR}/n_ways.out"
        nWays
        "${GRAPH_MATCHING_RUN_HANDS}"
        "${GRAPH_MATCHING_RUN_THREADS}"
    DEPENDS graph_matching
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    COMMENT "${GRAPH_MATCHING_RUN_HANDS}-way matching on ${GRAPH_MATCHING_RUN_MATRIX}; result in n_ways.out"
    USES_TERMINAL
    VERBATIM
)
