#
# Graph: weighted edges, the adjacency-array graph, and its Matrix Market reader.
#

add_library(graph_matching_cpp_graph STATIC
    src/edge.cpp
    src/graph.cpp
    src/matrix_market.cpp
)

target_sources(graph_matching_cpp_graph PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching_cpp/edge.hpp
        include/graph_matching_cpp/graph.hpp
)

target_compile_features(graph_matching_cpp_graph PUBLIC
    cxx_std_23
)

add_library(graph_matching_cpp::graph ALIAS graph_matching_cpp_graph)

graph_matching_cpp_set_warnings(graph_matching_cpp_graph)
