#
# Handshake: the parallel matching driver and the assignment.
#

find_package(Threads REQUIRED)

add_library(graph_matching_c_handshake STATIC
    # Assignment: implement these.
    src/n_way.c
    src/one_way.c
    src/worklist.c

    # Driver.
    src/handshake.c
)

target_sources(graph_matching_c_handshake PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/handshake.h
        include/graph_matching/handshake_worker.h
        include/graph_matching/n_way.h
        include/graph_matching/one_way.h
        include/graph_matching/worklist.h
)

target_compile_features(graph_matching_c_handshake PUBLIC
    c_std_17
)

# POSIX.1-2008 for pthread barriers under strict C17.
target_compile_definitions(graph_matching_c_handshake PRIVATE
    _POSIX_C_SOURCE=200809L
)

target_link_libraries(graph_matching_c_handshake PUBLIC
    graph_matching_c::graph
    graph_matching_c::matching
    graph_matching_c::support
    Threads::Threads
)

add_library(graph_matching_c::handshake ALIAS graph_matching_c_handshake)

graph_matching_c_set_warnings(graph_matching_c_handshake)
