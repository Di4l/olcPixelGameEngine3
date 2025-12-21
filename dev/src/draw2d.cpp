#include "draw2d.h"

#include "gpu_iface.h"

//! START IMPLEMENTATION
using namespace olc;

Draw2D::Draw2D()
{
}

void Draw2D::SetGPU(olc::gpu::Renderer* const renderer)
{
	pRenderer = renderer;
}

void Draw2D::SetTarget(olc::Image& image)
{
	// Perform any outstanding tasks for current target
	ProcessGPUTasks();

	// Endure new target exists in GPU up to date
	PrepareImageForHW(image);

	// Store the target image
	pTarget = &image;	

	// Reset Affine transform to unity
	WorldReset();

	// Configure default render target
	pRenderer->AssignTextureTarget(0, pTarget->GetGPUID());
	pRenderer->SetViewport({ 0,0 }, pTarget->Size());
}

void olc::Draw2D::ProcessGPUTasks()
{
	for (const auto& task : vecGPUTasks)
		pRenderer->DoGPUTask(task);

	vecGPUTasks.clear();
}

void Draw2D::PrepareTargetForSW()
{
	if (pTarget->BoundToGPU())
	{
		// Process GPU queue bound for the target
		ProcessGPUTasks();

		// Image resource is primed for GPU operations, bring it to CPU
		pRenderer->ReadTexture(pTarget->GetGPUID(), *pTarget);

		// Image is now CPU bound
		pTarget->BindCPU();
	}
}

void Draw2D::PrepareTargetForHW()
{
	if (pTarget->BoundToCPU())
	{
		// Image resource is primed for CPU operations, send it to GPU
		pRenderer->WriteTexture(pTarget->GetGPUID(), *pTarget);

		// Image is now GPU bound
		pTarget->BindGPU();
	}
}

void Draw2D::PrepareImageForSW(olc::Image& image)
{
	if (image.BoundToGPU())
	{
		// Process GPU queue bound for the target
		ProcessGPUTasks();

		// Image resource is primed for GPU operations, bring it to CPU
		pRenderer->ReadTexture(image.GetGPUID(), image);

		// Image is now CPU bound
		image.BindCPU();
	}
}

void Draw2D::PrepareImageForHW(olc::Image& image)
{
	if (image.BoundToCPU())
	{
		// Image resource is primed for CPU operations, send it to GPU
		pRenderer->WriteTexture(image.GetGPUID(), image);

		if (&image == pTarget)
		{
			// If this image is also the current target, ensure renderer is updated
			SetTarget(image);
		}

		// Image is now GPU bound
		image.BindGPU();
	}
}

void olc::Draw2D::WorldReset()
{
	transformAffine = olc::tf2d();
}

void olc::Draw2D::WorldScale(const olc::vf2d& vScale)
{
	transformAffine.scale(vScale);
}

void olc::Draw2D::WorldOffset(const olc::vf2d& vOffset)
{
	transformAffine.translate(vOffset);
}

void olc::Draw2D::WorldRotate(const float& fTheta, const olc::vf2d& vPoint)
{
	transformAffine.rotate(fTheta, vPoint);
}

void olc::Draw2D::SetWorldTransform(const olc::tf2d& trans)
{
	transformAffine = trans;
}

olc::tf2d& olc::Draw2D::GetWorldTransform()
{
	return transformAffine;
}

olc::vf2d olc::Draw2D::WorldToScreen(const olc::vf2d& v) const
{
	return transformAffine.forward(v);
}

olc::vf2d olc::Draw2D::ScreenToWorld(const olc::vf2d& v) const
{
	return transformAffine.inverse(v);
}

void Draw2D::Pixel(const olc::vf2d& pos, const olc::Pixel col)
{
	// Check if in bounds
	olc::vf2d tpos = transformAffine.forward(pos);
	if (tpos.x >= 0 && tpos.y >= 0 && tpos.x < pTarget->Size().x && tpos.y < pTarget->Size().y)
	{
		PrepareTargetForSW();
		pTarget->Pixel(tpos) = col;
	}

	// otherwise do nothing
}

olc::Pixel olc::Draw2D::GetPixel(olc::Image& image, const olc::vf2d& pos)
{
	PrepareImageForSW(image);
	return image.Pixel(pos);
}

void olc::Draw2D::Clear(const olc::Pixel& col)
{
	//PrepareImageForHW(*pTarget);
	pRenderer->ClearViewport(col, true, true);
}

GPUTask olc::Draw2D::TaskDrawPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.bWireframe = false;
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer.push_back({ vPoints[i].x, vPoints[i].y, 1.0f, 1.0f, vColours[i], 0, 0, 0,0, 0, 0, 0, 0});
	//task.mvpMatrix = transformCombined.m
	task.tint = tint;
	return task;
}

GPUTask olc::Draw2D::TaskDrawPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const olc::Pixel colour, const olc::Pixel tint)
{	
	GPUTask task;
	task.structure = structure;
	task.bWireframe = false;
	for (const auto& v : vPoints)
		task.vertexBuffer.push_back({ v.x+0.0f, v.y+0.0f, 1.0f, 1.0f, colour, 0, 0, 0,0, 0, 0, 0, 0 });
	task.tint = tint;
	return task;
}

GPUTask olc::Draw2D::TaskFillPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer.push_back({ vPoints[i].x, vPoints[i].y, 1.0f, 1.0f, vColours[i], 0, 0, 0, 0, 0, 0, 0, 0 });
	task.tint = tint;
	return task;
}

GPUTask olc::Draw2D::TaskFillPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const olc::Pixel colour, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	for (const auto& v : vPoints)
		task.vertexBuffer.push_back({ v.x, v.y, 1.0f, 1.0f, colour, 0, 0, 0,0, 0, 0, 0, 0 });
	task.tint = tint;
	return task;	
}

GPUTask olc::Draw2D::TaskTexturedPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	GPUTask task;
	for (size_t i = 0; i<vPoints.size(); i++)
		task.vertexBuffer.push_back({ vPoints[i].x, vPoints[i].y, 1.0f, 1.0f, vColours[i], vTexCoords[i].x, vTexCoords[i].y, 0, 0, 0, 0, 0, 0});
	task.pImage = image;
	task.tint = tint;
	//task.mvpMatrix = transformCombined.m4x4();
	return task;
}

GPUTask olc::Draw2D::TaskTexturedPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::vf2d>& vZWs, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	GPUTask task;
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer.push_back({ vPoints[i].x, vPoints[i].y, vZWs[i].x, vZWs[i].y, vColours[i], vTexCoords[i].x, vTexCoords[i].y, 0, 0, 0, 0, 0, 0});
	task.pImage = image;
	task.tint = tint;
	return task;
}



const GPUTask& Draw2D::Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col)
{
	// Lines are a pain. Software Rasterization is easy, but GPU line drawing is a mess
	PrepareTargetForSW();

	auto ClipLineToDrawTarget = [&](olc::vi2d& in_p1, olc::vi2d& in_p2)
		{
			olc::vi2d vDrawTargetSize{ (int32_t)pTarget->Size().x, (int32_t)pTarget->Size().y};

			// https://en.wikipedia.org/wiki/Cohen%E2%80%93Sutherland_algorithm
			static constexpr int SEG_I = 0b0000, SEG_L = 0b0001, SEG_R = 0b0010, SEG_B = 0b0100, SEG_T = 0b1000;
			auto Segment = [&vDrawTargetSize = vDrawTargetSize](const olc::vi2d& v)
				{
					int i = SEG_I;
					if (v.x < 0) i |= SEG_L; else if (v.x > vDrawTargetSize.x) i |= SEG_R;
					if (v.y < 0) i |= SEG_B; else if (v.y > vDrawTargetSize.y) i |= SEG_T;
					return i;
				};

			int s1 = Segment(in_p1), s2 = Segment(in_p2);

			while (true)
			{
				if (!(s1 | s2))	  return true;
				else if (s1 & s2) return false;
				else
				{
					int s3 = s2 > s1 ? s2 : s1;
					olc::vi2d n;
					if (s3 & SEG_T) { n.x = in_p1.x + (in_p2.x - in_p1.x) * (vDrawTargetSize.y - in_p1.y) / (in_p2.y - in_p1.y); n.y = vDrawTargetSize.y; }
					else if (s3 & SEG_B) { n.x = in_p1.x + (in_p2.x - in_p1.x) * (0 - in_p1.y) / (in_p2.y - in_p1.y); n.y = 0; }
					else if (s3 & SEG_R) { n.x = vDrawTargetSize.x; n.y = in_p1.y + (in_p2.y - in_p1.y) * (vDrawTargetSize.x - in_p1.x) / (in_p2.x - in_p1.x); }
					else if (s3 & SEG_L) { n.x = 0; n.y = in_p1.y + (in_p2.y - in_p1.y) * (0 - in_p1.x) / (in_p2.x - in_p1.x); }
					if (s3 == s1) { in_p1 = n; s1 = Segment(in_p1); }
					else { in_p2 = n; s2 = Segment(in_p2); }
				}
			}
			return true;
		};


		auto DrawLine = [&](int32_t x1, int32_t y1, int32_t x2, int32_t y2, olc::Pixel p, uint32_t pattern)
			{
				int x, y, dx, dy, dx1, dy1, px, py, xe, ye, i;
				dx = x2 - x1; dy = y2 - y1;

				auto rol = [&](void) { pattern = (pattern << 1) | (pattern >> 31); return pattern & 1; };

				olc::vi2d p1(x1, y1), p2(x2, y2);
				if (!ClipLineToDrawTarget(p1, p2))
					return;
				x1 = p1.x; y1 = p1.y;
				x2 = p2.x; y2 = p2.y;

				// straight lines idea by gurkanctn
				if (dx == 0) // Line is vertical
				{
					if (y2 < y1) std::swap(y1, y2);
					for (y = y1; y <= y2; y++) if (rol()) Pixel(olc::vf2d(x1, y), p);
					return;
				}

				if (dy == 0) // Line is horizontal
				{
					if (x2 < x1) std::swap(x1, x2);
					for (x = x1; x <= x2; x++) if (rol()) Pixel(olc::vf2d(x, y1), p);
					return;
				}

				// Line is Funk-aye
				dx1 = abs(dx); dy1 = abs(dy);
				px = 2 * dy1 - dx1;	py = 2 * dx1 - dy1;
				if (dy1 <= dx1)
				{
					if (dx >= 0)
					{
						x = x1; y = y1; xe = x2;
					}
					else
					{
						x = x2; y = y2; xe = x1;
					}

					if (rol()) Pixel(olc::vf2d(x, y), p);

					for (i = 0; x < xe; i++)
					{
						x = x + 1;
						if (px < 0)
							px = px + 2 * dy1;
						else
						{
							if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) y = y + 1; else y = y - 1;
							px = px + 2 * (dy1 - dx1);
						}
						if (rol()) Pixel(olc::vf2d(x, y), p);
					}
				}
				else
				{
					if (dy >= 0)
					{
						x = x1; y = y1; ye = y2;
					}
					else
					{
						x = x2; y = y2; ye = y1;
					}

					if (rol()) Pixel(olc::vf2d(x, y), p);

					for (i = 0; y < ye; i++)
					{
						y = y + 1;
						if (py <= 0)
							py = py + 2 * dx1;
						else
						{
							if ((dx < 0 && dy < 0) || (dx > 0 && dy > 0)) x = x + 1; else x = x - 1;
							py = py + 2 * (dx1 - dy1);
						}
						if (rol()) Pixel(olc::vf2d(x, y), p);
					}
				}
			};


		DrawLine(p1.x, p1.y, p2.x, p2.y, col, 0xFFFFFFFF);

		return vecGPUTasks.emplace_back(); // return a dummy task

	//PrepareTargetForHW();
	///*return vecGPUTasks.emplace_back(
	//	TaskDrawPolygon(
	//		GPUTask::Structure::Line,
	//		transformAffine.forward<float>({p1, p2}),
	//		col,
	//		olc::Colour::WHITE
	//	));*/

	//	// get image target pixel size in screen space
	////olc::vf2d tsize = transformAffine.inverse<float>({ 2.75f, 2.75f }) - transformAffine.inverse<float>({ 0.0f, 0.0f });



	//// Transform line to pixel space, and select pixel centers
	//auto tP1 = transformAffine.forward<float>(p1).floor() + 0.5f;
	//auto tP2 = transformAffine.forward<float>(p2).floor() + 0.5f;

	//// Diamond exit strategy workaround - hardcode line width to 1 pixel in target space
	//olc::vf2d tsize = { 0.5f, 0.5f };
	//olc::vf2d vSlope = (tP2 - tP1).norm().perp() * tsize;


	//return vecGPUTasks.emplace_back(
	//	TaskDrawPolygon(
	//		GPUTask::Structure::Fan,
	//		/*transformAffine.forward<float>({
	//			p1 - vSlope,
	//			p1 + vSlope,
	//			p2 + vSlope,
	//			p2 - vSlope,
	//			})*/
	//		{ tP1 - vSlope, tP1 + vSlope, tP2 + vSlope, tP2 - vSlope},
	//		{ col, col, col, col },
	//		olc::Colour::WHITE
	//		));
}

const GPUTask& Draw2D::Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel c1, const olc::Pixel c2)
{
	PrepareTargetForHW();
	/*return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Line,
			transformAffine.forward<float>({ p1, p2 }),
			{ c1, c2 },
			olc::Colour::WHITE
		));*/

		// get image target pixel size in screen space
	//olc::vf2d tsize = transformAffine.inverse<float>({ 1.0f, 1.0f }) - transformAffine.inverse<float>({ 0.0f, 0.0f });

	olc::vf2d tsize = 1.0f / olc::vf2d(pTarget->Size());
	olc::vf2d vSlope = (p2 - p1).norm().perp();

	// Draw a thin polygon rectangle to represent line
	return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>({ 
				olc::vf2d( p1.x - tsize.x * vSlope.x, p1.y - tsize.y * vSlope.y),
				olc::vf2d( p2.x + tsize.x * vSlope.x, p1.y - tsize.y * vSlope.y),
				olc::vf2d( p2.x + tsize.x * vSlope.x, p2.y + tsize.y * vSlope.y),
				olc::vf2d( p1.x - tsize.x * vSlope.x, p2.y + tsize.y * vSlope.y),
				}),
			{ c1, c2, c2, c1 },
			olc::Colour::WHITE
			));
		

}

const GPUTask& olc::Draw2D::Rect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	// Right a big, hearty, F&^% you to OpenGL's Diamond Exit Strategy. It makes line drawing
	// with OpenGL a smidge unreliable

	// Check if its axis aligned
	if (std::abs(transformAffine.rotate()) < 0.0001f)
	{
		// SW Raster the line
	}

	PrepareTargetForHW();
	/*return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>({ 
				olc::vf2d( pos.x + 0.0f, pos.y + 0.0f ),
				olc::vf2d( pos.x + size.x +0.0f, pos.y + 0.0f ),
				olc::vf2d( pos.x + size.x + 0.0f, pos.y + size.y + 0.0f ),
				olc::vf2d( pos.x + 0.0f, pos.y + size.y + 0.0f ),
				}),
			col,
			olc::Colour::WHITE
		));*/


		// Draw a strip of triangles to represent the outer boundary of a rectangle
		/*return vecGPUTasks.emplace_back(
			TaskDrawPolygon(
				GPUTask::Structure::Fan,
				transformAffine.forward<float>({ 
					olc::vf2d( pos.x - 0.5f, pos.y - 0.5f ),
					olc::vf2d( pos.x + size.x + 0.5f, pos.y - 0.5f ),
					olc::vf2d( pos.x + size.x + 0.5f, pos.y + 0.5f ),
					olc::vf2d( pos.x + size.x + 0.5f, pos.y + size.y + 0.5f ),
					olc::vf2d( pos.x - 0.5f, pos.y + size.y + 0.5f ),
					olc::vf2d( pos.x - 0.5f, pos.y - 0.5f ),
					}),
				{ col, col, col, col, col, col },
				olc::Colour::WHITE
				));*/
		
		Line({ pos.x, pos.y }, { pos.x + size.x, pos.y }, col);
		Line({ pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, col);
		Line({ pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y }, col);
		return Line({ pos.x, pos.y + size.y }, { pos.x, pos.y }, col);


}


const GPUTask& olc::Draw2D::FilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskFillPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),

			col,
			olc::Colour::WHITE
		));
}

const GPUTask& olc::Draw2D::FilledRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel colTL, const olc::Pixel colTR, const olc::Pixel colBL, const olc::Pixel colBR)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskFillPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			{ colTL, colTR, colBR, colBL },
			olc::Colour::WHITE
		));
}

const GPUTask& olc::Draw2D::String(const olc::vf2d& pos, const std::string& text, const olc::Pixel col, const olc::vf2d& scale, olc::Font& font)
{
	PrepareTargetForHW();

	olc::vf2d spos = { 0.0f, 0.0f };

	for (auto c : text)
	{
		const auto& glyph = font.glyphs[c];

		if (c == '\n')
		{
			spos.x = 0; 
			spos.y += font.fLineHeight * scale.y;
		}
		else if (c == '\t')
		{
			spos.x += font.fTabWidth * scale.x;
		}
		else
		{
			Draw2D::Image(font.glyphs[c].imgGlyph, pos + spos, scale, col);
			spos.x += glyph.vMonoSize.x * scale.x;
		}
	}

	return vecGPUTasks.emplace_back();
}

const GPUTask& olc::Draw2D::StringProp(const olc::vf2d& pos, const std::string& text, const olc::Pixel col, const olc::vf2d& scale, olc::Font& font)
{
	PrepareTargetForHW();

	olc::vf2d spos = { 0.0f, 0.0f };

	for (auto c : text)
	{
		const auto& glyph = font.glyphs[c];

		if (c == '\n')
		{
			spos.x = 0;
			spos.y += font.fLineHeight * scale.y;
		}
		else if (c == '\t')
		{
			spos.x += font.fTabWidth * scale.x;
		}
		else
		{
			Draw2D::Image(font.glyphs[c].imgGlyph, pos + spos, scale, col);
			spos.x += glyph.vPropSize.x * scale.x;
		}
	}

	return vecGPUTasks.emplace_back();
}

olc::vf2d olc::Draw2D::GetTextSize(const std::string& text, const bool bProportional, const olc::vf2d& scale, olc::Font& font)
{	
	olc::vf2d size = { 0, font.fLineHeight * scale.y };
	olc::vf2d pos = { 0, font.fLineHeight * scale.y };

	for (auto c : text)
	{
		const auto& glyph = font.glyphs[c];

		if (c == '\n') 
		{ 
			pos.y += font.fLineHeight * scale.y;
			pos.x = 0; 
		}
		else if (c == '\t') 
		{ 
			pos.x += font.fTabWidth * scale.x; 
		}
		else
		{
			if(bProportional)
				pos.x += glyph.vPropSize.x * scale.x;
			else
				pos.x += glyph.vMonoSize.x * scale.x;
		}

		size = size.max(pos);
	}

	return size;	
}

const GPUTask& olc::Draw2D::Image(olc::ImageRegion image, const olc::vf2d& pos, const olc::vf2d& scale, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);
	
	PrepareTargetForHW();

	olc::vf2d size = image.regionsize * scale;

	return vecGPUTasks.emplace_back(
		TaskTexturedPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forwardRound<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			{ tint, tint, tint, tint },
			// Tex coords are clockwise
			{ image.coords[0], image.coords[1], image.coords[2], image.coords[3]},
			&image.image
		));
}

const GPUTask& olc::Draw2D::ImageRotated(olc::ImageRegion image, const olc::vf2d& pos, const float theta, const olc::vf2d& center, const olc::vf2d& scale, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);
	PrepareTargetForHW();

	olc::vf2d size = image.regionsize * scale;

	std::vector<olc::vf2d> vPoints(4);
	vPoints[0] = (olc::vf2d(0.0f, 0.0f) - center) * scale;
	vPoints[1] = (olc::vf2d(size.x, 0.0f) - center) * scale;
	vPoints[2] = (size - center) * scale;
	vPoints[3] = (olc::vf2d(0.0f, size.y) - center) * scale;

	float c = cos(theta), s = sin(theta);
	for (int i = 0; i < 4; i++)
		vPoints[i] = pos + olc::vf2d(vPoints[i].x * c - vPoints[i].y * s, vPoints[i].x * s + vPoints[i].y * c);

	return vecGPUTasks.emplace_back(
		TaskTexturedPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>(vPoints),
			{ tint, tint, tint, tint},
			{ image.coords[0], image.coords[1], image.coords[2], image.coords[3] },
			&image.image
		));
}

const GPUTask& olc::Draw2D::ImageQuad(olc::ImageRegion image, const olc::vf2d& vTL, const olc::vf2d& vTR, const olc::vf2d& vBR, const olc::vf2d& vBL, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);
	PrepareTargetForHW();


	float rd = ((vBR.x - vTL.x) * (vTR.y - vBL.y) - (vTR.x - vBL.x) * (vBR.y - vTL.y));
	if (rd != 0)
	{
		rd = 1.0f / rd;
		float rn = ((vTR.x - vBL.x) * (vTL.y - vBL.y) - (vTR.y - vBL.y) * (vTL.x - vBL.x)) * rd;
		float sn = ((vBR.x - vTL.x) * (vTL.y - vBL.y) - (vBR.y - vTL.y) * (vTL.x - vBL.x)) * rd;
		
		olc::vf2d center;
		if (!(rn < 0.f || rn > 1.f || sn < 0.f || sn > 1.f)) 
			center = vTL + rn * (vBR - vTL);

		std::array<float, 4> d = { {
			(vTL - center).mag(), 
			(vTR - center).mag(), 
			(vBR - center).mag(), 
			(vBL - center).mag(),
		} };

		std::array<float, 4> q = { {
			d[0] == 0.0f ? 1.0f : (d[0] + d[2]) / d[2],
			d[1] == 0.0f ? 1.0f : (d[1] + d[3]) / d[3],
			d[2] == 0.0f ? 1.0f : (d[2] + d[0]) / d[0],
			d[3] == 0.0f ? 1.0f : (d[3] + d[1]) / d[1],
		} };
	
		return vecGPUTasks.emplace_back(
			TaskTexturedPolygon(
				GPUTask::Structure::Fan,
				transformAffine.forward<float>({ vTL, vTR, vBR, vBL }),
				{ {q[0], 1.0f}, {q[1], 1.0f}, {q[2], 1.0f}, {q[3], 1.0f} },
				{ tint, tint, tint, tint},
				{ image.coords[0] * q[0], image.coords[1] * q[1], image.coords[2] * q[2], image.coords[3] * q[3] },
				&image.image
			));		
	}

	// Default is just return a textured quad
	return vecGPUTasks.emplace_back(
		TaskTexturedPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>({ vTL, vTR, vBR, vBL }),
			{ tint, tint, tint, tint },
			{ image.coords[0], image.coords[1], image.coords[2], image.coords[3] },
			&image.image
		));
}

const GPUTask& olc::Draw2D::ImageQuad(olc::ImageRegion image, const std::vector<olc::vf2d>& vecPoints, const olc::Pixel tint)
{
	return ImageQuad(image, vecPoints[0], vecPoints[1], vecPoints[2], vecPoints[3], tint);
}


const GPUTask& olc::Draw2D::ImageRect(olc::ImageRegion image, const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel tint)
{
	// Ensure source image is up to date in VRAM
	PrepareImageForHW(image.image);

	PrepareTargetForHW();

	return vecGPUTasks.emplace_back(
		TaskTexturedPolygon(
			GPUTask::Structure::Fan,
			transformAffine.forward<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			{ tint, tint, tint, tint },
			// Tex coords are clockwise
			{ image.coords[3], image.coords[2], image.coords[1], image.coords[0] },
			&image.image
		));
}

//! END IMPLEMENTATION