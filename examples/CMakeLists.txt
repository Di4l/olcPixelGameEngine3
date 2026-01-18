cmake_minimum_required(VERSION 3.19)
project(PGE_Examples)

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

######################################################################
# Directories
file(GLOB SOURCES "*.cpp")

if(EMSCRIPTEN)
    # build Cache: libpng, zlib
    execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

foreach(source ${SOURCES})
    get_filename_component(EXE_NAME ${source} NAME_WE)
    add_executable(${EXE_NAME} ${source})
    target_link_libraries(${EXE_NAME} PRIVATE olcPixelGameEngine3)

    if(APPLE)
        if(CMAKE_SYSTEM_NAME STREQUAL "iOS")
            # TODO: Building for iOS
        else()
            target_link_libraries(${EXE_NAME} PRIVATE "-framework Metal")
            target_link_libraries(${EXE_NAME} PRIVATE "-framework MetalKit")
            target_link_libraries(${EXE_NAME} PRIVATE "-framework AppKit")
            target_link_libraries(${EXE_NAME} PRIVATE "-framework QuartzCore")
            target_link_libraries(${EXE_NAME} PRIVATE "-framework OpenGL")
            target_link_libraries(${EXE_NAME} PRIVATE "-framework Foundation")
        endif()
    endif()

    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        target_link_libraries(${EXE_NAME} PRIVATE png)
        target_link_libraries(${EXE_NAME} PRIVATE GL)
        target_link_libraries(${EXE_NAME} PRIVATE X11)
        target_link_libraries(${EXE_NAME} PRIVATE pthread)
        
        target_compile_options(${EXE_NAME} PRIVATE -fmax-errors=5)
    endif()

    if(WIN32 AND NOT MSVC AND NOT EMSCRIPTEN)
        target_link_libraries(${EXE_NAME} PRIVATE user32)
        target_link_libraries(${EXE_NAME} PRIVATE gdi32)
        target_link_libraries(${EXE_NAME} PRIVATE dwmapi)
        target_link_libraries(${EXE_NAME} PRIVATE gdiplus)
        target_link_libraries(${EXE_NAME} PRIVATE shlwapi)
        target_link_libraries(${EXE_NAME} PRIVATE opengl32)

        target_compile_options(${EXE_NAME} PRIVATE -fmax-errors=5)
    endif()
    
    if(EMSCRIPTEN)
        target_link_options(${EXE_NAME} PRIVATE -sASYNCIFY)
        target_link_options(${EXE_NAME} PRIVATE -sALLOW_MEMORY_GROWTH=1)
        target_link_options(${EXE_NAME} PRIVATE -sSTACK_SIZE=1048576)
        target_link_options(${EXE_NAME} PRIVATE -sEXPORTED_RUNTIME_METHODS=HEAPF32)
        target_link_options(${EXE_NAME} PRIVATE -sMAX_WEBGL_VERSION=2)
        target_link_options(${EXE_NAME} PRIVATE -sMIN_WEBGL_VERSION=2)
        target_link_options(${EXE_NAME} PRIVATE -sUSE_LIBPNG=1)
        target_link_options(${EXE_NAME} PRIVATE -sLLD_REPORT_UNDEFINED)
        target_link_options(${EXE_NAME} PRIVATE --preload-file ${CMAKE_CURRENT_SOURCE_DIR}/assets@assets)
    endif()

    if(NOT EMSCRIPTEN)
        # Copy assets when not Web platform
        add_custom_command(
            TARGET ${EXE_NAME}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/assets ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/assets
        )        
    endif()
endforeach()
