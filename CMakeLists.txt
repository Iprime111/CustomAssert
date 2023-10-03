cmake_minimum_required (VERSION 3.8 FATAL_ERROR)

project (CustomAssert)

add_library (CustomAssert INTERFACE src/CustomAssert.cpp
                                src/Logger.cpp
                                headers/CustomAssert.h
                                headers/Logger.h)

target_include_directories (CustomAssert INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/headers)

add_subdirectory (ColorConsole)
target_link_libraries (CustomAssert INTERFACE ColorConsole)



