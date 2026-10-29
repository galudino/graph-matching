cmake_minimum_required(VERSION 3.25)

project(
    cpp_starter
    VERSION 1.0.0
    DESCRIPTION "Modern C/C++ command-line starter project"
    LANGUAGES C CXX
)

#------------------------------------------------------------------------------
# Project options
#------------------------------------------------------------------------------

option(STARTER_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(STARTER_BUILD_DOCS "Add a Doxygen 'docs' target" ${PROJECT_IS_TOP_LEVEL})

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
check_include_file_cxx(print STARTER_HAS_STD_PRINT)

if(NOT STARTER_HAS_STD_PRINT)
    unset(STARTER_HAS_STD_PRINT CACHE)
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

add_subdirectory(components/greeting)
add_subdirectory(components/ring_buffer)
add_subdirectory(components/sensor)

#------------------------------------------------------------------------------
# Command-line executable
#------------------------------------------------------------------------------

add_subdirectory(app)

#------------------------------------------------------------------------------
# Tests
#------------------------------------------------------------------------------

if(STARTER_BUILD_TESTS)
    # enable_testing() rather than include(CTest): CTest adds dozens of
    # dashboard targets that clutter the CMake Tools target picker.
    enable_testing()
    add_subdirectory(tests)
endif()

#------------------------------------------------------------------------------
# Documentation
#------------------------------------------------------------------------------

if(STARTER_BUILD_DOCS)
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
