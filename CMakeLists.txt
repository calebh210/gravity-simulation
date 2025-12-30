cmake_minimum_required(VERSION 3.15...4.21)

project(GravitySimulator VERSION 1.0
                  DESCRIPTION "Simulate Gravity in C and OpenGL"
                  LANGUAGES C)


set(CMAKE_C_STANDARD 23)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_subdirectory(lib/glad_4_6_core)

add_subdirectory(src)

set_target_properties(sim PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/out
)