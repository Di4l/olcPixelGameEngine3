/*
	olc::PixelGameEngine3 Example - Text Basics

	Draws text using the default PGE font

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating various text drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Text : public olc::PixelGameEngine
{
public:
	Example_Text()
	{
		sAppName = "Example - Text Basics";
	}

protected:
	float fTickerTime = 0.0f;

	// We'll draw random text in the background with
	// a typing effect
	std::string sBackgroundRandomText;

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

		// Rainbow text in time
		float fTotalTime = TotalTimeElapsed();
		olc::Pixel colRainbow = olc::Pixel(
			(uint8_t)((std::sin(fTotalTime * 2.0f + 0.0f) + 1.0f) * 127.5f),
			(uint8_t)((std::sin(fTotalTime * 2.0f + 2.0f) + 1.0f) * 127.5f),
			(uint8_t)((std::sin(fTotalTime * 2.0f + 4.0f) + 1.0f) * 127.5f)
		);

		// Draw background random text

		// Determine how many characters fit on the screen
		olc::vf2d vSizeOfChar = draw.GetTextSize("A");
		olc::vf2d nVisibleChars = GetScreen().Size() / vSizeOfChar;

		// Update ticker
		fTickerTime += fElapsedTime;
		if (fTickerTime > 0.01f)
		{
			fTickerTime = 0.0f;

			// Append a new random character
			sBackgroundRandomText += (char)('A' + (rand() % 26));
			
			// New line if we reach end of screen horizontally
			if (sBackgroundRandomText.size() % (size_t)(nVisibleChars.x+1) == 0)
				sBackgroundRandomText += '\n';

			// Remove first line if we reach end of screen vertically
			if (sBackgroundRandomText.size() > (size_t)(nVisibleChars.x * nVisibleChars.y))
				sBackgroundRandomText.erase(0, nVisibleChars.x + 1);
		}

		// Draw the background random text in dark blue
		draw.String({ 0,0 }, sBackgroundRandomText, olc::Pixel(0, 0, 100));

		// Draw some example text

		// Using monospaced font with rainbow colours
		draw.String({ 10, 10 }, "\tWelcome To\nolc::PixelGameEngine3!\nThis is MONOSPACE font", colRainbow);
		// Get bounding box of monospaced text
		olc::vf2d vBoundingBox = draw.GetTextSize("\tWelcome To\nolc::PixelGameEngine3!\nThis is MONOSPACE font");
		// Draw bounding box with a 5 pixel padding
		draw.Rect({ 5, 5 }, vBoundingBox + 10, olc::Colour::WHITE);

		// Using proportional font with inverse rainbow colours
		draw.StringProp({ 10, 60 }, "\tWelcome To\nolc::PixelGameEngine3!\nThis is PROPORTIONAL font", colRainbow.inv());
		// Get bounding box of proportional text
		olc::vf2d vBoundingBoxProp = draw.GetTextSize("\tWelcome To\nolc::PixelGameEngine3!\nThis is PROPORTIONAL font", true);
		// Draw bounding box with a 5 pixel padding
		draw.Rect({ 5, 55 }, vBoundingBoxProp + 10, olc::Colour::WHITE);

		// Large scaled text
		std::string sLargeText = "Large Text!";
		// Get bounding box of large text (note the scaling parameter)
		olc::vf2d vLargeTextSize = draw.GetTextSize(sLargeText, false, { 2.0f, 4.0f });
		// Draw large text and bounding box
		draw.String({ 10, 150 }, sLargeText, olc::Colour::YELLOW, { 2.0f, 4.0f });
		draw.Rect({ 10, 150 }, vLargeTextSize, olc::Colour::WHITE);
		

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Text demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}