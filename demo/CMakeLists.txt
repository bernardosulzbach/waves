cmake_minimum_required (VERSION 2.9)

set (CMAKE_BUILD_TYPE Release)

add_executable (demo demo.c)

target_link_libraries (demo Waves)

include (FindPkgConfig)
pkg_search_module (SDL2 REQUIRED sdl2)
include_directories (${SDL2_INCLUDE_DIRS})
target_link_libraries (demo ${SDL2_LIBRARIES})
