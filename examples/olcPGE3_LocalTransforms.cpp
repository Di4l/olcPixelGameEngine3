/*
	olc::PixelGameEngine3 Example - Local Transforms

	Draws a tree with local transforms, demonstrating how the 
	transform can be used to create hierarchical relationships 
	between objects

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating transformed drawing. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_LocalTransforms : public olc::PixelGameEngine
{
public:
	Example_LocalTransforms()
	{
	}

protected:
	bool bFirstFrame = true;

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
		if (!bFirstFrame && !keyboard.GetKey(olc::Key::SPACE).bPressed)
			return true;

		// Only generate the tree on the first frame, or when space is pressed
		bFirstFrame = false;

		// Clear screen to sky
		draw.FilledRect({ 0,0 }, ScreenSize(), 
			olc::Colour::CYAN, olc::Colour::CYAN, 
			olc::Colour::DARK_CYAN, olc::Colour::DARK_CYAN);

		auto RandomFloat = []() -> float
			{
				return float(rand()) / float(RAND_MAX);
			};

		auto RandomFloatRange = [&](float fMin, float fMax) -> float
			{
				return fMin + RandomFloat() * (fMax - fMin);
			};

		auto RandomIntRange = [&](int iMin, int iMax) -> int
			{
				return iMin + rand() % (iMax - iMin + 1);
			};

		auto RandomColour = [&]() ->olc::Pixel
			{
				return olc::PixelF(RandomFloat(), RandomFloat(), RandomFloat());
			};

		// Root Transform
		olc::tf2d transform;		
		transform.translate(ScreenSize() / 2.0f + olc::vf2d(0.0f, 40.0f));

		std::function<void(int)> DrawBranch = [&](int depth) -> void
			{
				if (depth <= 0)
				{
					// Draw a leaf - note its always the same circle!
					draw.FilledCircle({ 0, -40 }, 40, olc::Pixel(0, 180, 0), olc::Pixel(0, 150, 0), olc::Colour::WHITE);
					return;
				}
					
				// Set the lopcal transform
				draw.SetWorldTransform(transform);

				// Draw the branch - note its always teh same rectangle!
				draw.FilledRect({ -2, -40 }, { 4, 40 }, olc::Pixel(150,75,0));

				// Randomly generate some child branches
				int numBranches = RandomIntRange(1, 5);
				for (int b = 0; b < numBranches; b++)
				{
					// Move the transform to the end of the branch, so that child branches will be positioned correctly
					transform.translate(olc::vf2d{ 0.0f, -40.0f });
					transform.scale(olc::vf2d{ 0.75, RandomFloatRange(0.5f, 0.75f)});
					transform.rotate(RandomFloatRange(-0.5f, 0.5f));
					
					// Recursively draw child branches
					DrawBranch(depth - 1);
					
					// Pop the transform to return to the previous state for the next branch
					transform.pop();
					transform.pop();
					transform.pop();
				}

			};

		// Go!
		DrawBranch(5);

		draw.WorldReset();
		draw.StringProp({ 10, 10 }, "Press SPACE to regenerate tree", olc::Colour::BLACK);

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_LocalTransforms demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}