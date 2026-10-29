#
# Application executable.
#

add_executable(graph_matching
    main.cpp
)

target_link_libraries(graph_matching PRIVATE
    graph_matching::greeting
    graph_matching::ring_buffer
    graph_matching::sensor
)

set_project_warnings(graph_matching)
