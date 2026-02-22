/*
	olc::PixelGameEngine3 Example - ImageRect

	Example of using ImageRect to draw images with various effects

	Licenced under the OLC-3 License
*/

// ImageRect() draws an image (or region) to the screen, but it 
// is scaled to a specific rectangular size.


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating image drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Image : public olc::PixelGameEngine
{
public:
	Example_Image()
	{
		sAppName = "Example - ImageRect";
	}

protected:

	// An Image object
	olc::Image imgTest;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Load asset
		CreateImageFromFile(imgTest, "./assets/minsanity_texture.png");

		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Image is 64x64 pixels

		// Draw normal image at x2 size
		draw.ImageRect(imgTest, { 4, 4 }, imgTest.Size() * 2);

		// Flips are useful for sprite mirroring

		// Draw flipped in x
		draw.ImageRect(imgTest.flipH(), { 136, 4}, imgTest.Size());

		// Draw flipped in x and y
		draw.ImageRect(imgTest.flipH().flipV(), { 136, 136 }, imgTest.Size());

		// Draw an extracted region of the image (and flip that too :D)
		draw.ImageRect(imgTest.region({ 20,20 }, { 16,16 }).flipV(), { 4, 136 }, { 64, 64 });

		// Draw a half size, but tinted red version
		draw.ImageRect(imgTest, { 72, 136 }, imgTest.Size() / 2, olc::Colour::RED);


		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Image demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
		//if (demo.Construct({1024, 960 }, { 1, 1 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}