#include <iostream>
#include <cassert>
#include <set>

#include "../src/config.h"
#include "../src/pixel.h"

extern "C" void test_pixels()
{
	olc::Pixel p1;
	olc::Pixel p2(olc::Colour::TANGERINE);
	olc::Pixel p3(0x22446688);
	olc::Pixel p4(100, 200, 50, 255);
	olc::Pixel p5 = p2;
	olc::Pixel p6(p5);

	olc::Pixel p7 = p2 + p3;
	olc::Pixel p8 = p3 - p2;
	olc::Pixel p9 = p2 * 2.0f;
	olc::Pixel p10 = p2 * 0.5f;
	olc::Pixel p11 = p2 * 2;
	olc::Pixel p12 = p2 / 2;

	p2 += p3;
	p2 -= p3;
	p3 *= 2.0f;
	p3 /= 2.0f;
	p4 /= 2;
	p4 *= 2;

	assert(p6 == p5);
	assert(p6 != p4);

	olc::Pixel p13 = olc::PixelF(0.5f, 1.0f, 0.2f, 1.0f);
	olc::Pixel p14 = olc::PixelHSV(270.0f, 0.5f, 0.5f);

		
	std::set<olc::Pixel> s;
	s.insert(p1);
	s.insert(p2);
	s.insert(p5);
	s.insert(p6);
	assert(s.size() == 3);

	std::cout << olc::Colour::TANGERINE << "\n";
}