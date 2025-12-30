add_subdirectory(utils)
add_subdirectory(physics)
add_subdirectory(math)
add_subdirectory(graphics)


add_executable(sim main.c)

target_compile_features(sim PUBLIC c_std_23)

find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

add_library(core_libs INTERFACE)

if (WIN32)
    find_package(yaml CONFIG REQUIRED)
    target_link_libraries(core_libs INTERFACE
    OpenGL::GL
    OpenGL::GLU
    glfw
    glad
    yaml
    )
endif()

if (UNIX)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(YAML REQUIRED yaml-0.1)
    target_link_libraries(core_libs INTERFACE
    OpenGL::GL
    OpenGL::GLU
    glfw
    glad
    yaml
    m
    dl )
endif()

target_link_libraries(sim PRIVATE utils graphics physics math)