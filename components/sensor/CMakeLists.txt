#
# Sensor C library.
#

add_library(graph_matching_sensor STATIC
    src/sensor.c
)

target_sources(graph_matching_sensor PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/graph_matching/sensor.h
)

target_compile_features(graph_matching_sensor PUBLIC
    c_std_17
)

add_library(graph_matching::sensor ALIAS graph_matching_sensor)

set_project_warnings(graph_matching_sensor)
