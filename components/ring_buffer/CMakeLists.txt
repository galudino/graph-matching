#
# Header-only ring buffer component.
#

add_library(starter_ring_buffer INTERFACE)

target_sources(starter_ring_buffer INTERFACE
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/starter/ring_buffer.hpp
)

target_compile_features(starter_ring_buffer INTERFACE
    cxx_std_23
)

add_library(starter::ring_buffer ALIAS starter_ring_buffer)
