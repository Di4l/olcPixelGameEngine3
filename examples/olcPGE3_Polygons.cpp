/*
	olc::PixelGameEngine3 Example - Polygons

	Demonstrates various polygon drawing techniques

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating polygon drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Pixels : public olc::PixelGameEngine
{
public:
	Example_Pixels()
	{

	}

protected:
	std::vector<olc::vf2d> vecPolygonPoints;
	int nSelectedPoint = -1;

	std::vector<olc::Pixel> vecPolygonColours;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Polygon 1 - A simple octagon loop, centered around 64, 64
		for (int i = 0; i < 8; i++)
		{
			float angle = i * (3.14159f * 2.0f / 8.0f);
			float x = 64.0f + std::cos(angle) * 48.0f;
			float y = 64.0f + std::sin(angle) * 48.0f;
			vecPolygonPoints.push_back({ x, y });
		}


		// Polygon 2 - An octagon triangle fan, centered around 192, 64
		for (int i = 0; i < 8; i++)
		{
			float angle = i * (3.14159f * 2.0f / 8.0f);
			float x = 192.0f + std::cos(angle) * 48.0f;
			float y = 64.0f + std::sin(angle) * 48.0f;
			vecPolygonPoints.push_back({ x, y });
		}

		// Polygon 3 - A curved triangle strip, centered around 64, 186
		for (int i = 0; i <= 8; i++)
		{
			float angle = i * (3.14159f * 2.0f / 8.0f);

			float x1 = 64.0f + std::cos(angle) * 48.0f;
			float y1 = 186.0f + std::sin(angle) * 48.0f;
			vecPolygonPoints.push_back({ x1, y1 });

			float x2 = 64.0f + std::cos(angle) * 24.0f;
			float y2 = 186.0f + std::sin(angle) * 24.0f;
			vecPolygonPoints.push_back({ x2, y2 });
		}


		
		// A vector of assorted colours for the polygon vertices
		vecPolygonColours = {
			olc::Colour::RED, olc::Colour::TANGERINE, olc::Colour::YELLOW, olc::Colour::GREEN,
			olc::Colour::CYAN, olc::Colour::BLUE, olc::Colour::MAGENTA,	olc::Colour::WHITE,
			olc::Colour::RED, olc::Colour::TANGERINE, olc::Colour::YELLOW, olc::Colour::GREEN,
			olc::Colour::CYAN, olc::Colour::BLUE, olc::Colour::MAGENTA,	olc::Colour::WHITE,
			olc::Colour::RED, olc::Colour::TANGERINE, olc::Colour::YELLOW, olc::Colour::GREEN,
			olc::Colour::CYAN, olc::Colour::BLUE, olc::Colour::MAGENTA,	olc::Colour::WHITE,
		};



		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear screen to dark blue
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Handle Mouse Input

		// When the left button is pressed, check if we are near
		// a control point. If so, select it.
		if (mouse.GetButton(0).bPressed)
		{
			nSelectedPoint = -1;
			for (int i = 0; i < vecPolygonPoints.size(); i++)
			{
				if ((vecPolygonPoints[i] - mouse.GetPosition()).mag() < 4.0f)
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
				vecPolygonPoints[nSelectedPoint] = mouse.GetPosition();
			}
		}

		// When the left button is released, clear the selected point
		if (mouse.GetButton(0).bReleased)
		{
			nSelectedPoint = -1;
		}

		// Draw polygon 1, a solid outline
		std::vector<olc::vf2d> polygon1_points(vecPolygonPoints.begin() + 0, vecPolygonPoints.begin() + 8);
		draw.Polygon(polygon1_points, olc::Colour::WHITE);

		// Draw polygon 2, a fan, filled, single colour
		std::vector<olc::vf2d> polygon2_points(vecPolygonPoints.begin() + 8, vecPolygonPoints.begin() + 16);
		draw.FilledPolygon(olc::Structure::Fan, polygon2_points, olc::Colour::DARK_CYAN);

		// Draw polygon 2 with a gradient outline
		draw.Polygon(polygon2_points, vecPolygonColours);

		// Draw polygon 3, a strip, filled, gradient
		std::vector<olc::vf2d> polygon3_points(vecPolygonPoints.begin() + 16, vecPolygonPoints.end());
		draw.FilledPolygon(olc::Structure::Strip, polygon3_points, vecPolygonColours);
			

		// Draw Control Points
		for (int i = 0; i < vecPolygonPoints.size(); i++)
		{
			draw.Circle(vecPolygonPoints[i], 4, olc::Colour::RED);
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