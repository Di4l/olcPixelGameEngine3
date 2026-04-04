/*
	olc::PixelGameEngine3 Example - Blending Modes

	Draws a few lines using different styles and techniques

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating various blend modes. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_BlendingModes : public olc::PixelGameEngine
{
public:
	Example_BlendingModes()
	{
		sAppName = "Example - Blending Modes";
	}

	struct sGel
	{
		olc::vf2d vPos;
		olc::Pixel col;
	};

	std::vector<sGel> vecGels;
	olc::vf2d vGelSize = { 32.0f, 32.0f };
	sGel* pSelectedGel = nullptr;
	olc::BlendMode currentBlendMode = olc::BlendMode::Alpha;
	olc::Image imgTest;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		// Create "gels" - these are just coloured rectangles that we 
		// will draw with different blend modes
		vecGels.push_back({ { 98.0f, 74.0f }, olc::Colour::RED });
		vecGels.push_back({ { 114.0f, 57.0f }, olc::Colour::GREEN });
		vecGels.push_back({ { 118.0f, 80.0f }, olc::Colour::BLUE });
		vecGels.push_back({ { 145.0f, 178.0f }, olc::Colour::WHITE });
		vecGels.push_back({ { 113.0f, 135.0f }, olc::Colour::BLACK });
		vecGels.push_back({ { 77.0f, 176.0f }, olc::Pixel(128, 128, 128) });
		vecGels.push_back({ { 47.0f, 128.0f }, olc::Pixel(255, 255, 255, 64) });

		// Last gel uses an image for complexity
		vecGels.push_back({ { 18.0f, 169.0f }, olc::Pixel(255,255,255,64)});
		CreateImageFromFile(imgTest, "./assets/minsanity_texture.png");
		
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Draw Colour Bars
		draw.FilledRect({ 0.0f, 0.0f }, { 32.0f, 240.0f }, 
			olc::Colour::VERY_DARK_RED, olc::Colour::VERY_DARK_RED, 
			olc::Colour::RED, olc::Colour::RED);
		draw.FilledRect({ 32.0f, 0.0f }, { 32.0f, 240.0f }, 
			olc::Colour::VERY_DARK_GREEN, olc::Colour::VERY_DARK_GREEN,
			olc::Colour::GREEN, olc::Colour::GREEN);
		draw.FilledRect({ 64.0f, 0.0f }, { 32.0f, 240.0f }, 
			olc::Colour::VERY_DARK_BLUE, olc::Colour::VERY_DARK_BLUE,
			olc::Colour::BLUE, olc::Colour::BLUE);
		draw.FilledRect({ 96.0f, 0.0f }, { 32.0f, 240.0f }, 
			olc::Colour::BLACK);
		draw.FilledRect({ 128.0f, 0.0f }, { 128.0f, 240.0f }, 
			olc::Colour::WHITE, olc::Colour::WHITE, 
			olc::Colour::BLACK, olc::Colour::BLACK);

		// TADA!! Setting the Blend Mode changes how the GPU draws pixels to the screen. 
		// This is a global state, so all drawing operations will be affected by this 
		// setting until it is changed again
		draw.SetBlendMode(currentBlendMode);

		// Note drawing happens as normal, the GPU has been instructed to 
		// use the current blend mode when drawing pixels
					
		// Draw Gels with appropriate mode
		int gelCount = 0;
		for (auto& gel : vecGels)
		{
			if (gelCount == vecGels.size() - 1)
			{
				// Gel is a texture
				draw.ImageRect(imgTest, gel.vPos, vGelSize);
			}
			else // Gel is just a coloured rectangle
				draw.FilledRect(gel.vPos, vGelSize, gel.col);

			gelCount++;
		}



		// Switch back to alpha mode (aka Normal) for UI
		draw.SetBlendMode(olc::BlendMode::Alpha);

		// Hilioght frame of gel and detect if mouse is over it
		for (auto& gel : vecGels)
		{
			draw.Rect(gel.vPos, vGelSize, olc::Colour::WHITE);

			if (mouse.GetPosition().x >= gel.vPos.x && mouse.GetPosition().x < gel.vPos.x + vGelSize.x &&
				mouse.GetPosition().y >= gel.vPos.y && mouse.GetPosition().y < gel.vPos.y + vGelSize.y)
			{
				if (mouse.GetButton(0).bPressed)
				{
					pSelectedGel = &gel;
				}					
			}
		}

		// Handle mouse
		if(pSelectedGel && mouse.GetButton(0).bHeld)
		{
			pSelectedGel->vPos = mouse.GetPosition() - vGelSize / 2.0f;			
		}

		if (mouse.GetButton(0).bReleased)
		{
			pSelectedGel = nullptr;
		}


		// Change blend mode with number keys
		if (keyboard.GetKey(olc::Key::K1).bPressed)
			currentBlendMode = olc::BlendMode::Alpha;
		if (keyboard.GetKey(olc::Key::K2).bPressed)
			currentBlendMode = olc::BlendMode::Multiplicative;
		if (keyboard.GetKey(olc::Key::K3).bPressed)
			currentBlendMode = olc::BlendMode::Additive;


		// Draw UI
		draw.String({ 10.0f, 2.0f }, "1: Alpha Blend", 
			currentBlendMode == olc::BlendMode::Alpha ? olc::Colour::YELLOW : olc::Colour::WHITE);
		draw.String({ 10.0f, 12.0f }, "2: Multiplicative Blend",
			currentBlendMode == olc::BlendMode::Multiplicative ? olc::Colour::YELLOW : olc::Colour::WHITE);
		draw.String({ 10.0f, 22.0f }, "3: Additive Blend", 
			currentBlendMode == olc::BlendMode::Additive ? olc::Colour::YELLOW : olc::Colour::WHITE);
		draw.String({ 10.0f, 32.0f }, "Mouse to drag gels", olc::Colour::WHITE);

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_BlendingModes demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}