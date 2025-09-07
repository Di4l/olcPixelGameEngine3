#define OLC_PGE_APPLICATION
#include "olcpge3.h"

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{

	}

	olc::Image imTest;

public:
	bool OnUserCreate() override
	{
		CreateImage(imTest, { 128,128 });
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		draw.Line({ 0,0 }, { 200,200 }, olc::Colour::WHITE);

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