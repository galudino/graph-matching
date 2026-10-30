cmake_minimum_required(VERSION 3.25)

project(
    graph_matching
    VERSION 1.0.0
    DESCRIPTION "Parallel graph matching with one-way and N-way handshaking"
    LANGUAGES C CXX
)

#------------------------------------------------------------------------------
# Project options
#------------------------------------------------------------------------------

option(GRAPH_MATCHING_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(GRAPH_MATCHING_BUILD_DOCS "Add a Doxygen 'docs' target" ${PROJECT_IS_TOP_LEVEL})

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

include(cmake/CompilerWarnings.cmake)

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

if(GRAPH_MATCHING_BUILD_TESTS)
    # enable_testing() rather than include(CTest): CTest adds dozens of
    # dashboard targets that clutter the CMake Tools target picker.
    enable_testing()
    add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Documentation
#------------------------------------------------------------------------------

if(GRAPH_MATCHING_BUILD_DOCS)
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

        add_custom_target(docs
            COMMAND
                "${DOXYGEN_EXECUTABLE}"
                "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
            WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
            COMMENT "Generating Doxygen documentation"
            VERBATIM
        )
    else()
        message(STATUS "Doxygen not found; documentation disabled")
    endif()
endif()
