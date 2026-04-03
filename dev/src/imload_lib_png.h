#pragma once

//! START CUSTOMHEADER
#include "imload_iface.h"
//! END CUSTOMHEADER

//! START STDHEADER GLOBAL
#include <cstring>
#include <filesystem>
//! END STDHEADER

//! START DECLARATION
#if !defined(PGE_IMAGELOADER_LIB_PNG_DECLARED)
#include <png.h>

namespace olc::imload
{
    class ImageLoader_LibPNG : public ImageLoader
    {	
        // Create an image resource based on an image file asset on disk
        bool CreateImageFromFile(olc::Image& image, const std::string& sFileName) override;

        // Create an image resource based on an image file asset in memory
        bool CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes) override;

        // Create an image resource based on an image file asset in memory
        bool CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data) override;

        // Store an image as a file asset on disk
        bool WriteImageToFile(const olc::Image& image, const std::string& sFileName) override;

        // Store an image as a file asset in memory
        bool WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data) override;
    
    public: // libpng readers
        struct MemReader { const uint8_t* data; size_t offset; };
        static void PNGReadFromMemory(png_structp png, png_bytep out, png_size_t count);

    private: // libpng internals
        bool DecodePNG(olc::Image& image, png_structp png, png_infop info);
    };
}

#define PGE_IMAGELOADER_LIB_PNG_DECLARED 1
#endif
//! END DECLARATION
