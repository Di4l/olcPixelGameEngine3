/*
	olc::PixelGameEngine3 Example - Image Quads

	Draws an image quads, demonstrating "warping"

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating quad drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_ImageQuads : public olc::PixelGameEngine
{
public:
	Example_ImageQuads()
	{

	}

protected:
	std::vector<olc::vf2d> vecControlPoints;
	int nSelectedPoint = -1;

	olc::Image imgTest;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Load asset
		CreateImageFromFile(imgTest, "./assets/sanity_texture.png");

		// Define control points for quad corners
		vecControlPoints.push_back({ 50.0f, 50.0f });   // Top-Left
		vecControlPoints.push_back({ 150.0f, 50.0f });  // Top-Right
		vecControlPoints.push_back({ 150.0f, 150.0f }); // Bottom-Right
		vecControlPoints.push_back({ 50.0f, 150.0f });  // Bottom-Left
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::BLACK);

		// Draw Background Gradient
		draw.FilledRect({ 0, 0 }, draw.GetTargetSize(), 
			olc::Colour::WHITE, olc::Colour::YELLOW,
			olc::Colour::CYAN, olc::Colour::MAGENTA);
		

		// Handle Mouse Input

		// When the left button is pressed, check if we are near
		// a control point. If so, select it.
		if (mouse.GetButton(0).bPressed)
		{
			nSelectedPoint = -1;
			for (int i = 0; i < vecControlPoints.size(); i++)
			{
				if ((vecControlPoints[i] - mouse.GetPosition()).mag() < 8.0f)
				{
					nSelectedPoint = i;
					break;
				}
			}
		}

		// If the left button is held, and we have a selected point,
		// move the point to the mouse position
		if (mouse.GetButton(0).bHeld)
		{
			if (nSelectedPoint != -1)
			{
				vecControlPoints[nSelectedPoint] = mouse.GetPosition().round();
			}
		}

		// When the left button is released, clear the selected point
		if (mouse.GetButton(0).bReleased)
		{
			nSelectedPoint = -1;
		}

		// Draw Quad
		draw.ImageQuad(imgTest, vecControlPoints);

		// Draw Control Points
		for (const auto& p : vecControlPoints)
		{
			draw.Circle(p, 8, olc::Colour::RED);
		}


		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_ImageQuads demo;

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