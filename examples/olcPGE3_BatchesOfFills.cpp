/*
	olc::PixelGameEngine3 Example - Batches of Filled Primitives

	Demonstrates filled shapes drawn using batches for efficiency

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating batches of fills drawing. 
// This class overrides the olc::PixelGameEngine base class by 
// implementing the OnUserCreate() and OnUserUpdate() functions
class Example_BatchesOfFills : public olc::PixelGameEngine
{
public:
	Example_BatchesOfFills()
	{

	}

protected:
	std::vector<olc::vf2d> vecBubblePos;
	std::vector<olc::vf2d> vecBubbleVel;
	std::vector<olc::Pixel> vecBubbleCol;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Prepare bubble positions and velocities
		for (int i = 0; i < 20; i++)
		{
			vecBubblePos.push_back({
				float((rand() % (GetScreen().Size().x - 64)) + 32),
				float((rand() % (GetScreen().Size().y - 64)) + 32) });

			vecBubbleVel.push_back({
				(float(rand() % 2000) - 1000.0f) / 100.0f,
				(float(rand() % 2000) - 1000.0f) / 100.0f });

			vecBubbleCol.push_back(olc::Pixel(rand() % 256, rand() % 256, rand() % 256, 128));
		}


		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to dark blue
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Create the batch for filled shapes
		auto batch = draw.CreateFilledBatch();

		// For additiona ltest, spin the world around the center of the screen
		draw.WorldRotate(float(TotalTimeElapsed()) * 0.5f, ScreenSize() * 0.5f);

		// === Code mostly stolen form olcPGE3_CirclesEllipses.cpp ===

		// Draw a circle outline - CANT DO THIS WITH FILLED BATCHES!!!
		//draw.Circle({ 32.0f, 32.0f }, 20.0f, olc::Colour::WHITE);

		// Draw a filled circle
		draw.FilledCircle(batch, { 96.0f, 32.0f }, 20.0f, olc::Colour::GREEN);

		// Draw a filled circle with a white outline
		draw.FilledCircle(batch, { 160.0f, 32.0f }, 20.0f, olc::Colour::DARK_RED);
		//draw.Circle({ 160.0f, 32.0f }, 20.0f, olc::Colour::WHITE);

		// Draw a gradient circle
		draw.FilledCircle(batch, { 224.0f, 32.0f }, 20.0f,
			olc::Colour::RED, olc::Colour::YELLOW, olc::Colour::WHITE);



		// Draw an ellipse outline
		//draw.Ellipse({ 32.0f, 96.0f }, 20, 10, olc::Colour::WHITE);

		// Draw a filled ellipse
		draw.FilledEllipse(batch, { 96.0f, 96.0f }, 10, 20, olc::Colour::CYAN);

		// Draw a filled ellipse with a white outline
		draw.FilledEllipse(batch, { 160.0f, 96.0f }, 20, 10, olc::Colour::DARK_GREEN);
		//draw.Ellipse({ 160.0f, 96.0f }, 20, 10, olc::Colour::WHITE);

		// Draw a gradient ellipse
		draw.FilledEllipse(batch, { 224.0f, 96.0f }, 10, 20,
			olc::Colour::BLUE, olc::Colour::CYAN, olc::Colour::WHITE);


		// Draw a circle outline with fewer facets
		//draw.Circle({ 32.0f, 160.0f }, 20.0f, olc::Colour::WHITE, olc::Colour::WHITE, 5);

		// Draw a filled circle with fewer facets
		draw.FilledCircle(batch, { 96.0f, 160.0f }, 20.0f, olc::Colour::MAGENTA, olc::Colour::WHITE, 6);

		// Draw an ellipse outline with fewer facets
		//draw.Ellipse({ 160.0f, 160.0f }, 20, 10, olc::Colour::WHITE, olc::Colour::WHITE, 5);

		// Draw a filled ellipse with fewer facets
		draw.FilledEllipse(batch, { 224.0f, 160.0f }, 10, 20, olc::Colour::YELLOW, olc::Colour::MAGENTA, 6);


		// Bubbles!
		for (int i = 0; i < vecBubblePos.size(); i++)
		{
			// Update position
			vecBubblePos[i] += vecBubbleVel[i] * fElapsedTime * 10.0f;

			// Bounce off walls
			if (vecBubblePos[i].x < 32.0f || vecBubblePos[i].x > float(GetScreen().Size().x - 32))
			{
				if (vecBubbleVel[i].x < 0)
					vecBubblePos[i].x = 32.0f;
				else
					vecBubblePos[i].x = float(GetScreen().Size().x - 32);

				vecBubbleVel[i].x = -vecBubbleVel[i].x;
			}

			if (vecBubblePos[i].y < 32.0f || vecBubblePos[i].y > float(GetScreen().Size().y - 32))
			{
				if (vecBubbleVel[i].y < 0)
					vecBubblePos[i].y = 32.0f;
				else
					vecBubblePos[i].y = float(GetScreen().Size().y - 32);

				vecBubbleVel[i].y = -vecBubbleVel[i].y;
			}

			// Draw bubble (filled circles with outline)

			// Note the round() - This is to demonstrate that sub-pixel positions
			// can affect the appearance of the shapes due to the way they are rasterised
			draw.FilledCircle(batch, vecBubblePos[i].round(), 16, vecBubbleCol[i] * 0.5f);
			//draw.Circle(vecBubblePos[i].round(), 16, vecBubbleCol[i]);
		}


		// Test a filled rectangle
		draw.FilledRect(batch, { 10.0f, 200.0f }, { 236.0f, 20.0f }, olc::Colour::DARK_GREY);

		// Test a filled triangle that follows the mouse. Notice use of ScreenToWorld to
		// convert from screen space to world space coordinates
		olc::vf2d vMouse = draw.ScreenToWorld(mouse.GetPosition());
		draw.FilledTriangle(batch, 
			{ vMouse.x - 0.0f, vMouse.y - 50.0f },
			{ vMouse.x - 30.0f, vMouse.y + 30.0f },
			{ vMouse.x + 30.0f, vMouse.y + 30.0f },
			olc::Colour::WHITE);
		
		// If mouse button held, change batch colour to green
		if(mouse.GetButton(0).bHeld)
			draw.Batch(batch, olc::Colour::GREEN);
		else
			draw.Batch(batch);


		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_BatchesOfFills demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}