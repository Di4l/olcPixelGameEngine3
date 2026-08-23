#include "imload_stb_image.h"

//! START IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace olc::imload
{

    // Create an image resource based on an image file asset on disk
    bool ImageLoader_STB_Image::CreateImageFromFile(olc::Image& image, const std::string& sFileName)
    {
        std::cout << "ImageLoader: using stb image to load " << sFileName << ".\n";
        
        // Open file
        if(!std::filesystem::exists(sFileName))
        {
            std::cout << "Error: failed to load image <" << sFileName << "> - file not found.\n";
            return false;
        }
        
        stbi_uc* bytes = nullptr;
        int width = 0, height = 0, cmp = 0;
        bytes = stbi_load(sFileName.c_str(), &width, &height, &cmp, 4);

        if(!bytes)
        {
            std::cout << "Error: failed to load image <" << sFileName << "> - failed to allocate memory.\n";
            return false;
        }
        
        image.CreateNoGPU({width, height});
        std::memcpy(reinterpret_cast<void*>(image.Data()), bytes, width * height * 4);

        delete[] bytes;
        
        return true;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_STB_Image::CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes)
    {
        stbi_uc* pixelData = nullptr;
        int width = 0, height = 0, cmp = 0;
        pixelData = stbi_load_from_memory(data, bytes, &width, &height, &cmp, 4);
        if(!pixelData)
            return false;

        image.CreateNoGPU({width, height});
        std::memcpy(reinterpret_cast<void*>(image.Data()), pixelData, width * height * 4);
        
        delete[] pixelData;

        return true;
    }
    
    // Create an image resource based on an image file asset in memory
    bool ImageLoader_STB_Image::CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data)
    {
        stbi_uc* pixelData = nullptr;
        int width = 0, height = 0, cmp = 0;
        pixelData = stbi_load_from_memory(data.data(), data.size(), &width, &height, &cmp, 4);
        if(!pixelData)
            return false;

        image.CreateNoGPU({width, height});
        std::memcpy(reinterpret_cast<void*>(image.Data()), pixelData, width * height * 4);
        
        delete[] pixelData;

        return true;
    }

    // Store an image as a file asset on disk
    bool ImageLoader_STB_Image::WriteImageToFile(const olc::Image& image, const std::string& sFileName) 
    {
        return false;
    }
    
    // Store an image as a file asset in memory
    bool ImageLoader_STB_Image::WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data)
    {
        return false;
    }

}
//! END IMPLEMENTATION
