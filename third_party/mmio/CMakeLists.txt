#
# NIST Matrix Market I/O library, vendored unchanged.
# See https://math.nist.gov/MatrixMarket/mmio-c.html
#

add_library(graph_matching_mmio STATIC
    mmio.c
)

# SYSTEM keeps this library's headers out of project warnings.
target_include_directories(graph_matching_mmio SYSTEM PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

# Third-party code: build it as-is, with its warnings silenced and no -Werror.
target_compile_options(graph_matching_mmio PRIVATE
    $<$<C_COMPILER_ID:GNU,Clang>:-w>
)

set_target_properties(graph_matching_mmio PROPERTIES
    COMPILE_WARNING_AS_ERROR OFF
)

add_library(graph_matching::mmio ALIAS graph_matching_mmio)
