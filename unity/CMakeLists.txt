cmake_minimum_required (VERSION 2.9)

add_library (Unity unity.c unity.h unity_internals.h)

target_include_directories (Unity PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
