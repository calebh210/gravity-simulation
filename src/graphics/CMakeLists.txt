file(GLOB GRAPHICS_SOURCES *.c shapes/*.c)

file(GLOB LOADER_SOURCES loader/*.c)

file(GLOB TEXTURES_SOURCES textures/*.c)

add_library(graphics STATIC ${GRAPHICS_SOURCES})

add_library(loader STATIC ${LOADER_SOURCES})

add_library(textures STATIC ${TEXTURES_SOURCES})

target_include_directories(graphics PUBLIC ${CMAKE_SOURCE_DIR}/include/graphics)

target_link_libraries(textures PUBLIC core_libs)

target_link_libraries(graphics PUBLIC core_libs)
