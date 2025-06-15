#include <iostream>

#include "../src/config.h"
#include "../src/pixel.h"

int main()
{
	olc::Pixel p;

	p = p * 7;
	p = p * 3.9f;

	
	
	p = olc::Colour::CYAN;

	return 0;
}