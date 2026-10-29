#
# Application executable.
#

add_executable(graph_matching
    main.c
)

target_link_libraries(graph_matching PRIVATE
    graph_matching::match
)

set_project_warnings(graph_matching)
