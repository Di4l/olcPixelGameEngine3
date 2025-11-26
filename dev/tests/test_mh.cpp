#define OLC_PGE3_APPLICATION
#include "olcpge3.h"


class SecondWindow : public olc::PGEWindow
{
public:

	// Return true if window is to continue
	bool OnUserCreate() override
	{

		return false;
	}

	// Return true if window is to continue
	bool OnUserUpdate(float fElapsedTime) override
	{
		fTotalTime += fElapsedTime;

		draw.Clear(olc::Colour::RED);
		draw.WorldRotate(fTotalTime, GetDefaultImage().Size() / 2);
		draw.FilledRect({ 10,10 }, { 20,20 }, olc::Colour::BLUE);
		return true;
	}


	float fTotalTime = 0;
};


class Example : public olc::PixelGameEngine
{
public:
	Example()
	{

	}

	olc::Image imTest;

	olc::Image imLogo;
	olc::Image imTemp;

	olc::Image imSampleTest;

	olc::Image imLowRes;
	olc::Image imHighResSprite;

	void CreateSampleTestImage(olc::Image& image, const olc::vi2d& s)
	{
		CreateImage(image, s);
		draw.SetTarget(image);

		draw.Clear(olc::Colour::WHITE);

		std::vector<olc::Pixel> vColours = {
			olc::Colour::RED,
			olc::Colour::YELLOW,
			olc::Colour::GREEN,
			olc::Colour::CYAN,
			olc::Colour::BLUE,
			olc::Colour::MAGENTA,
		};

		for (int i = 0; i < s.x/2; i++)
		{
			draw.Rect(olc::vi2d{ i,i }, s - olc::vi2d{ (i * 2)+1 ,(i * 2)+1  },  vColours[i % vColours.size()]);
		}

		draw.Pixel({ 0.0,1 }, olc::Colour::BLACK);

	}

	float fAngle = 0.0f;


	struct logo
	{
		olc::vf2d pos;
		olc::vf2d vel;
		float ang = 0;
		float angvel = 0;
	};
	std::vector<logo> vecLogos;

	std::vector<olc::vf2d> vecVerts;

	float fScale = 1.0f;


	int nSelectedVert = -1;

	std::shared_ptr<SecondWindow> win2;

public:
	bool OnUserCreate() override
	{

		CreateImage(imLowRes, { 256, 240 });
		CreateImageFromFile(imHighResSprite, "e:/voxel.png");

		//CreateImage(imTest, { 64,64 });
		CreateImageFromFile(imLogo, "../tests/olc.png");


		CreateSampleTestImage(imSampleTest, { 32, 32 });

		CreateImage(imTemp, { 64, 64 });

		size_t x = 1;
		vecLogos.resize(x);
		for (auto& a : vecLogos)
		{
			a.pos = olc::vf2d(rand() % GetDefaultImage().Size().x, rand() % GetDefaultImage().Size().y);
			//a.vel = olc::vf2d(rand() % 100 - 50, rand() % 100 - 50);
			a.angvel = 1.1f;
		}

		vecVerts = {
			{ 100.0f,  100.0f},
			{ 200.0f, 100.0f},
			{ 200.0f, 200.0f},
			{ 100.0f,  200.0f}
		};

		
		

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		//draw.SetTarget(imTemp);
		//draw.Clear(olc::Colour::BLANK);
		//draw.FilledRect({ 20,5 }, { 6, 54 }, olc::Colour::GREEN);
		//draw.FilledRect({ 0,0 }, imTemp.Size(), olc::Colour::BLANK);


		//draw.SetTarget(imgPrimary);
		//draw.Clear(olc::Colour::BLUE);


		//draw.WorldReset();
		//draw.FilledRect({ 0,0 }, imgPrimary.Size() * olc::vf2d(0.5f, 1.0f), olc::Pixel(0, 0, 0, 1));



		draw.SetTarget(GetDefaultImage());
		draw.WorldReset();
		draw.Clear(olc::Colour::TANGERINE);
		//draw.Image(imHighResSprite, mouse.GetPosition());


		//draw.SetTarget(imLowRes);

		//draw.Clear(olc::Colour::BLANK);

		if (mouse.GetButton(1).bHeld)
		{		
			fAngle += 0.2f * fElapsedTime;
			draw.WorldRotate(fAngle, imLowRes.Size() / 2.0f);
		}

		if (mouse.GetButton(2).bPressed)
		{
			auto w = std::make_shared<SecondWindow>();
			//AddChildWindow(w, { 100,100 }, { 2, 2 });
		}

		if (mouse.GetWheel() > 0)
		{
			fScale *= 1.1f;
		}

		if (mouse.GetWheel() < 0)
		{
			fScale *= 0.9f;
		}

		draw.WorldScale({ fScale, fScale });
		//for (int x = 0; x < imgPrimary.Size().x; x++)
		//	for (int y = 0; y < imgPrimary.Size().y; y++)
		//		draw.Pixel(olc::vf2d( x, y ), olc::Pixel(rand() % 255, rand() % 255, rand() % 255));

	 ////  
		//return true;




		//std::cout << mouse.GetPosition() << "\n";
		auto vMouse = draw.ScreenToWorld(mouse.GetPosition() );

		if (mouse.GetButton(0).bPressed)
		{
			float dist = 100000.0f;
			int idx = -1;
			for (int i = 0; i < 4; i++)
			{
				float d = (vecVerts[i] - vMouse).mag();
				if (d < 8 && d < dist)
				{
					dist = d;
					idx = i;
				}
			}
			nSelectedVert = idx;
		}

		

		if (nSelectedVert != -1 && mouse.GetButton(0).bHeld)
			vecVerts[nSelectedVert] = vMouse;

		if (mouse.GetButton(0).bReleased)
			nSelectedVert = -1;

		draw.ImageQuad(imLogo.region({ 0,0 }, { 10,10 }), vecVerts);

		draw.Line(vecVerts[0], vecVerts[1], olc::Colour::MAGENTA);
		draw.Line(vecVerts[1], vecVerts[2], olc::Colour::MAGENTA);
		draw.Line(vecVerts[2], vecVerts[3], olc::Colour::MAGENTA);
		draw.Line(vecVerts[3], vecVerts[0], olc::Colour::MAGENTA);

		//draw.Image(imTemp, vMouse, { 4,4 });

		draw.Image(imSampleTest, vMouse);


		//draw.Pixel(vMouse, olc::Colour::GREEN);
		//draw.Line(vMouse, vMouse + 1,  olc::Colour::GREEN);
		
		draw.FilledRect({ 5, 100 }, { 100,100 }, olc::Colour::TANGERINE, olc::Colour::DARK_CYAN, olc::Colour::RED, olc::Colour::GREEN);
		
		draw.FilledRect({ 5,5 }, { 10,10 }, olc::Colour::YELLOW);



		draw.Rect({ 8,8 }, { 20,20 }, olc::Colour::RED);

		if(mouse.GetButton(0).bHeld)
			draw.Line({ 1.0f, 1.0f }, { 25.5f, 25.5f });





		//draw.SetTarget(imgPrimary);
		//draw.WorldReset();
		//draw.Image(imLowRes, { 0,0 }, { 4, 4 });
	
		//draw.Rect(vMouse, { 100,100 });

		/*for (auto& a : vecLogos)
		{
			a.pos += a.vel * fElapsedTime;
			a.ang += a.angvel * fElapsedTime;

			if (a.pos.x >= imgPrimary.Size().x - 46)
			{
				a.pos.x = imgPrimary.Size().x - 46;
				a.vel.x *= -1.0f;
			}

			if (a.pos.y >= imgPrimary.Size().y - 28)
			{
				a.pos.y = imgPrimary.Size().y - 28;
				a.vel.y *= -1.0f;
			}

			if (a.pos.x < 0)
			{
				a.pos.x = 0;
				a.vel.x *= -1.0f;
			}

			if (a.pos.y < 0)
			{
				a.pos.y = 0;
				a.vel.y *= -1.0f;
			}

			draw.ImageRotated(imLogo, a.pos, a.ang, { 10,10 }, { 2.0f, 4.0f });
		}*/
		


		/*draw.SetTarget(imTest);
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

		draw.FilledRect({ 200,200 }, { 10,10 }, p);*/

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
	//if (demo.Construct({ 256, 240 }, { 4, 4 }))

	olc::PGEConfig cfg;
	cfg.vPixelSize = { 1,1 };
	cfg.vScreenSize = { 1024, 960 };
	//cfg.bVSync = true;

	//if (demo.Construct({ 1280, 960 }, { 1, 1 }, cfg))
	if(demo.Construct(cfg))
		demo.Start();

	return 0;
}