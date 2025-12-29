cmake_minimum_required(VERSION 3.15)
project(Bussin2D)

set(CMAKE_CXX_STANDARD 17)

# SDL2
find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)

# LuaJIT
find_path(LUAJIT_INCLUDE_DIR luajit.h
    PATH_SUFFIXES luajit-2.1 luajit
)

find_library(LUAJIT_LIBRARY 
    NAMES lua51 luajit luajit-5.1
)

# Collect all source files
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
    "src/core/*.cpp"
    "src/modules/*/*.cpp"
    "src/modules/*/*.c"
    "bindings/*.cpp"
)

# Debug: Print what was found
message(STATUS "LuaJIT Include: ${LUAJIT_INCLUDE_DIR}")
message(STATUS "LuaJIT Library: ${LUAJIT_LIBRARY}")

# Create executable
add_executable(Bussin2D ${SOURCES})

# Link libraries
target_link_libraries(Bussin2D PRIVATE 
    SDL2::SDL2
    SDL2::SDL2main
    $<IF:$<TARGET_EXISTS:SDL2_image::SDL2_image>,SDL2_image::SDL2_image,SDL2_image::SDL2_image-static>
    $<IF:$<TARGET_EXISTS:SDL2_mixer::SDL2_mixer>,SDL2_mixer::SDL2_mixer,SDL2_mixer::SDL2_mixer-static>
    ${LUAJIT_LIBRARY}
)

# Include directories
target_include_directories(Bussin2D PRIVATE 
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/bindings
    ${LUAJIT_INCLUDE_DIR}
)

# Copy Lua files to build directory
file(COPY ${CMAKE_SOURCE_DIR}/lua DESTINATION ${CMAKE_BINARY_DIR})