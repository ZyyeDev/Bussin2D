cmake_minimum_required(VERSION 3.15)
project(Bussin2D)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenGL REQUIRED)
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

find_path(LUAJIT_INCLUDE_DIR luajit.h PATH_SUFFIXES luajit-2.1 luajit)
find_library(LUAJIT_LIBRARY NAMES lua51 luajit luajit-5.1)

file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/core/*.cpp"
    "src/core/*.h"
    "src/modules/*/*.cpp"
    "src/modules/*/*.c"
    "bindings/*.cpp"
)

# WINDOWS TEMPLATE
add_executable(template_windows WIN32 ${SOURCES})

target_link_libraries(template_windows PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    OpenGL::GL
    ${LUAJIT_LIBRARY}
)

target_include_directories(template_windows PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/bindings
    ${LUAJIT_INCLUDE_DIR}
)

set_target_properties(template_windows PROPERTIES
    OUTPUT_NAME "template_windows"
    SUFFIX ".exe"
)

# MAIN EXECUTABLE
add_executable(bussin2d ${SOURCES})

target_compile_definitions(bussin2d PRIVATE BUSSIN_EDITOR)

target_link_libraries(bussin2d PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    OpenGL::GL
    ${LUAJIT_LIBRARY}
)

target_include_directories(bussin2d PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/bindings
    ${LUAJIT_INCLUDE_DIR}
)

set_target_properties(bussin2d PROPERTIES
    OUTPUT_NAME "Bussin2D"
)

if(WIN32 AND EXISTS ${CMAKE_SOURCE_DIR}/assets/engine_icon.rc)
    target_sources(bussin2d PRIVATE ${CMAKE_SOURCE_DIR}/assets/engine_icon.rc)
    target_sources(template_windows PRIVATE ${CMAKE_SOURCE_DIR}/assets/engine_icon.rc)
endif()