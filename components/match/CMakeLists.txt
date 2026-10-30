#
# Graph matching library: the course harness plus the assignment files.
#

find_package(Threads REQUIRED)

add_library(graph_matching_match STATIC
    # Assignment: implement these.
    src/filter.c
    src/nways.c
    src/oneway.c

    # Course harness.
    src/nways_wrapper.c
    src/oneway_wrapper.c
    src/sort.c
    src/utils.c
)

target_sources(graph_matching_match PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/filter.h
        include/graph_matching/graph_data.h
        include/graph_matching/match.h
        include/graph_matching/nways.h
        include/graph_matching/oneway.h
        include/graph_matching/sort.h
        include/graph_matching/utils.h
)

target_compile_features(graph_matching_match PUBLIC
    c_std_17
)

# POSIX.1-2008 for pthread barriers and clock_gettime under strict C17.
target_compile_definitions(graph_matching_match PRIVATE
    _POSIX_C_SOURCE=200809L
)

target_link_libraries(graph_matching_match
    PUBLIC
        Threads::Threads
    PRIVATE
        graph_matching::mmio
)

add_library(graph_matching::match ALIAS graph_matching_match)

set_project_warnings(graph_matching_match)
