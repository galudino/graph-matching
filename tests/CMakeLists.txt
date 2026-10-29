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
    greeting_tests.cpp
    ring_buffer_tests.cpp
    sensor_tests.cpp
)

target_link_libraries(graph_matching_tests PRIVATE
    graph_matching::greeting
    graph_matching::ring_buffer
    graph_matching::sensor
    doctest::doctest_with_main
)

set_project_warnings(graph_matching_tests)

doctest_discover_tests(graph_matching_tests)
