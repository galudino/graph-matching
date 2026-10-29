#
# Greeting component.
#

add_library(graph_matching_greeting STATIC
    src/greeting.cpp
)

target_sources(graph_matching_greeting PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/greeting.hpp
)

target_compile_features(graph_matching_greeting PUBLIC
    cxx_std_23
)

add_library(graph_matching::greeting ALIAS graph_matching_greeting)

set_project_warnings(graph_matching_greeting)
