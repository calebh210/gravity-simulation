file(GLOB GRAPHICS_SOURCES *.c shapes/*.c)

file(GLOB LOADER_SOURCES loader/*.c)


add_library(graphics STATIC ${GRAPHICS_SOURCES})

add_library(loader STATIC ${LOADER_SOURCES})


target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/include/graphics)

target_link_libraries(graphics PUBLIC core_libs)