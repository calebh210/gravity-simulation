file(GLOB PHYSICS_SOURCES *.c)

add_library(physics STATIC ${PHYSICS_SOURCES})

target_include_directories(physics PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(physics PUBLIC core_libs math utils)