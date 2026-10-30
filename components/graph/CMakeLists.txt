#
# Graph: weighted edge lists and the adjacency-array graph.
#

add_library(graph_matching_graph STATIC
    src/edge_list.c
    src/graph.c
)

target_sources(graph_matching_graph PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/edge_list.h
        include/graph_matching/graph.h
)

target_compile_features(graph_matching_graph PUBLIC
    c_std_17
)

target_link_libraries(graph_matching_graph
    PUBLIC
        graph_matching::support
    PRIVATE
        graph_matching::mmio
)

add_library(graph_matching::graph ALIAS graph_matching_graph)

set_project_warnings(graph_matching_graph)
