/*
	olc::PixelGameEngine3 Example - ImageRotated (and scaled)

	Example of using ImageRotated to draw images with different
	rotation and scaling. Note that the image is rotated around 
	a pivot point, which is specified in the function call. 
	
	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating image rotation. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_ImageRotated : public olc::PixelGameEngine
{
public:
	Example_ImageRotated()
	{
		sAppName = "Example - ImageRotated (and scaled)";
	}

protected:

	// An Image object
	olc::Image imgTest;

	// Handy graph paper for pixel testing
	olc::Image imgGraph;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Load asset
		CreateImageFromFile(imgTest, "./assets/minsanity_texture.png");

		// Load Graph Paper
		CreateImageFromFile(imgGraph, "./assets/graph_paper.png");
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Draw graph paper background
		draw.Image(imgGraph, { -512,-512 });

		// Note! Our imgTest is 64x64 pixels

		// Draw Image
		draw.Image(imgTest, { 64, 64 });

		// Draw Scaled Image
		draw.Image(imgTest, { 192, 64 }, { 2.0f, 1.0f });

		// Draw Scaled Image (but also horizontally flipped)
		draw.Image(imgTest.flipH(), {64, 192}, {1.0f, 3.0f});

		// Draw Rotated Image, Scaled evenly
		draw.ImageRotated(imgTest, { 256, 256 }, TotalTimeElapsed() * 0.1f, { 32.0f, 32.0f }, {2.0f, 2.0f});

		// Draw Rotated Image, Scaled unevenly
		draw.ImageRotated(imgTest, { 448, 256 }, TotalTimeElapsed() * -0.1f, { 32.0f, 32.0f },  { 1.0f, 0.5f });

		// Draw Rotated Image not around center, Scaled unevenly and flipped vertically
		draw.ImageRotated(imgTest.flipV(), { 384, 384 }, TotalTimeElapsed() * 0.2f, { 16.0f, 32.0f }, { 2.0f, 1.0f });

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_ImageRotated demo;

	// Create "screen" of 512x480 "pixels"
	// with a pixel size of 2x2 actual screen pixels
	if (demo.Construct({ 512, 480 }, { 2, 2 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}