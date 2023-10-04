cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project (CustomAssert)

if (NOT TARGET CustomAssert)

add_library (CustomAssert STATIC src/CustomAssert.cpp
                                 src/Logger.cpp
                                 headers/CustomAssert.h
                                 headers/Logger.h)

target_include_directories (CustomAssert PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/headers)

add_subdirectory (ColorConsole)
target_link_libraries (CustomAssert PRIVATE ColorConsole)

endif ()



