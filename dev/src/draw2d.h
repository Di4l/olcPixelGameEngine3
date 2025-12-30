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
		// Reset world transform to identity
		void WorldReset();
		// Apply world scaling
		void WorldScale(const olc::vf2d& vScale);
		// Apply world translation
		void WorldOffset(const olc::vf2d& vOffset);
		// Apply world rotation
		void WorldRotate(const float& fTheta, const olc::vf2d& vPoint = { 0,0 });
		
		// World transforms
		void SetWorldTransform(const olc::tf2d& trans);
		olc::tf2d& GetWorldTransform();

		// Coordinate transforms
		olc::vf2d WorldToScreen(const olc::vf2d& v) const;
		olc::vf2d ScreenToWorld(const olc::vf2d& v) const;

	public: // Primitive Drawing Functions
		// Plot a single pixel
		void Pixel(
			const olc::vf2d& pos, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Read a pixel from an image (guarantees fresh)
		olc::Pixel GetPixel(
			olc::Image& image, 
			const olc::vf2d& pos);

		// Clear entire draw target to specific colour
		void Clear(const olc::Pixel& col);
	
	public: // Shape Drawing Functions


		// === Lines ===

		// Draws a single pixel wide line		
		const GPUTask& Line(
			const olc::vf2d& p1, 
			const olc::vf2d& p2, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a single pixel wide line with a gradient		
		const GPUTask& Line(
			const olc::vf2d& p1, 
			const olc::vf2d& p2, 
			const olc::Pixel c1, 
			const olc::Pixel c2,
			const olc::Pixel tint = olc::Colour::WHITE);

		// === Rectangles ===

		// Draws a rectangle outline
		const GPUTask& Rect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a multiple colour rectangle, with linear colour interpolation
		const GPUTask& Rect(
			const olc::vf2d& pos,
			const olc::vf2d& size,
			const olc::Pixel colTL,
			const olc::Pixel colTR,
			const olc::Pixel colBL,
			const olc::Pixel colBR,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, single colour rectangle
		const GPUTask& FilledRect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, multiple colour rectangle, with linear colour interpolation
		const GPUTask& FilledRect(
			const olc::vf2d& pos, 
			const olc::vf2d& size, 
			const olc::Pixel colTL, 
			const olc::Pixel colTR, 
			const olc::Pixel colBL, 
			const olc::Pixel colBR,
			const olc::Pixel tint = olc::Colour::WHITE);

		// === Circles ===
		
		// Draws a circle outline with a single colour
		const GPUTask& Circle(
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = 32);

		// Draws a filled circle with a single colour
		const GPUTask& FilledCircle(
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = 32);

		// Draws a shaded circle with a radial gradient
		const GPUTask& FilledCircle(
			const olc::vf2d& pos,
			const float& radius,
			const olc::Pixel colInner,
			const olc::Pixel colOuter,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = 32);

		// === Ellipses ===

		// Draws an ellipse outline with a single colour
		const GPUTask& Ellipse(
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = 32);

		// Draws a filled ellipse with a single colour
		const GPUTask& FilledEllipse(
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = 32);

		// Draws a shaded ellipse with a radial gradient
		const GPUTask& FilledEllipse(
			const olc::vf2d& pos,
			const float& rx,
			const float& ry,
			const olc::Pixel colInner,
			const olc::Pixel colOuter,
			const olc::Pixel tint = olc::Colour::WHITE,
			int32_t nFacets = 32);




		//RoundRect
		//FilledRoundRect
		//ShadedRoundRect
		//TexturedRoundRect

		// === Triangles ===

		// Draws a triangle outline with a single colour
		const GPUTask& Triangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a multiple colour triangle outline
		const GPUTask& Triangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, single colour triangle
		const GPUTask& FilledTriangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled, multiple colour triangle
		const GPUTask& FilledTriangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a textured triangle, with per vertex colouring
		const GPUTask& TexturedTriangle(
			const olc::vf2d& p1,
			const olc::vf2d& p2,
			const olc::vf2d& p3,
			const olc::Pixel c1,
			const olc::Pixel c2,
			const olc::Pixel c3,
			const olc::vf2d& t1,
			const olc::vf2d& t2,
			const olc::vf2d& t3,
			olc::Image& texture,
			const olc::Pixel tint = olc::Colour::WHITE);

		// === Polygons ===

		// Draws a polygon outline with a single colour
		const GPUTask& Polygon(
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a polygon outline with multiple colours
		const GPUTask& Polygon(
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a polygon outline with a single colour
		const GPUTask& Polygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE, 
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a polygon outline with multiple colours
		const GPUTask& Polygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled polygon with a single colour
		const GPUTask& FilledPolygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const olc::Pixel col = olc::Colour::WHITE,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a filled polygon with multiple colours
		const GPUTask& FilledPolygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		// Draws a textured polygon with per vertex colouring
		const GPUTask& TexturedPolygon(
			const olc::Structure structure,
			const std::vector<olc::vf2d>& vecPoints,
			const std::vector<olc::Pixel>& vecColours,
			const std::vector<olc::vf2d>& vecTexCoords,
			olc::Image& texture,
			const olc::Pixel tint = olc::Colour::WHITE);


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

		// Returns the bounding box size of a string in pixels
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
		GPUTask TaskDrawLine(
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);
		
		GPUTask TaskDrawPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskDrawPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const olc::Pixel colour,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskFillPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const olc::Pixel colour,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskTexturedPolygon(
			olc::Structure structure,
			const std::vector<olc::vf2d>& vPoints,
			const std::vector<olc::Pixel>& vColours,
			const std::vector<olc::vf2d>& vTexCoords,
			olc::Image* const image,
			const olc::Pixel tint = olc::Colour::WHITE);

		GPUTask TaskTexturedPolygon(
			olc::Structure structure,
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

			// Draws a rectangle outline
			void swRect(
				const olc::vf2d& pos,
				const olc::vf2d& size,
				const olc::Pixel col = olc::Colour::WHITE);

			// Draws a multiple colour rectangle, with linear colour interpolation
			void swRect(
				const olc::vf2d& pos,
				const olc::vf2d& size,
				const olc::Pixel colTL,
				const olc::Pixel colTR,
				const olc::Pixel colBL,
				const olc::Pixel colBR);

			// Draws a filled, single colour rectangle
			void swFilledRect(
				const olc::vf2d& pos,
				const olc::vf2d& size,
				const olc::Pixel col = olc::Colour::WHITE);

			// Draws a filled, multiple colour rectangle, with linear colour interpolation
			void swFilledRect(
				const olc::vf2d& pos,
				const olc::vf2d& size,
				const olc::Pixel colTL,
				const olc::Pixel colTR,
				const olc::Pixel colBL,
				const olc::Pixel colBR);

			// Draws a triangle outline
			void swTriangle(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::vf2d& p3,
				const olc::Pixel col = olc::Colour::WHITE);

			// Draws a multiple colour triangle, with linear colour interpolation
			void swTriangle(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::vf2d& p3,
				const olc::Pixel c1,
				const olc::Pixel c2,
				const olc::Pixel c3);

			// Draws a filled, single colour triangle
			void swFilledTriangle(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::vf2d& p3,
				const olc::Pixel col = olc::Colour::WHITE);

			// Draws a filled, multiple colour triangle, with linear colour interpolation
			void swFilledTriangle(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::vf2d& p3,
				const olc::Pixel c1,
				const olc::Pixel c2,
				const olc::Pixel c3);

			// Rasterises a textured triangle in integer space
			void swTexturedTriangle(
				const olc::vf2d& p1,
				const olc::vf2d& p2,
				const olc::vf2d& p3,
				const olc::Pixel c1,
				const olc::Pixel c2,
				const olc::Pixel c3,
				const olc::vf2d& t1,
				const olc::vf2d& t2,
				const olc::vf2d& t3,
				olc::Image& texture);



		protected: // Software rasteriser helper functions

			// Clips a line to a rectangular region, returns true if line is visible
			bool swClipLine(
				olc::vf2d& v0,
				olc::vf2d& v1,
				const olc::vf2d& vMin,
				const olc::vf2d& vMax);

			// Clips a line to a rectangular region, returns true if line is visible.
			// The returned weights correspond to distance along the line from v0 to v1
			bool swClipWeightedLine(
				olc::vf2d& v0,
				olc::vf2d& v1,
				const olc::vf2d& vMin,
				const olc::vf2d& vMax,
				float& w0,
				float& w1);

			/* bool swClipTriangle(
				olc::vf2d& v1,
				olc::vf2d& v2,
				olc::vf2d& v3,
				const olc::vf2d& vMin,
				const olc::vf2d& vMax);*/
				
			// Rasterises a shaded line in integer space
			void swRasterShadedLine(
				const olc::vi2d& v1,
				const olc::vi2d& v2,
				const olc::Pixel c1,
				const olc::Pixel c2);

			// Rasterises a shaded triangle in integer space
			void swRasterShadedTriangle(
				const olc::vi2d& v1,
				const olc::vi2d& v2,
				const olc::vi2d& v3,
				const olc::Pixel c1,
				const olc::Pixel c2,
				const olc::Pixel c3);

			// Rasterises a textured triangle in integer space
			void swRasterTexturedTriangle(
				const olc::vi2d& v1,
				const olc::vi2d& v2,
				const olc::vi2d& v3,
				const olc::Pixel c1,
				const olc::Pixel c2,
				const olc::Pixel c3,
				const olc::vf2d& t1,
				const olc::vf2d& t2,
				const olc::vf2d& t3,
				olc::Image& texture);



		




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

		protected: // SW Rasteriser Helpers
			struct Scanline
			{
				int32_t nMin = std::numeric_limits<int32_t>::max();
				int32_t nMax = std::numeric_limits<int32_t>::min();
				std::array<float, 3> fBaryMin;
				std::array<float, 3> fBaryMax;
			};

			std::vector<Scanline> vScanlines;


			// Fills scanline buffer with visible triangle extents and barycentric coordinates.
			// Returns vertical, visible extents of triangle scanlines
			std::pair<int, int> swBaryFillTriangle(
				const olc::vi2d& v1,
				const olc::vi2d& v2,
				const olc::vi2d& v3);
	
	};
}
#define PGE_DRAW2D_DECLARED
#endif
//! END DECLARATION