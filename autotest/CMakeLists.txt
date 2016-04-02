cmake_minimum_required (VERSION 2.9)

set (CMAKE_BUILD_TYPE Release)

add_executable (autotest autotest.c)

target_link_libraries (autotest Waves)
target_link_libraries (autotest Unity)
