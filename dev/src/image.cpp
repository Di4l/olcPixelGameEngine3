#include "image.h"

//! START IMPLEMENTATION
namespace olc
{
	ImageCPU::ImageCPU(const olc::vi2d& size) : dimensions(size)
	{
		pixels.resize(dimensions.area(), olc::Pixel(255, 165, 0));
	}

	ImageCPU::~ImageCPU()
	{
		pixels.clear();
	}

	const olc::vi2d& ImageCPU::Size() const
	{
		return dimensions;
	}

	olc::Pixel* ImageCPU::Data()
	{
		return pixels.data();
	}


	Image::Image(const olc::vi2d& size, const bool onCPU, const bool onGPU)
	{
		// Always create CPU resident image
		imCPU = std::make_unique<olc::ImageCPU>(size);

		if (onGPU)
		{

		}

		if (!onCPU)
			imCPU.reset();
	}

	bool Image::InCPU() const
	{
		return imCPU != nullptr;
	}

	bool Image::InGPU() const
	{
		return imGPU != nullptr;
	}

};
//! END IMPLEMENTATION