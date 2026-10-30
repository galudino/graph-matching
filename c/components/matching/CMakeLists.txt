#
# Matching: results, validation, file I/O, and the greedy baseline.
#

add_library(graph_matching_c_matching STATIC
    src/matching.c
)

target_sources(graph_matching_c_matching PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/matching.h
)

target_compile_features(graph_matching_c_matching PUBLIC
    c_std_17
)

target_link_libraries(graph_matching_c_matching PUBLIC
    graph_matching_c::graph
    graph_matching_c::support
)

add_library(graph_matching_c::matching ALIAS graph_matching_c_matching)

graph_matching_c_set_warnings(graph_matching_c_matching)
