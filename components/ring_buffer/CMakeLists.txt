#
# Header-only ring buffer component.
#

add_library(graph_matching_ring_buffer INTERFACE)

target_sources(graph_matching_ring_buffer INTERFACE
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/ring_buffer.hpp
)

target_compile_features(graph_matching_ring_buffer INTERFACE
    cxx_std_23
)

add_library(graph_matching::ring_buffer ALIAS graph_matching_ring_buffer)
