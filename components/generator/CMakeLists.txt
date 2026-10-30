#
# Random test matrix generator.
#

add_library(graph_matching_generator STATIC
    src/matrix_generator.cpp
)

target_sources(graph_matching_generator PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/matrix_generator.hpp
)

target_compile_features(graph_matching_generator PUBLIC
    cxx_std_23
)

add_library(graph_matching::generator ALIAS graph_matching_generator)

set_project_warnings(graph_matching_generator)
