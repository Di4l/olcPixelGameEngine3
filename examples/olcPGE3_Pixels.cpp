/*
	olc::PixelGameEngine3 Example - Pixels

	Draws a frame filled with random pixels

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating pixel drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Pixels : public olc::PixelGameEngine
{
public:
	Example_Pixels()
	{
		sAppName = "Example - Pixels ...lots of pixels";
	}

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Nothing to do here, so return true
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to dark blue
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		if (keyboard.GetKey(olc::Key::SPACE).bHeld)
			draw.WorldRotate(TotalTimeElapsed() * 0.1f, { 128, 120 });

		// Draw random pixels in a box
		for (int y = 10; y < 108; y++)
		{
			for(int x = 10; x < 108; x++)
			{
				// Create a random colour that is opaque
				olc::Pixel p = olc::Pixel(rand() % 256, rand() % 256, rand() % 256);

				// Plot the pixel to the screen
				draw.Pixel(olc::vf2d(x, y), p);
			}
		}

		// Draw random pixels in a box, tinted over time
		float fTotalTime = TotalTimeElapsed();
		olc::Pixel tint = olc::Pixel(
			(uint8_t)((std::sin(fTotalTime) + 1.0f) * 127.5f),
			(uint8_t)((std::sin(fTotalTime + 2.0f) + 1.0f) * 127.5f),
			(uint8_t)((std::sin(fTotalTime + 4.0f) + 1.0f) * 127.5f));

		for (int y = 130; y < 228; y++)
		{
			for (int x = 10; x < 108; x++)
			{
				olc::Pixel p = olc::Pixel(rand() % 256, rand() % 256, rand() % 256);
				draw.Pixel(olc::vf2d(x, y), p, tint);
			}
		}

		// Same again using HSV (no alpha)
		for (int y = 10; y < 108; y++)
		{
			for (int x = 130; x < 228; x++)
			{
				float h = (float)(rand() % 360);
				float s = 1.0f;
				float v = 1.0f;
				olc::Pixel p = olc::PixelHSV(h, s, v);
				draw.Pixel(olc::vf2d(x, y), p);
			}
		}

		// Duplicate of top left box, by reading the pixels back
		for (int y = 10; y < 108; y++)
		{
			for (int x = 10; x < 108; x++)
			{
				olc::Pixel p = draw.GetPixel(olc::vf2d(x, y));
				draw.Pixel(olc::vf2d(x, y) + olc::vf2d{120, 120}, p);
			}
		}


		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Pixels demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}