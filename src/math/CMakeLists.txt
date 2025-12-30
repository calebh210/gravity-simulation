file(GLOB MATH_SOURCES *.c)

add_library(math STATIC ${MATH_SOURCES})


target_include_directories(math PUBLIC ${CMAKE_SOURCE_DIR}/include/math)

target_link_libraries(math PUBLIC core_libs)