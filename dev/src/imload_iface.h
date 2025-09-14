#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "image.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_IMAGELOADER_IFACE_DECLARED)
namespace olc
{
	namespace imload
	{
		class ImageLoader
		{
		public:
			ImageLoader() = default;
			virtual ~ImageLoader() = default;

			// Create an image resource based on an image file asset on disk
			virtual bool CreateImageFromFile(olc::Image& image, const std::string& sFileName) = 0;
			
			// Create an image resource based on an image file asset in memory
			virtual bool CreateImageFromMemory(olc::Image& image, const uint8_t* data, const size_t bytes) = 0;

			// Create an image resource based on an image file asset in memory
			virtual bool CreateImageFromMemory(olc::Image& image, const std::vector<uint8_t>& data) = 0;
			
			// Store an image as a file asset on disk
			virtual bool WriteImageToFile(const olc::Image& image, const std::string& sFileName) = 0;
			
			// Store an image as a file asset in memory
			virtual bool WriteImageToMemoryFile(olc::Image& image, const std::vector<uint8_t>& data) = 0;

		};
	}
}
#define PGE_IMAGELOADER_IFACE_DECLARED 1
#endif
//! END DECLARATION