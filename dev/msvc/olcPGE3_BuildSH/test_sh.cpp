#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine3.h"

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{

	}

public:
	bool OnUserCreate() override
	{
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		draw.Line({ 0,0 }, { 1, 1 }, olc::Colour::RED);
		draw.Line({ 0,0 }, { 1, -1 }, olc::Colour::GREEN);
		draw.Line({ 0,0 }, { -1, -1 }, olc::Colour::CYAN);
		draw.Line({ 0,0 }, { -1, 1 }, olc::Colour::WHITE);

		draw.Pixel({ -0.5f, -0.25f }, olc::Colour::YELLOW);

		draw.FillRect({ 0.25f, 0.25f }, { 0.3f, 0.6f }, olc::Colour::TANGERINE);

		draw.Pixel({ 0.3f, 0.3f }, olc::Colour::BLACK);
		draw.Pixel({ 0.32f, 0.3f }, olc::Colour::BLACK);
		draw.Pixel({ 0.34f, 0.3f }, olc::Colour::BLACK);
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