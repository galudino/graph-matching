#
# Libraries that make up the C++ implementation, one folder each, such as
# components/graph/ with include/graph_matching_cpp/ and src/. Add each one
# here with add_subdirectory(), name its target graph_matching_cpp_<name>, and
# give it the alias graph_matching_cpp::<name>.
#

add_subdirectory(graph)
