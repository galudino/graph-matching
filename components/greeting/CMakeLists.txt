#
# Greeting component.
#

add_library(starter_greeting STATIC
    src/greeting.cpp
)

target_sources(starter_greeting PUBLIC
    FILE_SET HEADERS
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/include"
    FILES
        include/starter/greeting.hpp
)

target_compile_features(starter_greeting PUBLIC
    cxx_std_23
)

add_library(starter::greeting ALIAS starter_greeting)

set_project_warnings(starter_greeting)
