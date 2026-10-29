#
# Sensor C library.
#

add_library(starter_sensor STATIC
    src/sensor.c
)

target_sources(starter_sensor PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/starter/sensor.h
)

target_compile_features(starter_sensor PUBLIC
    c_std_17
)

add_library(starter::sensor ALIAS starter_sensor)

set_project_warnings(starter_sensor)
