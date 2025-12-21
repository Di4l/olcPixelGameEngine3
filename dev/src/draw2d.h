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
#include "font.h"
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
		void ProcessGPUTasks();

	public:
		// Sets the drawing target of this drawing toolbox
		void SetTarget(olc::Image& image);



	

	public: // Affine Transformation (these affect all subsequent draw calls for this target)
		void WorldReset();
		void WorldScale(const olc::vf2d& vScale);
		void WorldOffset(const olc::vf2d& vOffset);
		void WorldRotate(const float& fTheta, const olc::vf2d& vPoint = { 0,0 });
		void SetWorldTransform(const olc::tf2d& trans);
		olc::tf2d& GetWorldTransform();
		olc::vf2d WorldToScreen(const olc::vf2d& v) const;
		olc::vf2d ScreenToWorld(const olc::vf2d& v) const;

	public: // Primitive Drawing Functions
		// Plot a single pixel
		void Pixel(
			const olc::vf2d& pos, 
			const olc::Pixel col = olc::Colour::WHITE);

		// Read a pixel from an image (guarantees fresh)
		olc::Pixel GetPixel(
			olc::Image& image, 
			const olc::vf2d& pos);

		// Clear entire draw target to specific colour
		void Clear(const olc::Pixel& col);
	
	public: // Shape Drawing Functions
		// Draws a single pixel wide line		
		const GPUTask& Line(
			const olc::vf2d& p1, 
			const olc::vf2d& p2, 
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a single pixel wide line with a gradient		
		const GPUTask& Line(
			const olc::vf2d& p1, 
			const olc::vf2d& p2, 
			const olc::Pixel c1, 
			const olc::Pixel c2);

		// Draws a rectangle outline
		const GPUTask& Rect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a filled, single colour rectangle
		const GPUTask& FilledRect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel col = olc::Colour::WHITE);

		// Draws a filled, multiple colour rectangle, with linear colour interpolation
		const GPUTask& FilledRect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel colTL, 
			const olc::Pixel colTR, 
			const olc::Pixel colBL, 
			const olc::Pixel colBR);
		
		
		//TexturedRect


		//Circle
		//FilledCircle
		//ShadedCircle
		//TexturedCircle

		//Ellipse
		//FilledEllipse
		//ShadedEllipse
		//TexturedEllipse

		//RoundRect
		//FilledRoundRect
		//ShadedRoundRect
		//TexturedRoundRect

		//Triangle
		//FilledTriangle
		//ShadedTriangle
		//TexturedTriangle

		//Polygon
		//FilledPolygon
		//ShadedPolygon
		//TexturedPolygon

	public: // Text Drawing Functions
		// Draws a string at specified location in monospace font
		const GPUTask& String(
			const olc::vf2d& pos,
			const std::string& text, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			olc::Font& font = olc::fontClassicPGE);

		// Draws a string at specified location in proportional font
		const GPUTask& StringProp(
			const olc::vf2d& pos,
			const std::string& text,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			olc::Font& font = olc::fontClassicPGE);

		olc::vf2d GetTextSize(
			const std::string& text,
			const bool bProportional = false,
			const olc::vf2d& scale = { 1.0f, 1.0f },
			olc::Font& font = olc::fontClassicPGE);

	public: // Image Drawing Functions		
		// Draws a scaled image at specified location
		const GPUTask& Image(
			olc::ImageRegion image, 
			const olc::vf2d& pos, 
			const olc::vf2d& scale = { 1.0f, 1.0f }, 
			const olc::Pixel tint = olc::Colour::WHITE);						
		
		// Draws an image rotated around a point at specified location
		const GPUTask& ImageRotated(
			olc::ImageRegion image, 
			const olc::vf2d& pos, 
			const float theta, 
			const olc::vf2d& center = { 0.0f, 0.0f }, 
			const olc::vf2d& scale = { 1.0f, 1.0f },
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image warped correctly to linearly fill a quadrilateral (formerly DrawWarped...)		
		const GPUTask& ImageQuad(
			olc::ImageRegion image, 
			const olc::vf2d& vTL, 
			const olc::vf2d& vTR, 
			const olc::vf2d& vBR, 
			const olc::vf2d& vBL,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image warped correctly to linearly fill a quadrilateral (formerly DrawWarped...)
		const GPUTask& ImageQuad(
			olc::ImageRegion image, 
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws an image scaled to a specified rectangular area
		const GPUTask& ImageRect(
			olc::ImageRegion image, 
			const olc::vf2d& pos, 
			const olc::vf2d& size,
			const olc::Pixel tint = olc::Colour::WHITE);
	



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

		GPUTask TaskTexturedPolygon(
			GPUTask::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::vf2d>& vZWs,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);


		public: // Precision drawing functions via software rasteriser
			// Draws a single pixel wide line of fixed colour
			void swLine(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::Pixel col = olc::Colour::WHITE);

			// Draws a single pixel wide line with a gradient		
			void swLine(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::Pixel c1,
				const olc::Pixel c2);
		




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
			olc::tf2d transformAffine;

			std::vector<olc::GPUTask> vecGPUTasks;
	
	};
}
#define PGE_DRAW2D_DECLARED
#endif
//! END DECLARATION