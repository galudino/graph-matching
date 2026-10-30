cmake_minimum_required(VERSION 3.25)

project(
    graph_matching_c
    VERSION 1.0.0
    DESCRIPTION "Parallel graph matching with one-way and N-way handshaking"
    LANGUAGES C CXX
)

#------------------------------------------------------------------------------
# Project options
#------------------------------------------------------------------------------

# Tests and docs are on by default when this project is built on its own
# (cmake -S c) or from the repository root, which sets GRAPH_MATCHING_WORKSPACE.
if(PROJECT_IS_TOP_LEVEL OR GRAPH_MATCHING_WORKSPACE)
    set(graph_matching_c_is_main ON)
else()
    set(graph_matching_c_is_main OFF)
endif()

option(GRAPH_MATCHING_C_BUILD_TESTS "Build unit tests" ${graph_matching_c_is_main})
option(GRAPH_MATCHING_C_BUILD_DOCS "Add a Doxygen 'docs_c' target" ${graph_matching_c_is_main})

#------------------------------------------------------------------------------
# Repository paths
#------------------------------------------------------------------------------

# Test matrices live at the repository root, shared with the C++ project.
cmake_path(GET PROJECT_SOURCE_DIR PARENT_PATH GRAPH_MATCHING_C_REPOSITORY_DIR)
set(GRAPH_MATCHING_C_DATA_DIR "${GRAPH_MATCHING_C_REPOSITORY_DIR}/data")

# Put every executable in <build dir>/bin, next to the C++ project's.
if(NOT CMAKE_RUNTIME_OUTPUT_DIRECTORY)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endif()

#------------------------------------------------------------------------------
# Language standards
#------------------------------------------------------------------------------

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#------------------------------------------------------------------------------
# Toolchain check
#------------------------------------------------------------------------------

# Fail at configure time, rather than mid-build, when the standard library
# lacks C++23 <print> (for example, GCC 13 or a stale cache from one).
include(CheckIncludeFileCXX)
check_include_file_cxx(print GRAPH_MATCHING_HAS_STD_PRINT)

if(NOT GRAPH_MATCHING_HAS_STD_PRINT)
    unset(GRAPH_MATCHING_HAS_STD_PRINT CACHE)
    message(FATAL_ERROR
        "C++23 <print> was not found using ${CMAKE_CXX_COMPILER} "
        "(${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}).\n"
        "This project needs GCC 14+, or Clang 18+ with libstdc++ 14 or libc++ 17.\n"
        "In Codespaces: run 'Codespaces: Rebuild Container', then "
        "'CMake: Delete Cache and Reconfigure'.\n"
        "Elsewhere: delete the build directory and configure with "
        "CXX=g++-14 (or another suitable compiler).")
endif()

include("${PROJECT_SOURCE_DIR}/cmake/CompilerWarnings.cmake")

#------------------------------------------------------------------------------
# Components
#------------------------------------------------------------------------------

add_subdirectory(third_party/mmio)
add_subdirectory(components/support)
add_subdirectory(components/graph)
add_subdirectory(components/matching)
add_subdirectory(components/handshake)
add_subdirectory(components/generator)

#------------------------------------------------------------------------------
# Command-line executable
#------------------------------------------------------------------------------

add_subdirectory(app)

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------

if(GRAPH_MATCHING_C_BUILD_TESTS)
    # enable_testing() rather than include(CTest): CTest adds dozens of
    # dashboard targets that clutter the CMake Tools target picker.
    enable_testing()
    add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Documentation
#------------------------------------------------------------------------------

if(GRAPH_MATCHING_C_BUILD_DOCS)
    find_package(Doxygen QUIET)

    if(DOXYGEN_FOUND)
        set(DOXYGEN_OUTPUT_DIRECTORY
            "${CMAKE_CURRENT_BINARY_DIR}/docs"
        )

        configure_file(
            "${PROJECT_SOURCE_DIR}/docs/Doxyfile.in"
            "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            @ONLY
        )

        add_custom_target(docs_c
            COMMAND
                "${DOXYGEN_EXECUTABLE}"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
            COMMENT "Generating Doxygen documentation for the C project"
            VERBATIM
        )
    else()
        message(STATUS "Doxygen not found; documentation disabled")
    endif()
endif()
