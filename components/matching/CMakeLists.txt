#
# Matching: results, validation, file I/O, and the greedy baseline.
#

add_library(graph_matching_matching STATIC
    src/matching.c
)

target_sources(graph_matching_matching PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/matching.h
)

target_compile_features(graph_matching_matching PUBLIC
    c_std_17
)

target_link_libraries(graph_matching_matching PUBLIC
    graph_matching::graph
    graph_matching::support
)

add_library(graph_matching::matching ALIAS graph_matching_matching)

set_project_warnings(graph_matching_matching)
