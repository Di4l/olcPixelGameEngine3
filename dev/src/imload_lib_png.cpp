#include "imload_lib_png.h"

//! START IMPLEMENTATION
namespace olc::imload
{
    // Create an image resource based on an image file asset on disk
    bool ImageLoader_LibPNG::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        FILE* pngFileHandle = fopen(sFileName.c_str(), "rb");
        if(!pngFileHandle)
            return false;

        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png)
            return false;

        png_infop info = png_create_info_struct(png);
        if (!info)
            return false;

        if(setjmp(png_jmpbuf(png)))
        {
            png_destroy_read_struct(&png, &info, nullptr);
            fclose(pngFileHandle);
            return false;
        }
        
        png_init_io(png, pngFileHandle);
        bool decodeResult = DecodePNG(image, png, info);
        
        png_destroy_read_struct(&png, &info, nullptr);
        fclose(pngFileHandle);

        return decodeResult;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_LibPNG::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        MemReader reader{ data, 0 };
        png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        if (!png)
            return false;
        std::cout << "create read struct success\n";

        png_infop info = png_create_info_struct(png);
        if (!info)
            return false;
        std::cout << "create info struct success\n";
        
        if(setjmp(png_jmpbuf(png)))
        {
            std::cout << "setjmp/png_jmpbuf failed\n";
            png_destroy_read_struct(&png, &info, nullptr);
            return false;
        }
        std::cout << "setjmp/png_jmpbuf success\n";

        png_set_read_fn(png, &reader, &ImageLoader_LibPNG::PNGReadFromMemory);
        bool decodeResult = DecodePNG(image, png, info);
        
        png_destroy_read_struct(&png, &info, nullptr);
        return decodeResult;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_LibPNG::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return CreateImageFromMemory(image, data.data(), data.size());
    }
    
    // Store an image as a file asset on disk
    bool ImageLoader_LibPNG::WriteImageToFile(const olc::Image& image, const std::string& sFileName) 
    {
        return false;
    }
    
    // Store an image as a file asset in memory
    bool ImageLoader_LibPNG::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return false;
    }
    
    void ImageLoader_LibPNG::PNGReadFromMemory(png_structp png, png_bytep out, png_size_t count)
    {
        auto* reader = (MemReader*)png_get_io_ptr(png);
        std::memcpy(out, reader->data + reader->offset, count);
        reader->offset += count;
    }

    bool ImageLoader_LibPNG::DecodePNG(olc::Image& image, png_structp png, png_infop info)
    {
        ////////////////////////////////////////////////////////////////////////////
        // Use libpng, Thanks to Guillaume Cottenceau
        // https://gist.github.com/niw/5963798
        // Also reading png from streams
        // http://www.piko3d.net/tutorials/libpng-tutorial-loading-png-files-from-streams/
        png_read_info(png, info);
        png_byte color_type;
        png_byte bit_depth;
        image.Create(
            {
                static_cast<int>(png_get_image_width(png, info)),
                static_cast<int>(png_get_image_height(png, info))
            }
        );

        color_type = png_get_color_type(png, info);
        bit_depth = png_get_bit_depth(png, info);
        if (bit_depth == 16) png_set_strip_16(png);
        if (color_type == PNG_COLOR_TYPE_PALETTE) png_set_palette_to_rgb(png);
        if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)	png_set_expand_gray_1_2_4_to_8(png);
        if (png_get_valid(png, info, PNG_INFO_tRNS)) png_set_tRNS_to_alpha(png);
        if (color_type == PNG_COLOR_TYPE_RGB || color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_PALETTE)
            png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
        if (color_type == PNG_COLOR_TYPE_GRAY || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
            png_set_gray_to_rgb(png);
        
        png_read_update_info(png, info);
        
        std::vector<png_bytep> rows(image.Size().y);
        std::vector<std::vector<png_byte>> rowData(image.Size().y);
        for (int y = 0; y < image.Size().y; y++) {
            rowData[y].resize(png_get_rowbytes(png, info));
            rows[y] = rowData[y].data();
        }
        png_read_image(png, rows.data());

        // Iterate through image rows, converting into sprite format
        for (int y = 0; y < image.Size().y; y++)
        {
            png_bytep row = rows[y];
            for (int x = 0; x < image.Size().x; x++)
            {
                png_bytep px = &(row[x * 4]);
                image.Pixel(olc::vi2d(x, y)) = olc::Pixel(px[0], px[1], px[2], px[3]);
            }
        }
        
        return true;
    }
}
//! END IMPLEMENTATION
