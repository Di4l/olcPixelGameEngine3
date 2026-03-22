/*
	olc::PixelGameEngine3 Example - Extensions

	Creates a simple Window Level PGEX to illustrate an external object
	being hooked at various points in the rendering and update cycle. 
	
	Not shown is the accompanying System Level PGEX, which could be used 
	for things like sound engines, resource managers, etc.

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"


// This is a "Window Extension" class. These are used on a "per window" basis to allow you to modify
// the behaviour of the window and application at various stages of the cycle. Typically these would
// be used to implement things like UI layers, or to inject update functionality into objects
class ExamplePGEX : public olc::PGEWindowExtension
{
	// Called when extension is installed, usually in PGEWindow Constructor
	// Return true to continue application
	virtual bool OnInstall([[maybe_unused]] olc::PGEWindow* pge) override
	{ 
		return true; 
	}

	// Called after PGE is established, but before OnUserCreate()
	// Return true to continue application
	bool OnBeforeUserCreate([[maybe_unused]] olc::PGEWindow* pge) override
	{ 
		return true; 
	}

	// Called after OnUserCreate(), but before the first call to OnUserUpdate()
	// Return true to continue application
	bool OnAfterUserCreate([[maybe_unused]] olc::PGEWindow* pge) override
	{ 
		return true; 
	}

	// Called at the start of each frame, before OnUserUpdate()
	// Return true if you wish to block OnUserUpdate() from being called this frame
	// Note 'fElapsedTime' is passed by reference, so you could modify it to speed up or slow down the update cycle
	bool OnBeforeUserUpdate([[maybe_unused]] olc::PGEWindow* pge, float &fElapsedTime) override
	{ 		
		float fModfier = pge->GetMouse().GetPosition().x / float(pge->GetScreen().Size().x);

		// Warp Time depending on mouse position, just to illustrate the point that we can modify behaviour here. Note that this will affect all
		fElapsedTime *= fModfier;
		
		// If we returned true, we indicate that the update cycle should be blocked, so OnUserUpdate() will not be 
		// called this frame. This could be used to implement a pause menu for example. Note that the extension is 
		// still active, so OnAfterUserUpdate() will still be called, allowing you to draw a menu or something
		return false; 
	}

	// Called at the end of each frame, after OnUserUpdate()
	// Return true to continue application
	bool OnAfterUserUpdate([[maybe_unused]] olc::PGEWindow* pge, float fElapsedTime) override
	{ 
		pge->GetDraw().String({ 10.0f, 200.0f }, "This text is drawn from a\nWindow extension!", olc::Colour::YELLOW);
		return true; 
	}

};


// We also have olc::PGESystemExtension for system level extensions, these are called
// at various stages of the context lifecycle. For example, a sound engine would
// likely be a system extension.



// Example application demonstrating "PGEX". This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Extensions : public olc::PixelGameEngine
{
public:
	Example_Extensions()
	{
		// Installing an extension allows it to receive callbacks at various stages 
		// of the application and window lifecycle, and to modify behaviour if necessary. 
		// You can install as many extensions as you like, and they will be called in 
		// the order they were installed
		InstallWindowExtension(&pgex);

		sAppName = "Example - Extensions";
	}

protected:

	// We need to create an instance of our extension class, and then install it in the PGE constructor
	ExamplePGEX pgex;

    float fTotalTime = 0.0f;

public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		draw.String({ 10.0f, 10.0f }, "This text is drawn from the\nmain application!\n\nMouse Move in X To Warp Time", olc::Colour::WHITE);

		// Draw a clocking line to illustrate the passage of time...
        fTotalTime += fElapsedTime;
		draw.Line(GetScreen().Size() / 2, olc::vf2d(cos(fTotalTime), sin(fTotalTime)) * 50.0f + GetScreen().Size() / 2, olc::Colour::GREEN);
		
		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Extensions demo;

	// Create "screen" of 256x240 "pixels"
	// with a pixel size of 4x4 actual screen pixels
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}
