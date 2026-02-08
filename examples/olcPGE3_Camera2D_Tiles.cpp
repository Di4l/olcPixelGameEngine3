/*
	olc::PixelGameEngine3 Example - Camera2D - Tiles & Panning & Zooming

	Demonstration of teh Camera2D Utility class, which provides various 
	modes of camera movement and tracking. This example uses a simple tile 
	map world to show how the camera can be used to track a point in the 
	world, and how the different camera modes affect the way the camera 
	moves in response to the tracked point.

	The user can control the tracked point with WASD keys, and switch between
	"free roam" mode (where the camera does not track the point) and "play" mode
	(where the camera tracks the point) with the TAB key. The user can also switch
	between the different camera modes with the 1, 2, 3, 4, 5 keys.

	Licenced under the OLC-3 License
*/

// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Include the Camera2D Utility - AFTER including the PixelGameEngine
// as it relies on some of its types!
#include "../utilities/olcUTIL3_Camera2D.h"


// Example application demonstrating a Camera in 2D. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Camera2D_Tiles : public olc::PixelGameEngine
{
public:
	Example_Camera2D_Tiles()
	{

	}

protected:

	// Camera utility class
	olc::utils::Camera2D camera;
	
	// User can use mouse to pan & zoom around to show how
	// the camera works without needing to "look through it"
	bool bFreeRoam = false;

	// World parameters
	olc::vi2d vWorldSize = { 80, 75 };
	std::vector<uint8_t> vecWorldMap;
	
	// Size of each tile in the world, in pixels
	olc::vf2d vTileSize = { 32, 32 };
	// The point in the world we want the camera to track
	olc::vf2d vTrackedPoint;


public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		vTrackedPoint = { 20.0f, 20.0f };
		// Create the camera, providing it with the screen size, tile size and tracked point
		// Since our tile size is 32x32, the transform will be configured such that 1 unit in 
		// the world is 32 pixels on the screen. This is just useful for any tile based games, 
		// as it allows you to work in "tile space" rather than pixel space
		camera = olc::utils::Camera2D(ScreenSize(), vTileSize, vTrackedPoint);

		// Configure Camera

		// Set the point in the world we want the camera to track. It will follow this point
		// and adjust the world transform according to teh camera mode
		camera.SetTarget(vTrackedPoint);
		camera.SetMode(olc::utils::Camera2D::Mode::Simple);

		// Set the world boundary, which is used in some camera modes to prevent the camera from
		// showing areas outside of the world. In this example, the world boundary is set to
		// the size of our tile map, so that we dont see outside of it. Note that the world boundary
		// is in world units, so we dont need to multiply by tile size, the camera will take care
		// of that in the world transform
		camera.SetWorldBoundary({ 0.0f, 0.0f }, vWorldSize);
		camera.EnableWorldBoundary(true);

		// Create "tile map" world with just two tile types
		vecWorldMap.resize(vWorldSize.area());
		for (int i = 0; i < vecWorldMap.size(); i++)
			vecWorldMap[i] = ((rand() % 20) == 1) ? 1 : 0;
		
		return true;
	}

	// Called every frame, so update things here
	bool OnUserUpdate(float fElapsedTime) override
	{
		// Clear whole screen
		draw.Clear(olc::Colour::VERY_DARK_BLUE);

		// Handle player "physics" in response to key presses
		olc::vf2d vVel = { 0.0f, 0.0f };
		if (keyboard.GetKey(olc::Key::W).bHeld) vVel = vVel + olc::vf2d{0, -1};
		if (keyboard.GetKey(olc::Key::S).bHeld) vVel = vVel + olc::vf2d{0, +1};
		if (keyboard.GetKey(olc::Key::A).bHeld) vVel = vVel + olc::vf2d{-1, 0};
		if (keyboard.GetKey(olc::Key::D).bHeld) vVel = vVel + olc::vf2d{+1, 0};
		vTrackedPoint += vVel * 8.0f * fElapsedTime;

		// Switch between "free roam" and "play" mode with TAB key
		if (keyboard.GetKey(olc::Key::TAB).bPressed)
		{
			bFreeRoam = !bFreeRoam;
		}

		// Switch camera mode in operation
		if (keyboard.GetKey(olc::Key::K1).bPressed)
			camera.SetMode(olc::utils::Camera2D::Mode::Simple);
		if (keyboard.GetKey(olc::Key::K2).bPressed)
			camera.SetMode(olc::utils::Camera2D::Mode::EdgeMove);
		if (keyboard.GetKey(olc::Key::K3).bPressed)
			camera.SetMode(olc::utils::Camera2D::Mode::LazyFollow);
		if (keyboard.GetKey(olc::Key::K4).bPressed)
			camera.SetMode(olc::utils::Camera2D::Mode::FixedScreens);
		if (keyboard.GetKey(olc::Key::K5).bPressed)
			camera.SetMode(olc::utils::Camera2D::Mode::SlideScreens);

		// Update the camera, if teh tracked object remains visible, 
		// true is returned
		bool bOnScreen = false;

		if (bFreeRoam)
		{
			// In free roam mode, we ignore the tracked point and instead 
			// allow the user to pan and zoom the camera with the mouse
			camera.HandlePanAndZoom(mouse);
			// Update camera, but dont actually change the world transform
			bOnScreen = camera.Update(fElapsedTime, false);
		}
		else
			// In play mode, we update the camera as normal, which will cause it to
			// follow the tracked point according to the camera mode
			bOnScreen = camera.Update(fElapsedTime);

		// Set the world transform for the camera, so that all drawing operations
		draw.SetWorldTransform(camera.GetWorldTransform());

		// Render "tile map", by getting visible tiles
		 
		// If we never change scale we can just use the view parameters
		// directly...
		//olc::vi2d vTileCount = camera.GetViewSize().ceil() + 1;
		//olc::vf2d vTileOffset = camera.GetViewPosition().floor();

		// ... but if we allow free zooming, then we need to convert 
		// screen coordinates to world coordinates to get the correct 
		// tile offsets and counts
		olc::vi2d vTileOffset = draw.ScreenToWorld({ 0,0 }).floor();
		olc::vi2d vTileCount = draw.ScreenToWorld(ScreenSize()).ceil() - vTileOffset;

		// Clamp to ensure we stay in bounds of our world map
		olc::vi2d vTileTL = vTileOffset.max({ 0,0 });
		olc::vi2d vTileBR = (vTileOffset + vTileCount).min(vWorldSize);
		olc::vi2d vTile;

		// Then looping through them and drawing them
		auto batch = draw.CreateFilledBatch();

		for (vTile.y = vTileTL.y; vTile.y < vTileBR.y; vTile.y++)
			for (vTile.x = vTileTL.x; vTile.x < vTileBR.x; vTile.x++)
			{
				// 2D -> 1D index conversion for our world map
				int idx = vTile.y * vWorldSize.x + vTile.x;

				if (vecWorldMap[idx] == 0)
					draw.FilledRect(batch, vTile, { 1.0f, 1.0f }, olc::Colour::DARK_GREEN);

				if (vecWorldMap[idx] == 1)
					draw.FilledRect(batch, vTile, { 1.0f, 1.0f }, olc::Colour::TANGERINE);
			}

		// Draw the batch of tiles
		draw.Batch(batch);

		// Draw the "player" as a 1x1 cell
		draw.FilledRect(vTrackedPoint - olc::vf2d(0.5f, 0.5f), { 1.0f, 1.0f }, olc::Colour::BLUE);

		// Overlay with information
		if (bFreeRoam)
		{
			draw.FilledRect(camera.GetViewPosition(), camera.GetViewSize(), olc::PixelF(1.0f, 0.0f, 0.0f, 0.5f));			
		}

		
		// Reset world transform to draw info in screen space
		draw.WorldReset();

		if (bFreeRoam)
			draw.StringProp({ 2, 2 }, "TAB: Free Mode, M-Btn to Pan & Zoom", olc::Colour::YELLOW);
		else
			draw.StringProp({ 2,2 }, "TAB: Play Mode", olc::Colour::YELLOW);

		draw.StringProp({ 2,12 }, "WASD  : Move", olc::Colour::YELLOW);
		draw.StringProp({ 2,22 }, "CAMERA: 1) Simple  2) EdgeMove  3) LazyFollow  4) Screens 5) Slides", olc::Colour::YELLOW);
		draw.StringProp({ 2,42 }, vTileOffset.str(), olc::Colour::YELLOW);

		// Successful frame
		return true;
	}
};


// Main entry point for the application
int main()
{
	// Construct demo application
	Example_Camera2D_Tiles demo;

	// Create "screen" of 512x480 "pixels"
	// with a pixel size of 2x2 actual screen pixels
	if (demo.Construct({ 512, 480 }, { 2, 2 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}