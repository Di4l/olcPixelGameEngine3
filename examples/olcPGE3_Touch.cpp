/*
olc::PixelGameEngine3 Example - Touch

Demonstrates touch input handling

Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating touch input handling. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Touch : public olc::PixelGameEngine
{
public:
	Example_Touch()
	{
		sAppName = "Example - Touch";
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
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		auto touches = touch.GetTouchIDs();


		draw.String({ 2.0f, 2.0f }, "Touch Points: " + std::to_string(touches.size()), olc::Colour::WHITE);

		

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Touch demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}