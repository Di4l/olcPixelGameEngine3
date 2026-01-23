/*
	olc::PixelGameEngine3 Example - Batchs of Lines

	Draws all shapes that have an outline into a single line batch for efficiency

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating batched line drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_BatchesOfLines : public olc::PixelGameEngine
{
public:
	Example_BatchesOfLines()
	{

	}

protected:
	// We accumulate total time for some animation
	float fTotalTime = 0.0f;

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

		// Create a line batch
		auto batch = draw.CreateLineBatch();

		// === Stolen from olcPGE3_Lines.cpp ===
		// 
		// Simple white line
		draw.Line(batch, { 10.0f, 10.0f }, { 246.0f, 40.0f }, olc::Colour::WHITE);

		// Gradient line
		draw.Line(batch, { 10.0f, 50.0f }, olc::Colour::RED, { 246.0f, 80.0f }, olc::Colour::YELLOW);

		// Rotating gradient lines
		fTotalTime += fElapsedTime;
		olc::vf2d p1 = { 64.0f, 160.0f };
		olc::vf2d p2 = olc::vf2d{ std::cos(fTotalTime), std::sin(fTotalTime) } * 64.0f;

		draw.Line(batch, p1, olc::Colour::CYAN, p1 + p2, olc::Colour::MAGENTA);
		draw.Line(batch, p1, olc::Colour::CYAN, p1 - p2, olc::Colour::MAGENTA);
		draw.Line(batch, p1, olc::Colour::CYAN, p1 + olc::vf2d{ -p2.y, p2.x }, olc::Colour::MAGENTA);
		draw.Line(batch, p1, olc::Colour::CYAN, p1 + olc::vf2d{ p2.y, -p2.x }, olc::Colour::MAGENTA);

		// Line to mouse pointer with tint
		float alpha = (std::sin(fTotalTime * 4.0f) + 1.0f) / 2.0f;
		draw.Line(batch, p1, mouse.GetPosition(), olc::PixelF(1.0f, 0.0f, 0.0f, alpha)); // Note! Can't tint a single line in a batch

		// Boundary box (with fixed	coordinates)
		draw.Line(batch, { 0.0f, 0.0f }, { 255.0f, 0.0f }, olc::Colour::DARK_GREY);
		draw.Line(batch, { 255.0f, 0.0f }, { 255.0f, 239.0f }, olc::Colour::DARK_GREY);
		draw.Line(batch, { 255.0f, 239.0f }, { 0.0f, 239.0f }, olc::Colour::DARK_GREY);
		draw.Line(batch, { 0.0f, 239.0f }, { 0.0f, 0.0f }, olc::Colour::DARK_GREY);

		// ======================================

		
		// Add Rectangle with single colour
		draw.Rect(batch, { 10.0f, 200.0f }, { 100.0f, 30.0f }, olc::Colour::DARK_GREEN);
		
		// Add Rectangle with gradient colours
		draw.Rect(batch, { 14.0f, 204.0f }, { 92.0f, 22.0f }, olc::Colour::GREEN, olc::Colour::RED, olc::Colour::BLUE, olc::Colour::WHITE);
		
		// Add Circle with single colour
		draw.Circle(batch, ScreenSize() * 0.5f, 10.0f, olc::Colour::DARK_CYAN);
		
		// Add Ellipse with dynamic size
		draw.Ellipse(batch, ScreenSize() * 0.5f, 100.0f * ((cos(fTotalTime) + 1.0f) * 0.5f), 100.0f * ((sin(fTotalTime) + 1.0f) * 0.5f), olc::Colour::DARK_YELLOW);
		
		// Add Polygon with single colour
		draw.Polygon(batch, {
			{ 200.0f, 150.0f },
			{ 250.0f, 180.0f },
			{ 220.0f, 220.0f },
			{ 180.0f, 200.0f },
			{ 160.0f, 160.0f }
			}, olc::Colour::DARK_MAGENTA);
		
		// Add Polygon with gradient colours
		draw.Polygon(batch, {
			{ 200.0f, 50.0f },
			{ 250.0f, 80.0f },
			{ 220.0f, 120.0f },
			{ 180.0f, 100.0f },
			{ 160.0f, 60.0f }
			}, { 
			olc::Colour::MAGENTA, 
			olc::Colour::RED, 
			olc::Colour::YELLOW, 
			olc::Colour::GREEN, 
			olc::Colour::CYAN });

		// Add Rounded Rectangle
		draw.RoundedRect(batch, { 150.0f, 200.0f }, { 80.0f, 30.0f }, 5.0f, olc::Colour::TANGERINE);



		// Single batch draw with optional tinting
		if(mouse.GetButton(0).bHeld)
			draw.Batch(batch, olc::PixelF(1.0f, 0.0f, 0.0f, 0.5f));
		else
			draw.Batch(batch, olc::Colour::WHITE);

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_BatchesOfLines demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}