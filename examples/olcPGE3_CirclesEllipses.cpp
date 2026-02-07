/*
	olc::PixelGameEngine3 Example - Circles & Ellipses

	Demonstrates circle and ellipse drawing techniques

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating circle & ellipse drawing. 
// This class overrides the olc::PixelGameEngine base class by 
// implementing the OnUserCreate() and OnUserUpdate() functions
class Example_RoundThings : public olc::PixelGameEngine
{
public:
	Example_RoundThings()
	{

	}

protected:
	// We accumulate total time for some animation
	float fTotalTime = 0.0f;

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
				float((rand() % (GetScreen().Size().y - 64)) + 32)});

			vecBubbleVel.push_back({ 
				(float(rand() % 2000) - 1000.0f) / 100.0f, 
				(float(rand() % 2000) - 1000.0f) / 100.0f});

			vecBubbleCol.push_back(olc::Pixel(rand() % 256, rand() % 256, rand() % 256, 128	));
		}


		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to dark blue
		draw.Clear(olc::Colour::VERY_DARK_BLUE);



		// Draw a circle outline
		draw.Circle({ 32.0f, 32.0f }, 20.0f, olc::Colour::WHITE);

		// Draw a filled circle
		draw.FilledCircle({ 96.0f, 32.0f }, 20.0f, olc::Colour::GREEN);

		// Draw a filled circle with a white outline
		draw.FilledCircle({ 160.0f, 32.0f }, 20.0f, olc::Colour::DARK_RED);
		draw.Circle({ 160.0f, 32.0f }, 20.0f, olc::Colour::WHITE);

		// Draw a gradient circle
		draw.FilledCircle({ 224.0f, 32.0f }, 20.0f, 
			olc::Colour::RED, olc::Colour::YELLOW, olc::Colour::WHITE);



		// Draw an ellipse outline
		draw.Ellipse({ 32.0f, 96.0f }, 20, 10, olc::Colour::WHITE);

		// Draw a filled ellipse
		draw.FilledEllipse({ 96.0f, 96.0f }, 10, 20, olc::Colour::CYAN);

		// Draw a filled ellipse with a white outline
		draw.FilledEllipse({ 160.0f, 96.0f }, 20, 10, olc::Colour::DARK_GREEN);
		draw.Ellipse({ 160.0f, 96.0f }, 20, 10, olc::Colour::WHITE);

		// Draw a gradient ellipse
		draw.FilledEllipse({ 224.0f, 96.0f }, 10, 20,
			olc::Colour::BLUE, olc::Colour::CYAN, olc::Colour::WHITE);


		// Draw a circle outline with fewer facets
		draw.Circle({ 32.0f, 160.0f }, 20.0f, olc::Colour::WHITE, olc::Colour::WHITE, 5);

		// Draw a filled circle with fewer facets
		draw.FilledCircle({ 96.0f, 160.0f }, 20.0f, olc::Colour::MAGENTA, olc::Colour::WHITE, 6);

		// Draw an ellipse outline with fewer facets
		draw.Ellipse({ 160.0f, 160.0f }, 20, 10, olc::Colour::WHITE, olc::Colour::WHITE, 5);

		// Draw a filled ellipse with fewer facets
		draw.FilledEllipse({ 224.0f, 160.0f }, 10, 20, olc::Colour::YELLOW, olc::Colour::MAGENTA, olc::Colour::WHITE, 6);


		// Bubbles!
		for (int i = 0; i < vecBubblePos.size(); i++)
		{
			// Update position
			vecBubblePos[i] += vecBubbleVel[i] * fElapsedTime * 10.0f;

			// Bounce off walls
			if (vecBubblePos[i].x < 32.0f || vecBubblePos[i].x > float(GetScreen().Size().x - 32))
			{
				if(vecBubbleVel[i].x < 0)
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
			draw.FilledCircle(vecBubblePos[i].round(), 16, vecBubbleCol[i] * 0.5f);
			draw.Circle(vecBubblePos[i].round(), 16, vecBubbleCol[i]);
		}


		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_RoundThings demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}