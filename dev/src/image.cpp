#include "image.h"

//! START IMPLEMENTATION
namespace olc
{
	/*Image::Image(const olc::vi2d& size, const ImageConfig& cfg)
	{
	}

	Image::~Image()
	{
	}*/

	bool Image::Create(const olc::vi2d& size, const ImageConfig& cfg)
	{
		dimensions = size;
		config = cfg;
		pixels.resize(dimensions.area(), olc::Pixel(255, 165, 0));
		return true;
	}

	const olc::vi2d& Image::Size() const
	{
		return dimensions;
	}

	olc::Pixel* Image::Data()
	{
		return pixels.data();
	}

	const ImageConfig& Image::GetConfig() const
	{
		return config;
	}

	int32_t Image::GetGPUID() const
	{
		return gpuResourceID;
	}

	void Image::SetGPUID(const int32_t id)
	{
		gpuResourceID = id;
	}

};
//! END IMPLEMENTATION