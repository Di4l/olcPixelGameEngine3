/*
	olc::PixelGameEngine3 Example - Image Tiling

	Shows a pannable & zoomable tiled image background

	Licenced under the OLC-3 License
*/


// Define OLC_PGE3_APPLICATION to include the implementation of 
// the Pixel Game Engine as part of this translation unit
#define OLC_PGE3_APPLICATION
#include "../olcPixelGameEngine3.h"

// Example application demonstrating image tiling. This class
// overrides the olc::PixelGameEngine base class by implementing
// the OnUserCreate() and OnUserUpdate() functions
class Example_Pixels : public olc::PixelGameEngine
{
public:
	Example_Pixels()
	{

	}

protected:
	// We accumulate total time for some animation
	float fTotalTime = 0.0f;
	int x = 0;

	olc::Image imTiles;

	bool bPanning = false;
	olc::vf2d vLastMouseScreenPos = { 0.0f, 0.0f };
	olc::tf2d transform;

	void HandlePanAndZoom()
	{
		if (mouse.GetButton(0).bPressed)
		{
			transform = olc::tf2d(); // Reset transform
		}

		if (mouse.GetButton(2).bPressed)
		{
			bPanning = true;
			vLastMouseScreenPos = mouse.GetPosition();
		}

		if (mouse.GetButton(2).bReleased)
		{
			bPanning = false;
		}

		olc::vf2d vMousePos = mouse.GetPosition();

		if (bPanning)
		{
			transform.translate((transform.translate() + olc::vf2d(vMousePos.x - vLastMouseScreenPos.x, vMousePos.y - vLastMouseScreenPos.y)).round());
		}

		if (mouse.GetWheel() != 0)
		{
			if (mouse.GetButton(1).bHeld)
			{
				// Rotate around mouse pos
				auto posWorldBeforeRotate = transform.inverse(vLastMouseScreenPos);

				if (mouse.GetWheel() > 0)
					transform.rotate(transform.rotate() + 0.1f, posWorldBeforeRotate);
				else
					transform.rotate(transform.rotate() - 0.1f, posWorldBeforeRotate);

				// Adjust translation to keep mouse position stable
				auto posScreenAfterRotate = transform.forward(posWorldBeforeRotate);
				auto posScreenDisplacement = vLastMouseScreenPos - posScreenAfterRotate;
				transform.translate(transform.translate() + posScreenDisplacement);
			}
			else
			{
				// Zoom around mouse pos
				auto posBeforeScale = transform.inverse(vLastMouseScreenPos);

				if (mouse.GetWheel() > 0)
					transform.scale(transform.scale() * 1.1f);
				else
					transform.scale(transform.scale() * 0.9f);

				// Adjust translation to keep mouse position stable
				auto posAfterScale = transform.forward(posBeforeScale);
				auto posScreenDisplacement = vLastMouseScreenPos - posAfterScale;
				transform.translate((transform.translate() + posScreenDisplacement).round());
			}
		}

		draw.SetWorldTransform(transform);
		vLastMouseScreenPos = vMousePos;
	}


	std::vector<olc::Pixel> vecPolygonColours;
public:
	// Called once at the start, so create things here
	bool OnUserCreate() override
	{
		
		CreateImageFromFile(imTiles, "H:/HumbleBundle/smario.png");

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


		HandlePanAndZoom();


		// Get visible tile region
		olc::vf2d vViewPos = draw.ScreenToWorld({ 0.0f, 0.0f });
		olc::vf2d vViewSize = draw.ScreenToWorld(draw.GetTargetSize());

		olc::vi2d vTileStart = (vViewPos / 16.0f).floor();
		olc::vi2d vTileEnd = ((vViewSize) / 16.0f).ceil();

		// Get four corners of screen in world space
		olc::vf2d vTopLeft = draw.ScreenToWorld(olc::vf2d{ 0.0f, 0.0f });
		olc::vf2d vBottomRight = draw.ScreenToWorld(draw.GetTargetSize());

		// Work out radius of circle required to cover screen
		float fScreenRadius = (vBottomRight - vTopLeft).mag() * 0.5f;
		olc::vi2d vVisibleGridExtents = {
			static_cast<int>(std::ceil(fScreenRadius / 16.0f)),
			static_cast<int>(std::ceil(fScreenRadius / 16.0f))
		};

		olc::vf2d vScreenCenter = draw.ScreenToWorld(draw.GetTargetSize() * 0.5f);
		//vScreenCenter /= 16.0f;
		olc::vf2d vSnappedCenter = {
				std::round(vScreenCenter.x / 16.0f) * 16.0f,
				std::round(vScreenCenter.y / 16.0f) * 16.0f
		};


		size_t nTileCount = 0;

		auto task = draw.CreateImageBatch(imTiles);
		// Draw visible tiles
		for (int y = -vVisibleGridExtents.y; y < vVisibleGridExtents.y; y++)
		{
			for (int x = -vVisibleGridExtents.x; x < vVisibleGridExtents.x; x++)
			{
				olc::vi2d vTilePos = olc::vi2d{ x, y };
				olc::vf2d vWorldPos = vSnappedCenter + (vTilePos * 16.0f);

				// Only draw tiles that are actually visible, accounting for rotation
				if ((vWorldPos - vScreenCenter).mag2() > (fScreenRadius + 16.0f) * (fScreenRadius + 16.0f))
					continue;
			
				long a = (vTilePos.x + int(vSnappedCenter.x / 16.0f));
				long b = (vTilePos.y + int(vSnappedCenter.y / 16.0f));
				long idx = ((b + 16) % 16);
				long idy = ((a + 16) % 16);
				olc::vi2d vTileIndex = { idy, idx }; // wrap around negative coords

				//draw.FilledRect(vWorldPos, { 16.0f, 16.0f }, vecPolygonColours[id % vecPolygonColours.size()]);
				
				//draw.Image(imTiles.region(vTileIndex * 16.0f, { 16, 16 }), vWorldPos);

				draw.BatchImage(task, imTiles.region(vTileIndex * 16.0f, { 16, 16 }), vWorldPos);

				nTileCount++;
			}
		}

		draw.Batch(task);

		draw.WorldReset();
		draw.String({ 2,2 }, "Tiles: " + std::to_string(nTileCount), olc::Colour::WHITE);

		

		std::string lorem = R"(
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		Lorem ipsum dolor sit amet consectetur adipiscing elit.Quisque faucibus ex sapien vitae pellentesque sem placerat.
		In id cursus mi pretium tellus duis convallis.Tempus leo eu aenean sed diam urna tempor.Pulvinar vivamus fringilla 
		lacus nec metus bibendum egestas.Iaculis massa nisl malesuada lacinia integer nunc posuere.Ut hendrerit semper vel 
		class aptent taciti sociosqu.Ad litora torquent per conubia nostra inceptos himenaeos.
		)";

		draw.StringProp({ 2.0f, 16.0f }, lorem, olc::Colour::BLACK, { 1.0f, 1.0f });
		draw.StringProp({ 1.0f, 15.0f }, lorem, olc::Colour::WHITE, { 1.0f, 1.0f });

		
		/*if (mouse.GetButton(0).bHeld)
		{
			draw.Image(imTiles.region({ 0,176 }, { 16, 16 }), { 10,10 }, {10.0f, 10.0f});
		}
		else
		{
			draw.Image(imTiles.region({ 16,176 }, { 16, 16 }), { 10,10 }, { 10.0f, 10.0f });
		}*/

		//fTotalTime += fElapsedTime;
		//if (fTotalTime > 0.1f)
		//{
		//	fTotalTime -= 0.1f;
		//	x++;
		//	if (x > 3) x = 0;

		//}

		//draw.Image(imTiles.region(olc::vf2d(6 + x, 15) * 16.0f, { 16, 32 }), { 10,10 }, { 5.0f, 5.0f });

		//draw.Image(imTiles.region(olc::vf2d(6, 15) * 16.0f, { 48, 32 }), { 10,10 }, { 5.0f, 5.0f });


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
	//if (demo.Construct({ 256, 240 }, { 4, 4 }))
	if (demo.Construct({ 1024, 960 }, { 1, 1 }))
	{
		// Start the application
		demo.Start();
	}

	return 0;
}