cmake_minimum_required(VERSION 3.19)
project(olcPixelGameEngine VERSION 3.0.0)


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Make sure we have the header tool
include(FetchContent)
FetchContent_Declare(
    olcGimmeHead
    GIT_REPOSITORY https://github.com/OneLoneCoder/olcGimmeHead.git
    GIT_TAG e306043922ba5e1927080ad7b4f965505e607617
)

FetchContent_MakeAvailable(olcGimmeHead)

file(GLOB sources
    "dev/src/*.cpp"
    "dev/src/*.hpp"
    "dev/src/*.h"
    )

add_custom_command(
    OUTPUT olcPixelGameEngine3.h
    COMMAND ${olcGimmeHead_BINARY_DIR}/gimme-head ${CMAKE_CURRENT_SOURCE_DIR}/dev/src/sh_template.h ${CMAKE_BINARY_DIR}/olcPixelGameEngine3.h
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/dev/src
    DEPENDS ${sources}
    DEPENDS ${olcGimmeHead_BINARY_DIR}/gimme-head
)

add_custom_target(CopyHeader
    COMMAND ${CMAKE_COMMAND} -E copy olcPixelGameEngine3.h ${CMAKE_CURRENT_SOURCE_DIR}/olcPixelGameEngine3.h
    DEPENDS olcPixelGameEngine3.h
)

add_library(olcPixelGameEngine3 INTERFACE olcPixelGameEngine3.h)
add_dependencies(olcPixelGameEngine3 CopyHeader)
