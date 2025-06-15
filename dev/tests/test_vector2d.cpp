#include <iostream>
#include <cassert>
#include <set>

#include "../src/config.h"
#include "../src/vector2d.h"

extern "C" void test_vector2d()
{
	olc::vf2d v1 = { 3, 4 };
	v1.mag();

	v1.xy[0] = 11;

	auto s = sizeof(olc::vf2d);

	std::cout << v1 << "\n";  
}