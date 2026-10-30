#
# This file configures compiler warning flags for project targets.
#
# To fail the build on warnings, configure with
# -DCMAKE_COMPILE_WARNING_AS_ERROR=ON (CI does this).
#

function(graph_matching_cpp_set_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE
            /W4
            /permissive-
        )
    else()
        target_compile_options(${target} PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wsign-conversion
            -Wshadow
            -Wcast-align
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
            -Wold-style-cast
            -Wnon-virtual-dtor
            -Woverloaded-virtual
        )
    endif()
endfunction()
