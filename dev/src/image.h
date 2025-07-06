#pragma once

//! START STDHEADER
#include <cmath>
#include <cstdint>
#include <string>
#include <array>
#include <vector>
#include <memory>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "vector2d.h"
#include "pixel.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_IMAGE_DECLARED)
namespace olc
{
	struct ImageConfig
	{
		bool Filtered = false;
		bool Clamp = false;
	};

	class ImageCPU
	{
	public:
		ImageCPU() = default;
		ImageCPU(const olc::vi2d& size);
		virtual ~ImageCPU();

	public:
		const olc::vi2d& Size() const;
		olc::Pixel* Data();

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
		Image(const olc::vi2d& size, const bool onCPU = true, const bool onGPU = true);

	public:
		bool InCPU() const;
		bool InGPU() const;


	protected:
		std::unique_ptr<olc::ImageCPU> imCPU = nullptr;
		std::unique_ptr<olc::ImageGPU> imGPU = nullptr;
	};
}
#define PGE_IMAGE_DECLARED 1
#endif
//! END DECLARATION
