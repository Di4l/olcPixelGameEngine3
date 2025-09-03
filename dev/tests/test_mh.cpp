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
		draw.Line({ 10,10 }, { 20,20 }, olc::Colour::BLUE);

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