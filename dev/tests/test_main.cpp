
extern "C" void test_pixels();
extern "C" void test_vector2d();
extern "C" void test_matrices();

#include "api_opengl.h"

int main()
{
	test_pixels();
	test_vector2d();
	test_matrices();


	auto& gl = olc::apis::opengl::gl::Get();

	gl.glBindBuffer(0, 0);

	return 0;
}