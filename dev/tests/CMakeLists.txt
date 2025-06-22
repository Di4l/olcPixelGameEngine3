cmake_minimum_required(VERSION 3.10)
project(pge3_test)

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

######################################################################
# Directories

# Source Files are Curated Here
file(
    GLOB SOURCE_CXX_FILES
    "*.c"
    "*.cpp"
    "*.h"
    "*.hpp"
)

# Executable aka binary output
add_executable(${CMAKE_PROJECT_NAME} ${SOURCE_CXX_FILES})
