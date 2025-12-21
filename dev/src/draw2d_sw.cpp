#include "draw2d.h"

#include "gpu_iface.h"



//! START IMPLEMENTATION

// This is all essentially the olc::PixelGameEngine 2 rasteriser code
using namespace olc;


void Draw2D::swLine(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col)
{
	swLine(p1, p2, col, col);
}

void Draw2D::swLine(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel c1, const olc::Pixel c2)
{
	PrepareTargetForSW();

	// Lambda to draw a pixel gated by a pattern bit
	uint32_t pattern = 0xFFFFFFFF;
	auto rol = [&](void) 
	{ 
		pattern = (pattern << 1) | (pattern >> 31); 
		return pattern & 1; 
	};

	// Lambda to draw a pixel at integer location
	auto Plot = [&](int32_t x, int32_t y, const olc::Pixel& p)
	{
		if (x >= 0 && x < pTarget->Size().x && y >= 0 && y < pTarget->Size().y)
			pTarget->Pixel({ x, y }) = p;
	};

	// Clip line to draw target
	olc::vf2d clipped_p1 = transformAffine.forward<float>(p1);
	olc::vf2d clipped_p2 = transformAffine.forward<float>(p2);

	// If line is completely outside bounds, exit
	if (!swClipLine(clipped_p1, clipped_p2, { 0,0 }, pTarget->Size())) 
		return;

	// Move to integer space
	olc::vi2d ip1 = clipped_p1.floor();
	olc::vi2d ip2 = clipped_p2.floor();
	olc::vi2d pixel;

	// Gradients
	olc::vi2d diff1 = ip2 - ip1;

	// Colour interpolation variables
	float fColourT = 0.0f;
	float fColourStep = 1.0f / float(std::max(std::abs(diff1.x), std::abs(diff1.y)));

	// Quick draw straight lines
	if (diff1.x == 0) // Line is vertical
	{
		if (ip2.y < ip1.y)
		{
			std::swap(ip1.y, ip2.y);
			fColourStep *= -1.0f;
			fColourT = 1.0f;
		}		

		for (pixel.y = ip1.y; pixel.y <= ip2.y; pixel.y++)
		{
			if (rol())
				Plot(ip1.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));

			fColourT += fColourStep;
		}
		
		// Early exit
		return;
	}

	if (diff1.y == 0) // Line is horizontal
	{
		if (ip2.x < ip1.x)
		{
			std::swap(ip1.x, ip2.x);
			fColourStep *= -1.0f;
			fColourT = 1.0f;
		}
		

		for (pixel.x = ip1.x; pixel.x <= ip2.x; pixel.x++)
		{
			if (rol())
				Plot(pixel.x, ip1.y, olc::PixelLerp(c1, c2, fColourT));

			fColourT += fColourStep;
		}

		// Early exit
		return;
	}

	// Line is sloped
	olc::vi2d diff2 = diff1.abs();

	// Apply Bresenham algorithm
	olc::vi2d p = { diff2.y - diff2.x * 2, diff2.x - diff2.y * 2 };	
	olc::vi2d end = { 0,0 };
	
	if (diff2.y <= diff2.x) // Propoagate in x-direction
	{
		if (diff1.x >= 0)
		{
			pixel = ip1;
			end = ip2;
		}
		else
		{
			pixel = ip2;
			end = ip1;
			fColourStep *= -1.0f;
			fColourT = 1.0f;
		}

		if (rol()) 
			Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));

		for (int i = 0; pixel.x < end.x; i++)
		{
			pixel.x = pixel.x + 1;
			fColourT += fColourStep;

			if (p.x < 0)
				p.x = p.x + 2 * diff2.y;
			else
			{
				if ((diff1.x < 0 && diff1.y < 0) || (diff1.x > 0 && diff1.y > 0)) 
					pixel.y = pixel.y + 1;
				else 
					pixel.y = pixel.y - 1;

				p.x = p.x + 2 * (diff2.y - diff2.x);
			}

			if (rol()) 
				Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));
		}
	}
	else
	{
		if (diff1.y >= 0) // Propogate in y-direction
		{		
			pixel = ip1;
			end = ip2;
		}
		else
		{			
			pixel = ip2;
			end = ip1;
			fColourStep *= -1.0f;
			fColourT = 1.0f;
		}

		if (rol()) 
			Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));

		for (int i = 0; pixel.y < end.y; i++)
		{
			pixel.y = pixel.y + 1;
			fColourT += fColourStep;

			if (p.y <= 0)
				p.y = p.y + 2 * diff2.x;
			else
			{
				if ((diff1.x < 0 && diff1.y < 0) || (diff1.x > 0 && diff1.y > 0)) 
					pixel.x = pixel.x + 1;
				else 
					pixel.x = pixel.x - 1;

				p.y = p.y + 2 * (diff2.x - diff2.y);
			}

			if (rol()) 
				Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));
		}
	}
}

void olc::Draw2D::swRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	swRect(pos, size, col, col, col, col);
}

void olc::Draw2D::swRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR)
{
	swLine(pos, { pos.x + size.x, pos.y }, colTL, colTR);
	swLine({ pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, colTR, colBR);
	swLine({ pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y }, colBR, colBL);
	swLine({ pos.x, pos.y + size.y }, pos, colBL, colTL);
}

bool olc::Draw2D::swClipLine(olc::vf2d& p1, olc::vf2d& p2, const olc::vf2d& vMin, const olc::vf2d& vMax)
{
	// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
	static constexpr int SEG_I = 0b0000, SEG_L = 0b0001, SEG_R = 0b0010, SEG_B = 0b0100, SEG_T = 0b1000;
	auto Segment = [&vMin = vMin, &vMax = vMax](const olc::vi2d& v)
		{
			int i = SEG_I;
			if (v.x < vMin.x) i |= SEG_L; else if (v.x > vMax.x) i |= SEG_R;
			if (v.y < vMin.y) i |= SEG_B; else if (v.y > vMax.y) i |= SEG_T;
			return i;
		};

	int s1 = Segment(p1), s2 = Segment(p2);

	while (true)
	{
		if (!(s1 | s2))	  return true;
		else if (s1 & s2) return false;
		else
		{
			int s3 = s2 > s1 ? s2 : s1;
			olc::vf2d n;
			if (s3 & SEG_T) { n.x = p1.x + (p2.x - p1.x) * (vMax.y - p1.y) / (p2.y - p1.y); n.y = vMax.y; }
			else if (s3 & SEG_B) { n.x = p1.x + (p2.x - p1.x) * (vMin.y - p1.y) / (p2.y - p1.y); n.y = vMin.y; }
			else if (s3 & SEG_R) { n.x = vMax.x; n.y = p1.y + (p2.y - p1.y) * (vMax.x - p1.x) / (p2.x - p1.x); }
			else if (s3 & SEG_L) { n.x = vMin.x; n.y = p1.y + (p2.y - p1.y) * (vMin.x - p1.x) / (p2.x - p1.x); }
			if (s3 == s1) { p1 = n; s1 = Segment(p1); }
			else { p2 = n; s2 = Segment(p2); }
		}
	}
	return true;
}



//! END IMPLEMENTATION