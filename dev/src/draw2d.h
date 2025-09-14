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
#include "transform2d.h"
#include "pixel.h"
#include "image.h"
#include "gputask.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_DRAW2D_DECLARED)
namespace olc
{
	namespace gpu
	{
		class Renderer;
	}
	

	class Draw2D
	{
	public:
		Draw2D();

		// Associate this drawing toolbox with a renderer
		void SetGPU(olc::gpu::Renderer* const renderer);

	public:
		// Sets the drawing target of this drawing toolbox
		void SetTarget(olc::Image& image);

		// Clears transform stack and sets identity transform
		void ClearTransform();

		void SetTransform(const olc::tf2d& trans);

		olc::tf2d& GetTransform();

		void ProcessGPUTasks();

	protected:
		// Checks residency of image resource, and brings it to cpu RAM for r/w
		void PrepareTargetForSW();
		// Checks residency of image resource, and brings it to gpu VRAM for r/w
		void PrepareTargetForHW();

		// Checks residency of image resource, and brings it to cpu RAM for r/w
		void PrepareImageForSW(olc::Image& image);
		// Checks residency of image resource, and brings it to gpu VRAM for r/w
		void PrepareImageForHW(olc::Image& image);

		olc::Image* pTarget = nullptr;
		olc::gpu::Renderer* pRenderer = nullptr;
		olc::tf2d transformTarget;
		olc::tf2d transformAffine;
		olc::mf3d transformCombined;

		std::vector<olc::GPUTask> vecGPUTasks;

	public: // Affine Transformation (these affect all subsequent draw calls for this target)
		void AffineReset();
		void AffineScale(const olc::vf2d& vScale);
		void AffineOffset(const olc::vf2d& vOffset);
		void AffineRotate(const float& fTheta, const olc::vf2d& vPoint = { 0,0 });

	public:
		// Plot a single pixel
		void Pixel(const olc::vf2d& pos, const olc::Pixel col = olc::Colour::WHITE);
		// Read a pixel from an image (guarantees fresh)
		olc::Pixel GetPixel(olc::Image& image, const olc::vf2d& pos);
	
	public:
		// Draws a single pixel wide line		
		GPUTask Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col = olc::Colour::WHITE);
		// Draws a single pixel wide line with a gradient		
		GPUTask Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel c1, const olc::Pixel c2);

		// Draws a rectangle outline
		GPUTask Rect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col = olc::Colour::WHITE);
		// Draws a filled, single colour rectangle
		GPUTask FillRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col = olc::Colour::WHITE);
		
		// Draws an image
		GPUTask Image(olc::Image& image, const olc::vf2d& pos, const olc::vf2d& size);
	



	public: // GPU Task Creator Functions (not normally called by user)
		GPUTask TaskDrawPolygon(
			GPUTask::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskDrawPolygon(
			GPUTask::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const olc::Pixel colour,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillPolygon(
			GPUTask::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillPolygon(
			GPUTask::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const olc::Pixel colour,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskTexturedPolygon(
			GPUTask::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);
	
	};
}
#define PGE_DRAW2D_DECLARED
#endif
//! END DECLARATION