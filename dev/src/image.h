#pragma once

//! STDHEADER START
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
//! STDHEADER END

//! CUSTOMHEADER START
#include "config.h"
#include "vector2d.h"
#include "pixel.h"
//! CUSTOMHEADER END

//! DECLARATION START 
#if !defined(PGE_IMAGE_DECLARED)
namespace olc
{
	class ImageCPU
	{
	public:
		ImageCPU() = default;

		ImageCPU(const olc::vi2d& size) : dimensions(size)
		{
			pixels.resize(dimensions.area(), olc::Pixel(255,165,0));
		}

		virtual ~ImageCPU()
		{
			pixels.clear();
		}


	public:
		const olc::vi2d& Size() const
		{
			return dimensions;
		}

		olc::Pixel* Data()
		{
			return pixels.data();
		}

	public:
		olc::vi2d dimensions;
		std::vector<olc::Pixel> pixels;
	};

	struct ImageGPU
	{
		olc::vf2d size;
		olc::vf2d invsize;
		int32_t resourceID = -1;
	};

	class Image
	{
	public:
		// Constructs a general purpose image
		Image(const olc::vi2d& size, const bool onCPU = true, const bool onGPU = true)
		{
			// Always create CPU resident image
			imCPU = std::make_unique<olc::ImageCPU>(size);
			
			if (onGPU)
			{
				
			}

			if (!onCPU)
				imCPU.reset();
		}


	public:
		inline constexpr bool InCPU() const
		{
			return imCPU != nullptr;
		}

		inline constexpr bool InGPU() const
		{
			return imGPU != nullptr;
		}

	protected:
		std::unique_ptr<olc::ImageCPU> imCPU = nullptr;
		std::unique_ptr<olc::ImageGPU> imGPU = nullptr;
	};
}
#define PGE_IMAGE_DECLARED 1
#endif
//! DECLARATION END
