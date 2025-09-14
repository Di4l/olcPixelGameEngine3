#define OLC_PGE_APPLICATION
#include "olcpge3.h"

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

		draw.AffineRotate(fAngle, { 32.0f, 32.0f });
		
		draw.Rect({ 5,5 }, { 10,10 }, olc::Colour::YELLOW);
		draw.Line({ 0,0 }, { 20, 20 });


		draw.SetTarget(imgPrimary);
		draw.AffineReset();

		draw.FilledRect({ 0,0 }, imgPrimary.Size(), olc::Colour::VERY_DARK_MAGENTA);

		draw.Line({ 0,0 }, imgPrimary.Size() - 1, olc::Colour::RED);
		draw.Line(olc::vf2d( imgPrimary.Size().x - 1, 0 ), olc::vf2d( 0, imgPrimary.Size().y - 1 ), olc::Colour::GREEN);
		
		if(mouse.GetButton(0).bHeld)
			draw.Line({ 0,0 }, mouse.GetPosition(), olc::Colour::TANGERINE);

		draw.AffineRotate(fAngle * 0.2f, imgPrimary.Size() * 0.5f);
		draw.Rect({ 0,0 }, imgPrimary.Size());
		draw.Image(imTest, { 10, 10 }, { 64, 64 });
		draw.Image(imTest, { 100, 10 }, { 64, 64 });
		draw.Image(imTest, { 10, 100 }, { 64, 64 });

		draw.Image(sprite1, { 100, 100 }, { 64, 64 });
		draw.Image(sprite1.region({ 20.0f, 20.0f }, { 32.0f, 32.0f }), { 200, 100 }, { 32, 32 });		
		draw.Image(sprite1.region({ 20.0f, 20.0f }, { 32.0f, 32.0f }, { 100.0f, 200.0f }, { 150.0f, 180.0f }), { 100, 200 }, { 32, 32 });
		olc::Pixel p = draw.GetPixel(imTest, { 8,5 });

		draw.FilledRect({ 200,200 }, { 10,10 }, p);

		////draw.AffineRotate(fAngle, { 128,120 });
		////draw.AffineOffset({ 10,10 });
		//draw.AffineScale(olc::vf2d(1,1) * (std::sin(fAngle) + 1.1));

		//draw.Rect({ 0,0 }, GetSize() - olc::vf2d{1, 1}, olc::Colour::CYAN);
		//draw.FillRect({ 100.0f, 30.0f }, { 20.0f, 50.0f }, olc::Colour::TANGERINE);

		   

		//for (float i = 0; i < 20; i++)
		//{
		//	draw.AffineRotate(fAngle + (i/50.0f), {128,120});
		//	if (fmod(i,2) == 1)
		//	{
		//		draw.Line({ 0 + i * 4 ,0 + i * 4 }, { 255 - i * 4, 0 + i * 4 }, olc::Colour::RED, olc::Colour::GREEN);
		//		draw.Line({ 255 - i * 4,0 + i * 4 }, { 255 - i * 4, 239 - i * 4 }, olc::Colour::GREEN, olc::Colour::CYAN);
		//		draw.Line({ 255 - i * 4,239 - i * 4 }, { 0 + i * 4, 239 - i * 4 }, olc::Colour::CYAN, olc::Colour::WHITE);
		//		draw.Line({ 0 + i * 4,239 - i * 4 }, { 0 + i * 4, 0 + i * 4 }, olc::Colour::WHITE, olc::Colour::RED);
		//	}
		//	else
		//	{
		//		draw.Line({ 0 + i * 4 ,0 + i * 4 }, { 255 - i * 4, 0 + i * 4 }, olc::Colour::RED);
		//		draw.Line({ 255 - i * 4,0 + i * 4 }, { 255 - i * 4, 239 - i * 4 }, olc::Colour::GREEN);
		//		draw.Line({ 255 - i * 4,239 - i * 4 }, { 0 + i * 4, 239 - i * 4 }, olc::Colour::CYAN);
		//		draw.Line({ 0 + i * 4,239 - i * 4 }, { 0 + i * 4, 0 + i * 4 }, olc::Colour::WHITE);
		//	}
		//}

		///*for (int x = 0; x < 100; x++)
		//{
		//	for (int y = 0; y < 100; y++)
		//	{
		//		if (x % 2 == 0)
		//		{
		//			draw.Pixel(olc::vf2d(x, y) + olc::vf2d(100, 100), olc::Colour::YELLOW);
		//		}
		//	}
		//}*/

		////draw.Pixel({ -0.5f, -0.25f }, olc::Colour::YELLOW);

		//draw.FillRect({ 100.0f, 30.0f }, { 20.0f, 50.0f }, olc::Colour::TANGERINE);

		////draw.Pixel({ 0.3f, 0.3f }, olc::Colour::BLACK);
		////draw.Pixel({ 0.32f, 0.3f }, olc::Colour::BLACK);
		////draw.Pixel({ 0.34f, 0.3f }, olc::Colour::BLACK);




		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct({ 256, 240 }, { 4, 4 }))
	//if (demo.Construct({ 1280, 960 }, { 1, 1 }))
		demo.Start();

	return 0;
}