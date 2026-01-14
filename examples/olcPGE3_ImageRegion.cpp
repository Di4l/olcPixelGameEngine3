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

	}

protected:

	// An Image object
	olc::Image imgTest;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Create a blank image
		CreateImage(imgTest, { 256, 240 });

        const olc::vi2d halfSize = imgTest.Size() / 2;

		// Fill the image portions
		for (int y = 0; y < halfSize.y; y++)
			for (int x = 0; x < halfSize.x; x++)
            {
                float tX = ::sinf((float)x / halfSize.x * M_PI * 5.0f) * 0.5f + 0.5f;
                float tY = ::sinf((float)y / halfSize.y * M_PI * 5.0f) * 0.5f + 0.5f;
				imgTest.region({ 0, 0 }, halfSize).Pixel({ x, y }) =
					olc::PixelLerp(olc::Colour::BLACK, olc::Colour::TANGERINE, tX);
                imgTest.region({ halfSize.x, 0 }, halfSize).Pixel({ x, y }) =
					olc::PixelLerp(olc::Colour::BLACK, olc::Colour::RED, tY);
                imgTest.region({ 0, halfSize.y }, halfSize).Pixel({ x, y }) =
					olc::PixelLerp(olc::Colour::BLACK, olc::Colour::BLUE, tY);
                imgTest.region({ halfSize.x, halfSize.y }, halfSize).Pixel({ x, y }) =
					olc::PixelLerp(olc::Colour::BLACK, olc::Colour::GREEN, tX);
            }

		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		draw.Image(imgTest, { 0.0f, 0.0f });

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