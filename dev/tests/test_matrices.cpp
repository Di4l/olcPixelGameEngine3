#include <iostream>
#include <cassert>
#include <set>

#include "../src/config.h"
#include "../src/matrix3d.h"
#include "../src/transform2d.h"

extern "C" void test_matrices()
{
	olc::mf3d mat1;

	mat1.identity();

	std::cout << mat1.str() << std::endl;

	olc::tf2d trans1;
	trans1.translate(olc::vf2d{ 2, 3 });
	trans1.rotate(0.6f);

	std::cout << trans1.forward_matrix() << std::endl;


}