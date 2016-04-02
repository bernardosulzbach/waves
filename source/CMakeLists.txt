cmake_minimum_required (VERSION 2.9)

add_library (Waves geometry.h logger.h cached-geometry.h constants.h)

set_target_properties (Waves PROPERTIES LINKER_LANGUAGE C)

target_include_directories (Waves PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
