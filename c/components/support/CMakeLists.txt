#
# Support: status codes, logging, timing, and prefix sums.
#

add_library(graph_matching_c_support STATIC
    src/log.c
    src/prefix_sum.c
    src/status_code.c
    src/stopwatch.c
)

target_sources(graph_matching_c_support PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/log.h
        include/graph_matching/prefix_sum.h
        include/graph_matching/status_code.h
        include/graph_matching/stopwatch.h
)

target_compile_features(graph_matching_c_support PUBLIC
    c_std_17
)

# POSIX.1-2008 for clock_gettime under strict C17.
target_compile_definitions(graph_matching_c_support PRIVATE
    _POSIX_C_SOURCE=200809L
)

add_library(graph_matching_c::support ALIAS graph_matching_c_support)

graph_matching_c_set_warnings(graph_matching_c_support)
