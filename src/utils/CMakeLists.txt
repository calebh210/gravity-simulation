file(GLOB UTILS_SOURCES *.c)

add_library(utils STATIC ${UTILS_SOURCES})

target_include_directories(utils PUBLIC ${CMAKE_SOURCE_DIR}/include/utils)

target_link_libraries(utils PUBLIC core_libs)