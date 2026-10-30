#
# Unit tests (doctest).
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

add_executable(graph_matching_c_tests
    edge_list_tests.cpp
    graph_tests.cpp
    matching_tests.cpp
    matrix_generator_tests.cpp
    support_tests.cpp
)

target_link_libraries(graph_matching_c_tests PRIVATE
    graph_matching_c::generator
    graph_matching_c::graph
    graph_matching_c::matching
    graph_matching_c::support
    doctest::doctest_with_main
)

target_compile_definitions(graph_matching_c_tests PRIVATE
    GRAPH_MATCHING_DATA_DIR="${GRAPH_MATCHING_C_DATA_DIR}"
)

graph_matching_c_set_warnings(graph_matching_c_tests)

# The prefix keeps test names apart from the C++ project's.
doctest_discover_tests(graph_matching_c_tests TEST_PREFIX "c/")

#
# Assignment tests: off by default, since they fail until the assignment is
# implemented. Configure with -DGRAPH_MATCHING_C_TEST_ASSIGNMENT=ON.
#

option(GRAPH_MATCHING_C_TEST_ASSIGNMENT "Build the assignment's tests" OFF)

if(GRAPH_MATCHING_C_TEST_ASSIGNMENT)
    add_executable(graph_matching_c_assignment_tests
        assignment/handshake_tests.cpp
    )

    target_link_libraries(graph_matching_c_assignment_tests PRIVATE
        graph_matching_c::generator
        graph_matching_c::handshake
        doctest::doctest_with_main
    )

    target_compile_definitions(graph_matching_c_assignment_tests PRIVATE
        GRAPH_MATCHING_DATA_DIR="${GRAPH_MATCHING_C_DATA_DIR}"
    )

    graph_matching_c_set_warnings(graph_matching_c_assignment_tests)

    # An unfinished implementation can loop forever; stop it after 30 seconds.
    doctest_discover_tests(graph_matching_c_assignment_tests
        TEST_PREFIX "c/"
        PROPERTIES TIMEOUT 30 LABELS assignment
    )
endif()
