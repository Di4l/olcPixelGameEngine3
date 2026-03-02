/*
	olc::PixelGameEngine3 Example - Image Tiling

	Shows a pannable & zoomable & rotatable tiled image background

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// NOTE! This is example is slightly more complicated because it also
// does the following things:
// 1) It implements a pannable, zoomable & rotating world using the
//    olc::tf2d affine transform class built into PGE3
// 2) It uses "draw batching" to speed up the rendering of many tiles
//    by reducing the number of draw calls made to the graphics hardware
// 3) It roughly draws tiles that are visible within the current rotated
//    view, though this is highly approximate

// EXPERIMENTS!
// 1) Try changing the size of the constructed "screen" to see how it
//    affects performance and tile sampling quality. For example:
//    demo.Construct({ 1024, 960 }, { 1, 1 }))
// 2) Remove the draw batching code to see how it affects performance
//    by just using direct draw calls.

// Example application demonstrating image tiling. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_ImageTiling : public olc::PixelGameEngine
{
public:
	Example_ImageTiling()
	{
		sAppName = "Example Image Tiling";
	}

protected:
	// This is our tile atlas image, it contains 16 tiles arranged
	// in a 4x4 grid, each tile is 16x16 pixels in size
	olc::Image imTiles;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		
		olc::tf2d t1;
		t1.translate(olc::vf2d{ 10,5 });
		t1.scale(olc::vf2d{ 2,2 });
		t1.translate(olc::vf2d{ 10,5 });
		t1.scale(olc::vf2d{ 4.0f, 4.0f });
		t1.translate(olc::vf2d{ 10,5 });
		olc::vf2d v = t1.translation();
		olc::vf2d s = t1.scale();

		auto i1 = t1.forward_matrix().invert();
		auto i2 = i1.invert();

		CreateImageFromFile(imTiles, "./assets/tile_atlas_4x4_16x16.png");

		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to dark blue
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// See Below for implementation
		HandlePanAndZoom();

		// Work out roughly which tiles are visible on screen. Because
		// the world can be rotated, this is only approximate but is
		// important to reduce the number of tiles we attempt to draw.

		// Of course, without rotation, this is can be calculated exactly
		// and would be a simple rectangle.

		// Get four corners of screen in world space
		olc::vf2d vTopLeft = draw.ScreenToWorld(olc::vf2d{ 0.0f, 0.0f });
		olc::vf2d vBottomRight = draw.ScreenToWorld(draw.GetTargetSize());

		// Work out radius of circle required to cover screen, i.e. passes
		// through all four corners. This is in world space units. 1 unit 
		// of world space is a 16x16 tile. These extents are in tiles and
		// assume the origin of the world
		float fScreenRadius = (vBottomRight - vTopLeft).mag() * 0.5f;
		olc::vi2d vVisibleGridExtents = {
			(int)(std::ceil(fScreenRadius / 16.0f)),
			(int)(std::ceil(fScreenRadius / 16.0f))
		};

		// Find center of screen in world space
		olc::vf2d vScreenCenter = draw.ScreenToWorld(draw.GetTargetSize() * 0.5f);

		// Snap center to nearest tile to avoid jitter/locking when panning
		olc::vf2d vSnappedCenter = {
				std::round(vScreenCenter.x / 16.0f) * 16.0f,
				std::round(vScreenCenter.y / 16.0f) * 16.0f
		};


		size_t nTileCount = 0;

		// Create a draw batch to speed up rendering. We can do this because
		// all tiles are from the same image source. This minimises the number
		// of draw calls we make to the graphics hardware.
		auto batch = draw.CreateImageBatch(imTiles);

		// Draw visible tiles
		for (int y = -vVisibleGridExtents.y; y < vVisibleGridExtents.y; y++)
		{
			for (int x = -vVisibleGridExtents.x; x < vVisibleGridExtents.x; x++)
			{
				// Tile position relative to origin
				olc::vi2d vTilePos = olc::vi2d{ x, y };

				// Translate tile position to world space
				olc::vf2d vWorldPos = vSnappedCenter + (vTilePos * 16.0f);

				// Only draw tiles that are actually visible, accounting for rotation
				// This is highly approximate, but good enough for our needs and has
				// on average 100% error XD
				if ((vWorldPos - vScreenCenter).mag2() > (fScreenRadius + 16.0f) * (fScreenRadius + 16.0f))
					continue;
			
				// Determine which tile to draw from tile atlas. We can't use rand()
				// here because we want the same tiles to appear in the same place 
				// every frame, so we use a simple manipulation based on tile position
				// in world space.
				long a = (vTilePos.x + int(vSnappedCenter.x / 16.0f));
				long b = (vTilePos.y + int(vSnappedCenter.y / 16.0f));

				// Wrap tile indices to 0-3 range, including negative coords
				long idx = ((b + 4) % 4);
				long idy = ((a + 4) % 4);

				// Tile index in tile atlas in tile units
				olc::vi2d vTileIndex = { (int)idy, (int)idx };

				// Extract tile from tile atlas using the region() function, specifying
				// in pixels the top left corner of the tile and its size.
				// Draw extracted tile at calculated world position. The applied affine
				// transform will take care of rotation, scaling and translation
				// We are drawing to a batch for speed.
				draw.Image(batch, imTiles.region(vTileIndex * 16.0f, { 16, 16 }), vWorldPos);

				// Experiment 2
				// Uncomment below, and comment above to see individual draw calls instead of 
				// batching. This will be much slower!
				//draw.Image(imTiles.region(vTileIndex * 16.0f, { 16, 16 }), vWorldPos);

				// Count tiles drawn
				nTileCount++;
			}
		}

		// Finally, draw the batch to the screen
		draw.Batch(batch);

		// Reset the world transform to draw to the screen directly
		draw.WorldReset();
		// Display number of tiles drawn with a shadow
		draw.String({ 3,3 }, "Tiles: " + std::to_string(nTileCount), olc::Colour::BLACK);
		draw.String({ 2,2 }, "Tiles: " + std::to_string(nTileCount), olc::Colour::WHITE);
		
		// Successful frame
		return true;
	}

	
	
	// Panning & Zooming
	// ~~~~~~~~~~~~~~~~~
	// We use an olc::tf2d to keep track of the world. This is an "affine transform"
	// which is used by PGE to convert between coordinates in the world of tiles and
	// coordinates of the screen in pixels.

	// We can map mouse gestures to changes in this transform to achieve panning and 
	// zooming, and now in PGE3, rotation too!

	// Panning is changing the translation component of the transform
	//	- Hold down the mouse wheel and drag to pan
	// Zooming is changing the scale component of the transform
	//  - Use the mouse wheel to zoom in and out
	// Rotation is changing the rotation component of the transform
	//  - Hold down the right mouse button and use the mouse wheel to rotate

	// The trick to all this is to ensure that the point under the mouse cursor
	// remains "fixed" in world space as we manipulate the transform. This
	// requires a little calculation before and after changing the transform
	// components.

	// We also reset the transform if the left mouse button is clicked.

	// We need some state variables to help us out:
	bool bPanning = false;
	olc::vf2d vLastMouseScreenPos = { 0.0f, 0.0f };


	// The "Affine Transform" representing the world
	olc::tf2d transform;

	// Function to handle panning and zooming
	void HandlePanAndZoom()
	{
		// if left mouse button clicked, reset transform
		if (mouse.GetButton(0).bPressed)
		{
			transform = olc::tf2d();
		}

		// Enter panning mode if middle mouse button pressed (and held)
		if (mouse.GetButton(2).bPressed)
		{
			bPanning = true;
			// Cache mouse position at start of drag, so we can create a
			// mouse position delta
			vLastMouseScreenPos = mouse.GetPosition();
		}

		// Exit panning mode if middle mouse button released
		if (mouse.GetButton(2).bReleased)
		{
			bPanning = false;
		}

		// Squash current transform to a single matrix. This is required
		// so we dont contnuously add to the transform stack
		transform.squash();

		// Get current mouse position		
		olc::vf2d vMousePos = mouse.GetPosition();
		olc::vf2d vMouseWorldPos = transform.inverse(vMousePos);
		olc::vf2d vLastMouseWorldPos = transform.inverse(vLastMouseScreenPos);	

		// NOTE!!! Scale & Rotate BEFORE translation
		
		// Handle zooming and rotation. This is a bit clumsy because we are
		// using the mouse wheel for both. In a real application you would
		// probably want to use keyboard modifiers to distinguish the two.
		if (mouse.GetWheel() != 0)
		{

			// Cache the mouse position before transformation			
			auto posWorldBefore = vMouseWorldPos;

			// If right mouse button held, we are rotating
			if (mouse.GetButton(1).bHeld)
			{				
				float fRotateDelta = (mouse.GetWheel() > 0) ? 0.1f : -0.1f;
				transform.rotate(fRotateDelta, posWorldBefore);				
			}
			else
			{
				olc::vf2d vScaleDelta = olc::vf2d{ 1.0f, 1.0f } * ((mouse.GetWheel() > 0) ? 1.1f : 0.9f);
				transform.scale(vScaleDelta);
			}

			// Get the new screen position of the point under the mouse
			auto posWorldAfter = transform.inverse(mouse.GetPosition());

			// Adjust translation to keep mouse position stable
			auto posScreenDisplacement = posWorldBefore - posWorldAfter;

			// Apply adjustment
			transform.translate(-posScreenDisplacement);
		}
		

		// If we are panning, update translation component of transform
		if (bPanning)
		{
			// Update translation by the mouse delta
			transform.translate(vMouseWorldPos - vLastMouseWorldPos);
		}
		
		

		// Finally, apply the updated transform to the PGE draw system
		draw.SetWorldTransform(transform);

		// Cache last mouse position
		vLastMouseScreenPos = vMousePos;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_ImageTiling demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	// Experiment 1 - Replace with {1024,960}, { 1, 1 } to see
	// how it affects performance and tile sampling quality
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}