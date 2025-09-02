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
		bool InRAM = true;
		bool InVRAM = true;
	};

	class Image
	{
	public:
		// Constructs a general purpose image
		Image() = default;
		virtual ~Image() = default;

	public:
		bool Create(const olc::vi2d& size, const ImageConfig& cfg = olc::ImageConfig());

	public:
		// Returns size (x, y) in pixels
		const olc::vi2d& Size() const;
		// Returns read/write pointer to start of 1D stream of pixel data
		olc::Pixel* Data();
		// Returns how this image was configured upon creation
		const ImageConfig& GetConfig() const;
		// Return GPU Resource ID
		int32_t GetGPUID() const;
		// Set GPU Resource ID (0 to eliminate)
		void SetGPUID(const int32_t id);
		
		// True if data mirror has in-front RAM image		
		bool HasWetCPU() const;
		// True if data mirror has in-front VRAM image
		bool HasWetGPU() const;

		void PrimeCPU();
		void PrimeGPU();

	protected:
		ImageConfig config;
		olc::vi2d dimensions;
		std::vector<olc::Pixel> pixels;
		int32_t gpuResourceID = 0;
		bool gpuWet = false;
		bool cpuWet = true;
	};
}
#define PGE_IMAGE_DECLARED 1
#endif
//! END DECLARATION
