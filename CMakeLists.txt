cmake_minimum_required(VERSION 3.25)

#
# Repository root: builds each implementation as its own project. Each one
# also builds on its own, such as cmake -S c -B build/c.
#

project(
    graph_matching_workspace
    DESCRIPTION "Parallel graph matching with one-way and N-way handshaking"
    LANGUAGES NONE
)

#------------------------------------------------------------------------------
# Implementations
#------------------------------------------------------------------------------

option(GRAPH_MATCHING_BUILD_C "Build the C implementation in c/" ON)
option(GRAPH_MATCHING_BUILD_CPP "Build the C++ implementation in cpp/" ON)

# Tells each project it is the main build, so it builds its tests and docs.
set(GRAPH_MATCHING_WORKSPACE ON)

# Registers each project's tests with ctest from the build directory.
enable_testing()

if(GRAPH_MATCHING_BUILD_C)
    add_subdirectory(c)
endif()

if(GRAPH_MATCHING_BUILD_CPP)
    add_subdirectory(cpp)
endif()
