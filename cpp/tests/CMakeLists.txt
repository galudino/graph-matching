#
# Tests. Prefix every test name with "cpp/" to keep it apart from the C
# project's, such as doctest_discover_tests(<target> TEST_PREFIX "cpp/") once
# components have unit tests.
#

add_test(NAME cpp/graph_matching_cpp_prints_help
    COMMAND graph_matching_cpp --help
)
