#define OLC_PGE3_APPLICATION
#include "olcPixelGameEngine3.h"

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{

	}

	olc::Image imTest;

	olc::Image sprite1;

	float fAngle = 0.0f;

public:
	bool OnUserCreate() override
	{
		CreateImage(imTest, { 64,64 });
		CreateImageFromFile(sprite1, "E:/assets/Graphics/Tiles_SideOn/HiQ/FactoryPack/png/256/objects/static/Computer (1).png");

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		fAngle += fElapsedTime;




		draw.SetTarget(imTest);
		draw.FilledRect({ 0,0 }, imTest.Size(), olc::Colour::BLUE);

		draw.WorldRotate(fAngle, { 32.0f, 32.0f });

		draw.Rect({ 5,5 }, { 10,10 }, olc::Colour::YELLOW);
		draw.Line({ 0,0 }, { 20, 20 });


		draw.SetTarget(GetDefaultImage());
		draw.WorldReset();

		draw.FilledRect({ 0,0 }, GetDefaultImage().Size(), olc::Colour::VERY_DARK_MAGENTA);

		draw.Line({ 0,0 }, GetDefaultImage().Size() - 1, olc::Colour::RED);
		draw.Line(olc::vf2d(GetDefaultImage().Size().x - 1, 0), olc::vf2d(0, GetDefaultImage().Size().y - 1), olc::Colour::GREEN);

		if (mouse.GetButton(0).bHeld)
			draw.Line({ 0,0 }, mouse.GetPosition(), olc::Colour::TANGERINE);

		draw.WorldRotate(fAngle * 0.2f, GetDefaultImage().Size() * 0.5f);
		draw.Rect({ 0,0 }, GetDefaultImage().Size());
		draw.Image(imTest, { 10, 10 });				   
		draw.Image(imTest, { 100, 10 });
		draw.Image(imTest, { 10, 100 });
		draw.Image(sprite1, { 100, 100 }, {0.1f, 0.1f});

		olc::Pixel p = draw.GetPixel(imTest, { 8,5 });

		draw.FilledRect({ 200,200 }, { 10,10 }, p);
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
		demo.Start();

	return 0;
}