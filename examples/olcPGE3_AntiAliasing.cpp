/*
	olc::PixelGameEngine3 Example - Anti-Aliasing

	Demonstrates configuration of target images that use
	multi-sampled anti-aliasing (MSAA) to improve the
	visual quality of drawn shapes

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating anti-aliased drawing. 
// This class overrides the olc::PixelGameEngine base class by 
// implementing the OnUserCreate() and OnUserUpdate() functions
class Example_AntiAliasing : public olc::PixelGameEngine
{
public:
	Example_AntiAliasing()
	{
		sAppName = "Example - Anti-Aliasing";
	}

protected:

	olc::Image imgAntiAliased;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Create an image that uses anti-aliasing and is half
		// teh width of the screen
		CreateImage(imgAntiAliased, GetScreen().Size(),
			olc::ImageConfig{ .MSAA=true, .MSAASamples=16 });

		// There is a global config that sets the default number
		// of samples to 4, but im overriding it here to 16
		// to make it obvious

		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		float fTotalTime = TotalTimeElapsed() * 0.1f;

		// Lambda to draw a rotating fan of lines and circles
		auto DrawFan = [&]()
			{
				// Rotating gradient lines
				olc::vf2d p1 = GetScreen().Size() / 2.0f;
				olc::vf2d p2 = olc::vf2d{ std::cos(fTotalTime), std::sin(fTotalTime) } * 300.0f;
				draw.Line(p1, olc::Colour::CYAN, p1 + p2, olc::Colour::MAGENTA);
				draw.Line(p1, olc::Colour::CYAN, p1 - p2, olc::Colour::MAGENTA);
				draw.Line(p1, olc::Colour::CYAN, p1 + olc::vf2d{ -p2.y, p2.x }, olc::Colour::MAGENTA);
				draw.Line(p1, olc::Colour::CYAN, p1 + olc::vf2d{ p2.y, -p2.x }, olc::Colour::MAGENTA);
			
				// Draw some circles along the lines
				for (float t = -1.0f; t <= 1.0f; t += 0.2f)
				{
					olc::vf2d pos1 = p1 + p2 * t;
					olc::vf2d pos2 = p1 + olc::vf2d{ -p2.y, p2.x } * t;
					draw.Circle(pos1.round(), 10.0f, olc::Colour::YELLOW);
					draw.Circle(pos2.round(), 10.0f, olc::Colour::YELLOW);
				}

				// Link circles with lines
				for (float t = -1.0f; t <= 0.8f; t += 0.2f)
				{
					olc::vf2d pos1a = p1 + p2 * t;
					olc::vf2d pos1b = p1 + p2 * (t + 0.2f);
					olc::vf2d pos2a = p1 + olc::vf2d{ -p2.y, p2.x } * t;
					olc::vf2d pos2b = p1 + olc::vf2d{ -p2.y, p2.x } * (t + 0.2f);
					draw.Line(pos1a.round(), olc::Colour::GREEN, pos2a.round(), olc::Colour::GREEN);
					draw.Line(pos1b.round(), olc::Colour::GREEN, pos2b.round(), olc::Colour::GREEN);
				}


				draw.Line({ 320, 240 }, mouse.GetPosition(), olc::Colour::RED);

				// Rotating rectangle in middle				
				draw.WorldRotate(-fTotalTime, { 320, 240 });
				draw.FilledRect({ 320-100, 240-75 }, { 200, 150 }, olc::PixelF(1.0f, 1.0f, 1.0f, 0.5f))	;
			
			};


		// We will draw the same thing to two different buffers
		// one with anti-aliasing enabled and one without

		// Draw to anti-aliased buffer
		draw.SetTarget(imgAntiAliased);		
		draw.Clear(olc::Colour::BLACK);
		DrawFan();
		draw.WorldReset();

		// Draw to normal buffer
		draw.SetTarget(GetScreen());
		draw.Clear(olc::Colour::BLACK);
		DrawFan();
		draw.WorldReset();

		// Draw right half of anti-aliased image to right half of screen
		draw.Image(imgAntiAliased.region({ 320, 0 }, { 320, 480 }), { 320, 0 });

		draw.StringProp({ 10, 10 }, "No Anti-Aliasing", olc::Colour::WHITE);
		draw.StringProp({ 330, 10 }, "With Anti-Aliasing", olc::Colour::WHITE);

		return true;
	}
};

int main()
{
	Example_AntiAliasing demo;
	if (demo.Construct({ 640, 480 }, { 2, 2 }))
		demo.Start();
	return 0;
}