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

add_executable(starter_tests
    greeting_tests.cpp
    ring_buffer_tests.cpp
    sensor_tests.cpp
)

target_link_libraries(starter_tests PRIVATE
    starter::greeting
    starter::ring_buffer
    starter::sensor
    doctest::doctest_with_main
)

set_project_warnings(starter_tests)

doctest_discover_tests(starter_tests)
