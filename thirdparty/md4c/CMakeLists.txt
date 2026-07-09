cmake_minimum_required(VERSION 3.30.5)

project(md4c LANGUAGES C)

add_library(md4c STATIC
        md4c.c
)

target_include_directories(md4c
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set_target_properties(md4c PROPERTIES
        C_STANDARD 99
        C_STANDARD_REQUIRED ON
)