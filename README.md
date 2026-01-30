# Linux build

## Cmake

### Building X11 (Default)
```bash
cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
```

Header file should now be located at `build/olcPixelGameEngine3.h`.

Examples will be located at `build/examples`

### Building for Wayland

```bash
cmake -B build -DBUILD_EXAMPLES=ON -DBUILD_WAYLAND=ON
cmake --build build
```

Header file should now be located at `build/olcPixelGameEngine3.h`.

Examples will be located at `build/examples`


## Manually build a single example with GCC

```bash
cd examples
g++ -ggdb olcPGE3_ImageQuads.cpp -o olcPGE3_ImageQuads -std=c++20 -lpng -lGL -lX11 -lpthread
```

## Manually build a single example with Clang

```bash
cd examples
clang++ -ggdb olcPGE3_ImageQuads.cpp -o olcPGE3_ImageQuads -std=c++20 -lpng -lGL -lX11 -lpthread
```

# Emscripten build

## Cmake
```bash
emcmake cmake -B build -DBUILD_EXAMPLES=ON
cmake --build build
```
Header file should now be located at `build/olcPixelGameEngine3.h`.

Examples will be located at `build/examples`


## Manually build a single example

```bash
cd examples
emcc olcPGE3_ImageQuads.cpp -o olcPGE3_ImageQuads.html -sASYNCIFY -sALLOW_MEMORY_GROWTH=1 -sSTACK_SIZE=1048576 -sEXPORTED_RUNTIME_METHODS=HEAPF32 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2 -sUSE_LIBPNG=1 -sLLD_REPORT_UNDEFINED --preload-file assets@assets
```
