/*
	olc::PixelGameEngine3 Example - Rectangles

	Demonstrates various rectangle drawing techniques

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating rectangle drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Rectangles : public olc::PixelGameEngine
{
public:
	Example_Rectangles()
	{
		sAppName = "Example - Rectangles";
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

		// Draw a simple rectangle outline
		draw.Rect({ 10.0f, 10.0f }, { 100.0f, 40.0f }, olc::Colour::WHITE);

		// Draw a rectangle outline with multi-colour vertices
		draw.Rect( { 120.0f, 10.0f }, { 100.0f, 40.0f }, 
			olc::Colour::RED, olc::Colour::GREEN, 
			olc::Colour::BLUE, olc::Colour::YELLOW);

		// Draw a filled rectangle
		draw.FilledRect({ 10.0f, 60.0f }, { 100.0f, 40.0f }, olc::Colour::CYAN);

		// Draw a filled rectangle with multi-colour vertices
		draw.FilledRect({ 120.0f, 60.0f }, { 100.0f, 40.0f },
			olc::Colour::RED, olc::Colour::GREEN,
			olc::Colour::BLUE, olc::Colour::YELLOW);

		// Draw a filled rectangle with a tint that pulses over time
		float fTotalTime = TotalTimeElapsed();
		olc::Pixel tint = olc::Pixel(
			(uint8_t)((std::sin(fTotalTime) + 1.0f) * 127.5f),
			(uint8_t)((std::sin(fTotalTime + 2.0f) + 1.0f) * 127.5f),
			(uint8_t)((std::sin(fTotalTime + 4.0f) + 1.0f) * 127.5f)
		);
		draw.FilledRect({ 10.0f, 110.0f }, { 100.0f, 40.0f }, olc::Colour::WHITE, tint);

		// Draw a rectangle outline with a tint that pulses over time
		draw.Rect({ 120.0f, 110.0f }, { 100.0f, 40.0f }, olc::Colour::WHITE, tint);

		// Draw a rectangle rainbow
		draw.FilledRoundedRect({ 10.0f, 160.0f }, { 210.0f, 70.0f }, 20, olc::Colour::RED);
		draw.FilledRoundedRect({ 11.0f, 161.0f }, { 208.0f, 68.0f }, 20, olc::Colour::TANGERINE);
		draw.FilledRoundedRect({ 12.0f, 162.0f }, { 206.0f, 66.0f }, 20, olc::Colour::YELLOW);
		draw.FilledRoundedRect({ 13.0f, 163.0f }, { 204.0f, 64.0f }, 20, olc::Colour::GREEN);
		draw.FilledRoundedRect({ 14.0f, 164.0f }, { 202.0f, 62.0f }, 20, olc::Colour::CYAN);
		draw.FilledRoundedRect({ 15.0f, 165.0f }, { 200.0f, 60.0f }, 20, olc::Colour::BLUE);
		draw.FilledRoundedRect({ 16.0f, 166.0f }, { 198.0f, 58.0f }, 20, olc::Colour::MAGENTA);
		draw.FilledRoundedRect({ 17.0f, 167.0f }, { 196.0f, 56.0f }, 20, olc::Colour::BLACK);
		draw.RoundedRect({ 20.0f, 170.0f }, { 189.0f, 49.0f }, 20, olc::Colour::WHITE);
		
		// Sneaky peak at world transforms
		draw.WorldOffset({ 110.0f, 195.0f });
		draw.WorldRotate(fTotalTime);

		// Draw rotated filled rectangle with multi-colour vertices
		draw.FilledRect({ -20.0f, -15.0f }, { 40.0f, 30.0f },
			olc::Colour::RED, olc::Colour::GREEN,
			olc::Colour::BLUE, olc::Colour::YELLOW);
		draw.Rect({ -20.0f, -15.0f }, { 40.0f, 30.0f });

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Rectangles demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}