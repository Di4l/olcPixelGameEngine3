#include "imload_lib_png.h"

//! START IMPLEMENTATION
#include <png.h>

namespace olc::imload
{
    // Create an image resource based on an image file asset on disk
    bool ImageLoader_LibPNG::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        ////////////////////////////////////////////////////////////////////////////
        // Use libpng, Thanks to Guillaume Cottenceau
        // https://gist.github.com/niw/5963798
        // Also reading png from streams
        // http://www.piko3d.net/tutorials/libpng-tutorial-loading-png-files-from-streams/
        png_structp png;
        png_infop info;

        auto loadPNG = [&]()
            {
                png_read_info(png, info);
                png_byte color_type;
                png_byte bit_depth;
                png_bytep* row_pointers;
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
                row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * image.Size().y);
                for (int y = 0; y < image.Size().y; y++) {
                    row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png, info));
                }
                png_read_image(png, row_pointers);

                // Iterate through image rows, converting into sprite format
                for (int y = 0; y < image.Size().y; y++)
                {
                    png_bytep row = row_pointers[y];
                    for (int x = 0; x < image.Size().x; x++)
                    {
                        png_bytep px = &(row[x * 4]);
                        image.Pixel(olc::vi2d(x, y)) = olc::Pixel(px[0], px[1], px[2], px[3]);
                    }
                }

                for (int y = 0; y < image.Size().y; y++) // Thanks maksym33
                    free(row_pointers[y]);
                free(row_pointers);
                png_destroy_read_struct(&png, &info, nullptr);
            };

        png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (!png)
            return false;

        info = png_create_info_struct(png);
        if (!info)
            return false;

        if (setjmp(png_jmpbuf(png)))
            return false;

        {
            FILE* f = fopen(sFileName.c_str(), "rb");
            if (!f) return false;
            png_init_io(png, f);
            loadPNG();
            fclose(f);
        }

        return true;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_LibPNG::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        return false;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_LibPNG::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return false;
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

}
//! END IMPLEMENTATION
