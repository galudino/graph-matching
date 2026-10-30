#
# Unit tests (doctest). Every test name starts with "cpp/", to keep it apart
# from the C project's.
#

include(FetchContent)

FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.5.3
    GIT_SHALLOW    TRUE
    SYSTEM
)

FetchContent_MakeAvailable(doctest)

include("${doctest_SOURCE_DIR}/scripts/cmake/doctest.cmake")

add_executable(graph_matching_cpp_tests
    edge_tests.cpp
    graph_tests.cpp
)

target_link_libraries(graph_matching_cpp_tests PRIVATE
    graph_matching_cpp::graph
    doctest::doctest_with_main
)

target_compile_definitions(graph_matching_cpp_tests PRIVATE
    GRAPH_MATCHING_CPP_DATA_DIR="${GRAPH_MATCHING_CPP_DATA_DIR}"
)

graph_matching_cpp_set_warnings(graph_matching_cpp_tests)

doctest_discover_tests(graph_matching_cpp_tests TEST_PREFIX "cpp/")

add_test(NAME cpp/graph_matching_cpp_prints_help
    COMMAND graph_matching_cpp --help
)
