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
	const auto vTransformedPoints = transformAffine.forward<float>({ p1, p2 });
	swRasterShadedLine(
		vTransformedPoints[0],
		vTransformedPoints[1],
		c1, c2);	
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

void olc::Draw2D::swFilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	swFilledRect(pos, size, col, col, col, col);
}

void olc::Draw2D::swFilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR)
{
	const auto vTransformedPoints = transformAffine.forward<float>({pos, {pos.x + size.x, pos.y}, pos + size, {pos.x, pos.y + size.y}});
	
	// Most draws will be single colour, axis aligned rectangle. 
	// Optimise for that case first

	// Check if all one colour
	if (colTL == colBL && colTL == colTR && colTL == colBR)
	{
		// Check if axis aligned
		if(vTransformedPoints[0].y == vTransformedPoints[1].y &&
		   vTransformedPoints[1].x == vTransformedPoints[2].x &&
		   vTransformedPoints[2].y == vTransformedPoints[3].y &&
		   vTransformedPoints[3].x == vTransformedPoints[0].x)
		{
			PrepareTargetForSW();

			// Clip to target
			olc::vi2d p1 = vTransformedPoints[0].max({ 0,0 });
			olc::vi2d p2 = vTransformedPoints[2].min(pTarget->Size());
			
			// Draw filled rectangle
			for (int32_t y = p1.y; y < p2.y; y++)
				for (int32_t x = p1.x; x < p2.x; x++)
					pTarget->Pixel({ x, y }) = colTL;

			// Exit early
			return;
		}		
	}

	// Fallback to general case rasteriser, where we split into two triangles
	swRasterShadedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[1],
		vTransformedPoints[2],
		colTL, colTR, colBR);
	swRasterShadedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[2],
		vTransformedPoints[3],
		colTL, colBR, colBL);			
}

void olc::Draw2D::swTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col)
{
	swTriangle(p1, p2, p3, col, col, col);
}

void olc::Draw2D::swTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3)
{
	swLine(p1, p2, c1, c2);
	swLine(p2, p3, c2, c3);
	swLine(p3, p1, c3, c1);
}

void olc::Draw2D::swFilledTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel col)
{
	swFilledTriangle(p1, p2, p3, col, col, col);
}

void olc::Draw2D::swFilledTriangle(const olc::vf2d& p1, const olc::vf2d& p2, const olc::vf2d& p3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3)
{
	const auto vTransformedPoints = transformAffine.forward<float>({ p1, p2, p3 });
	swRasterShadedTriangle(
		vTransformedPoints[0],
		vTransformedPoints[1],
		vTransformedPoints[2],
		c1, c2, c3);
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

void olc::Draw2D::swRasterShadedTriangle(const olc::vi2d& v1, const olc::vi2d& v2, const olc::vi2d& v3, const olc::Pixel c1, const olc::Pixel c2, const olc::Pixel c3)
{
	// Get Height of triangle in whole pixels
	int32_t nMinY = std::min({ v1.y, v2.y, v3.y });
	int32_t nHeight = std::max({ v1.y, v2.y, v3.y }) - nMinY;

	// Maybe... unless we want 1 pixel high triangles (we do)
	if (nHeight <= 0)
		return; // Degenerate triangle

	struct Scanline
	{
		int32_t nMin = std::numeric_limits<int32_t>::max();
		int32_t nMax = std::numeric_limits<int32_t>::min();
		float fBaryMin[3];
		float fBaryMax[3];
	};

	// Allocate buffer to store scanline data
	std::vector<Scanline> vScanlines(nHeight+1);

	auto scanEdge = [&](int x0, int y0, int x1, int y1, int vertexIndex1, int vertexIndex2)
		{
			if (y0 == y1) return;

			// Ensure y0 < y1
			bool swapped = false;
			if (y0 > y1)
			{
				std::swap(x0, x1);
				std::swap(y0, y1);
				swapped = true;
			}

			int dy = y1 - y0;
			float dx_step = (x1 - x0) / float(dy);
			float x = x0;

			for (int y = y0; y <= y1; y++)
			{
				int idx = y - nMinY;
				if (idx >= 0 && idx < vScanlines.size())
				{
					int ix = (int)std::round(x);

					// interpolation along edge 
					float t = (y - y0) / float(dy);
					float bary[3] = { 0.0f, 0.0f, 0.0f };

					if (swapped)
					{
						bary[vertexIndex1] = t;
						bary[vertexIndex2] = 1.0f - t;
					}
					else
					{
						bary[vertexIndex1] = 1.0f - t;
						bary[vertexIndex2] = t;
					}

					if (ix < vScanlines[idx].nMin)
					{
						vScanlines[idx].nMin = ix;
						vScanlines[idx].fBaryMin[0] = bary[0];
						vScanlines[idx].fBaryMin[1] = bary[1];
						vScanlines[idx].fBaryMin[2] = bary[2];
					}
					if (ix > vScanlines[idx].nMax)
					{
						vScanlines[idx].nMax = ix;
						vScanlines[idx].fBaryMax[0] = bary[0];
						vScanlines[idx].fBaryMax[1] = bary[1];
						vScanlines[idx].fBaryMax[2] = bary[2];
					}
				}
				x += dx_step;
			}
		};

	// Rasterise triangle edges into scanline buffer
	scanEdge(v1.x, v1.y, v2.x, v2.y, 0, 1);
	scanEdge(v1.x, v1.y, v3.x, v3.y, 0, 2);
	scanEdge(v2.x, v2.y, v3.x, v3.y, 1, 2);

	PrepareTargetForSW();

	// Lambda to draw a pixel at integer location
	auto Plot = [&](int32_t x, int32_t y, const olc::Pixel& p)
		{
			if (x >= 0 && x < pTarget->Size().x && y >= 0 && y < pTarget->Size().y)
				pTarget->Pixel({ x, y }) = p;
		};

	// Now draw the scanlines
	int32_t y = nMinY;
	for (const auto& scanline : vScanlines)
	{
		int32_t xStart = scanline.nMin;
		int32_t xEnd = scanline.nMax;

		float fSpan = float(xEnd - xStart);
		float fSpanStep = fSpan > 0.0f ? 1.0f / fSpan : 0.0f;
		float t0 = scanline.fBaryMin[0];
		float t1 = scanline.fBaryMin[1];
		float t2 = scanline.fBaryMin[2];
		for (int32_t x = xStart; x <= xEnd; x++)
		{
			olc::Pixel col = olc::Pixel(
				uint8_t(c1.r * t0 + c2.r * t1 + c3.r * t2),
				uint8_t(c1.g * t0 + c2.g * t1 + c3.g * t2),
				uint8_t(c1.b * t0 + c2.b * t1 + c3.b * t2),
				255);

			//col = olc::PixelF(scanline.fBaryMin[0], 0, 0);

			Plot(x, y, col);

			t0 += fSpanStep * (scanline.fBaryMax[0] - scanline.fBaryMin[0]);
			t1 += fSpanStep * (scanline.fBaryMax[1] - scanline.fBaryMin[1]);
			t2 += fSpanStep * (scanline.fBaryMax[2] - scanline.fBaryMin[2]);
		}

		y++;
	}


	return;




	

	olc::vi2d p1 = v1;
	olc::vi2d p2 = v2;
	olc::vi2d p3 = v3;
	olc::Pixel vColour[3] = { c1, c2, c3 };

	if (p2.y < p1.y) { std::swap(p1.y, p2.y); std::swap(p1.x, p2.x); std::swap(vColour[0], vColour[1]); }
	if (p3.y < p1.y) { std::swap(p1.y, p3.y); std::swap(p1.x, p3.x); std::swap(vColour[0], vColour[2]); }
	if (p3.y < p2.y) { std::swap(p2.y, p3.y); std::swap(p2.x, p3.x); std::swap(vColour[1], vColour[2]); }

	olc::vi2d dPos1 = p2 - p1;
	int dcr1 = vColour[1].r - vColour[0].r;
	int dcg1 = vColour[1].g - vColour[0].g;
	int dcb1 = vColour[1].b - vColour[0].b;
	int dca1 = vColour[1].a - vColour[0].a;

	olc::vi2d dPos2 = p3 - p1;
	int dcr2 = vColour[2].r - vColour[0].r;
	int dcg2 = vColour[2].g - vColour[0].g;
	int dcb2 = vColour[2].b - vColour[0].b;
	int dca2 = vColour[2].a - vColour[0].a;

	float dax_step = 0, dbx_step = 0, dcr1_step = 0, dcr2_step = 0, dcg1_step = 0, dcg2_step = 0, dcb1_step = 0, dcb2_step = 0, dca1_step = 0, dca2_step = 0;
	olc::vf2d vTex1Step, vTex2Step;

	if (dPos1.y)
	{
		dax_step = dPos1.x / (float)abs(dPos1.y);
		dcr1_step = dcr1 / (float)abs(dPos1.y);
		dcg1_step = dcg1 / (float)abs(dPos1.y);
		dcb1_step = dcb1 / (float)abs(dPos1.y);
		dca1_step = dca1 / (float)abs(dPos1.y);
	}

	if (dPos2.y)
	{
		dbx_step = dPos2.x / (float)abs(dPos2.y);
		dcr2_step = dcr2 / (float)abs(dPos2.y);
		dcg2_step = dcg2 / (float)abs(dPos2.y);
		dcb2_step = dcb2 / (float)abs(dPos2.y);
		dca2_step = dca2 / (float)abs(dPos2.y);
	}

	olc::vi2d vStart;
	olc::vi2d vEnd;
	int vStartIdx;

	for (int pass = 0; pass < 2; pass++)
	{
		if (pass == 0)
		{
			vStart = p1; vEnd = p2;	vStartIdx = 0;
		}
		else
		{
			dPos1 = p3 - p2;
			dcr1 = vColour[2].r - vColour[1].r;
			dcg1 = vColour[2].g - vColour[1].g;
			dcb1 = vColour[2].b - vColour[1].b;
			dca1 = vColour[2].a - vColour[1].a;
			dcr1_step = 0; dcg1_step = 0; dcb1_step = 0; dca1_step = 0;

			if (dPos2.y) dbx_step = dPos2.x / (float)abs(dPos2.y);
			if (dPos1.y)
			{
				dax_step = dPos1.x / (float)abs(dPos1.y);
				dcr1_step = dcr1 / (float)abs(dPos1.y);
				dcg1_step = dcg1 / (float)abs(dPos1.y);
				dcb1_step = dcb1 / (float)abs(dPos1.y);
				dca1_step = dca1 / (float)abs(dPos1.y);
			}

			vStart = p2; vEnd = p3; vStartIdx = 1;
		}

		if (dPos1.y)
		{
			for (int i = vStart.y; i <= vEnd.y; i++)
			{
				int ax = int(std::round(vStart.x + (float)(i - vStart.y) * dax_step));
				int bx = int(std::round(p1.x + (float)(i - p1.y) * dbx_step));

	
				olc::Pixel col_s(vColour[vStartIdx].r + uint8_t((float)(i - vStart.y) * dcr1_step), vColour[vStartIdx].g + uint8_t((float)(i - vStart.y) * dcg1_step),
					vColour[vStartIdx].b + uint8_t((float)(i - vStart.y) * dcb1_step), vColour[vStartIdx].a + uint8_t((float)(i - vStart.y) * dca1_step));

				olc::Pixel col_e(vColour[0].r + uint8_t((float)(i - p1.y) * dcr2_step), vColour[0].g + uint8_t((float)(i - p1.y) * dcg2_step),
					vColour[0].b + uint8_t((float)(i - p1.y) * dcb2_step), vColour[0].a + uint8_t((float)(i - p1.y) * dca2_step));

				if (ax > bx) 
				{ 					
					bx = int(std::round(vStart.x + (float)(i - vStart.y) * dax_step));
					ax = int(std::round(p1.x + (float)(i - p1.y) * dbx_step));
					std::swap(col_s, col_e); 
				}

				float tstep = 1.0f / ((float)(bx - ax));
				float t = 0.0f;

				for (int j = ax; j <= bx; j++)
				{
					olc::Pixel pixel = PixelLerp(col_s, col_e, t);
					Plot(j, i, pixel);
					t += tstep;
				}
			}
		}
	}
}

void olc::Draw2D::swRasterShadedLine(const olc::vi2d& v1, const olc::vi2d& v2, const olc::Pixel c1, const olc::Pixel c2)
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
	olc::vf2d clipped_p1 = v1;
	olc::vf2d clipped_p2 = v2;

	// If line is completely outside bounds, exit
	if (!swClipLine(clipped_p1, clipped_p2, { 0,0 }, pTarget->Size()))
		return;

	// Move to integer space
	olc::vi2d ip1 = v1;// clipped_p1;// .floor();
	olc::vi2d ip2 = v2;// clipped_p2;// .floor();
	olc::vi2d pixel;

	// Calculate deltas
	int dx = ip2.x - ip1.x;
	int dy = ip2.y - ip1.y;
	int absDx = std::abs(dx);
	int absDy = std::abs(dy);

	// Determine dominant axis
	bool xMajor = absDx >= absDy;
	int steps = xMajor ? absDx : absDy;

	// Handle degenerate case (single pixel)
	if (steps == 0)
	{
		Plot(ip1.x, ip1.y, c1);
		return;
	}

	// Calculate step increments
	float xStep = float(dx) / float(steps);
	float yStep = float(dy) / float(steps);
	float colorStep = 1.0f / float(steps);

	// Starting position and color interpolation parameter
	float x = ip1.x;
	float y = ip1.y;
	float t = 0.0f;

	// Draw line pixel by pixel
	for (int i = 0; i <= steps; i++)
	{
		// Interpolate color
		olc::Pixel col = olc::PixelLerp(c1, c2, t);

		// Plot pixel
		Plot((int)std::round(x), (int)std::round(y), col);

		// Step to next pixel
		x += xStep;
		y += yStep;
		t += colorStep;
	}



	return;



	//// Gradients
	//olc::vi2d diff1 = ip2 - ip1;

	//// Colour interpolation variables
	//float fColourT = 0.0f;
	//float fColourStep = 1.0f / float(std::max(std::abs(diff1.x), std::abs(diff1.y)));

	//// Quick draw straight lines
	//if (diff1.x == 0) // Line is vertical
	//{
	//	if (ip2.y < ip1.y)
	//	{
	//		std::swap(ip1.y, ip2.y);
	//		fColourStep *= -1.0f;
	//		fColourT = 1.0f;
	//	}

	//	for (pixel.y = ip1.y; pixel.y <= ip2.y; pixel.y++)
	//	{
	//		if (rol())
	//			Plot(ip1.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));

	//		fColourT += fColourStep;
	//	}

	//	// Early exit
	//	return;
	//}

	//if (diff1.y == 0) // Line is horizontal
	//{
	//	if (ip2.x < ip1.x)
	//	{
	//		std::swap(ip1.x, ip2.x);
	//		fColourStep *= -1.0f;
	//		fColourT = 1.0f;
	//	}


	//	for (pixel.x = ip1.x; pixel.x <= ip2.x; pixel.x++)
	//	{
	//		if (rol())
	//			Plot(pixel.x, ip1.y, olc::PixelLerp(c1, c2, fColourT));

	//		fColourT += fColourStep;
	//	}

	//	// Early exit
	//	return;
	//}

	//// Line is sloped
	//olc::vi2d diff2 = diff1.abs();

	//// Apply Bresenham algorithm
	//olc::vi2d p = { diff2.y - diff2.x * 2, diff2.x - diff2.y * 2 };
	//olc::vi2d end = { 0,0 };

	//if (diff2.y <= diff2.x) // Propoagate in x-direction
	//{
	//	if (diff1.x >= 0)
	//	{
	//		pixel = ip1;
	//		end = ip2;
	//	}
	//	else
	//	{
	//		pixel = ip2;
	//		end = ip1;
	//		fColourStep *= -1.0f;
	//		fColourT = 1.0f;
	//	}

	//	if (rol())
	//		Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));

	//	for (int i = 0; pixel.x < end.x; i++)
	//	{
	//		pixel.x = pixel.x + 1;
	//		fColourT += fColourStep;

	//		if (p.x < 0)
	//			p.x = p.x + 2 * diff2.y;
	//		else
	//		{
	//			if ((diff1.x < 0 && diff1.y < 0) || (diff1.x > 0 && diff1.y > 0))
	//				pixel.y = pixel.y + 1;
	//			else
	//				pixel.y = pixel.y - 1;

	//			p.x = p.x + 2 * (diff2.y - diff2.x);
	//		}

	//		if (rol())
	//			Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));
	//	}
	//}
	//else
	//{
	//	if (diff1.y >= 0) // Propogate in y-direction
	//	{
	//		pixel = ip1;
	//		end = ip2;
	//	}
	//	else
	//	{
	//		pixel = ip2;
	//		end = ip1;
	//		fColourStep *= -1.0f;
	//		fColourT = 1.0f;
	//	}

	//	if (rol())
	//		Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));

	//	for (int i = 0; pixel.y < end.y; i++)
	//	{
	//		pixel.y = pixel.y + 1;
	//		fColourT += fColourStep;

	//		if (p.y <= 0)
	//			p.y = p.y + 2 * diff2.x;
	//		else
	//		{
	//			if ((diff1.x < 0 && diff1.y < 0) || (diff1.x > 0 && diff1.y > 0))
	//				pixel.x = pixel.x + 1;
	//			else
	//				pixel.x = pixel.x - 1;

	//			p.y = p.y + 2 * (diff2.x - diff2.y);
	//		}

	//		if (rol())
	//			Plot(pixel.x, pixel.y, olc::PixelLerp(c1, c2, fColourT));
	//	}
	//}
}



//! END IMPLEMENTATION