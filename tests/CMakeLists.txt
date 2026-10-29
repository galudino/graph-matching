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

add_executable(graph_matching_tests
    read_mm_tests.cpp
    sort_tests.cpp
    utils_tests.cpp
)

target_link_libraries(graph_matching_tests PRIVATE
    graph_matching::match
    doctest::doctest_with_main
)

target_compile_definitions(graph_matching_tests PRIVATE
    GRAPH_MATCHING_DATA_DIR="${PROJECT_SOURCE_DIR}/data"
)

set_project_warnings(graph_matching_tests)

doctest_discover_tests(graph_matching_tests)
