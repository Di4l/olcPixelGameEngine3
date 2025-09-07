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

		const olc::tf2d& GetTransform();

		void ProcessGPUTasks();

	protected:
		// Checks residency of image resource, and brings it to cpu RAM for r/w
		void PrepareTargetForSW();
		// Checks residency of image resource, and brings it to gpu VRAM for r/w
		void PrepareTargetForHW();

		olc::Image* pTarget = nullptr;
		olc::gpu::Renderer* pRenderer = nullptr;
		olc::tf2d transform;

		std::vector<olc::GPUTask> vecGPUTasks;

	public:
		// Plot a single pixel
		void Pixel(const olc::vf2d& pos, const olc::Pixel col = olc::Colour::WHITE);


	public: // GPU Task Creator Functions
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



	public:
		// Draws a single pixel wide line		
		GPUTask Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col = olc::Colour::WHITE);
		// Draws a single pixel wide line with a gradient		
		GPUTask Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel c1, const olc::Pixel c2);

		// Draws a filled, single colour rectangle
		GPUTask FillRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col = olc::Colour::WHITE);

		// Draws a filled, single colour rectangle
		GPUTask Image(olc::Image& image, const olc::vf2d& pos, const olc::vf2d& size);
	};
}
#define PGE_DRAW2D_DECLARED
#endif
//! END DECLARATION