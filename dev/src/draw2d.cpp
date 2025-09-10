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
	// Store the target image
	pTarget = &image;	

	// Create a transform to denormalise the image and present it 
	// in pixel space
	transformTarget.scale(olc::vf2d(2.0f, -2.0f) / olc::vf2d(pTarget->Size()));
	transformTarget.translate(
		olc::vf2d(
			-1.0 + (1.0f / float(pTarget->Size().x)),
			1.0f - (1.0f / float(pTarget->Size().y))
		));

	AffineReset();
	
}

void Draw2D::ClearTransform()
{
	transformTarget = olc::tf2d();
	transformAffine = olc::tf2d();
	transformCombined = olc::mf3d();
}

void Draw2D::SetTransform(const olc::tf2d& trans)
{
	transformTarget = trans;
}

tf2d& const olc::Draw2D::GetTransform()
{
	return transformAffine;
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

void olc::Draw2D::AffineReset()
{
	transformAffine = olc::tf2d();
	transformCombined = transformAffine * transformTarget;
}

void olc::Draw2D::AffineScale(const olc::vf2d& vScale)
{
	transformAffine.scale(vScale);
	transformCombined = transformAffine * transformTarget;
}

void olc::Draw2D::AffineOffset(const olc::vf2d& vOffset)
{
	transformAffine.translate(vOffset);
	transformCombined = transformAffine * transformTarget;
}

void olc::Draw2D::AffineRotate(const float& fTheta, const olc::vf2d& vPoint)
{
	transformAffine.rotate(fTheta, vPoint);
	transformCombined = transformAffine * transformTarget;
}

void Draw2D::Pixel(const olc::vf2d& pos, const olc::Pixel col)
{
	PrepareTargetForSW();
	pTarget->Data(transformTarget.forward(((pos + 1.0f) * 0.5f) * olc::vf2d(pTarget->Size()))) = col;
}

GPUTask olc::Draw2D::TaskDrawPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.bWireframe = true;
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
	task.bWireframe = true;
	for (const auto& v : vPoints)
		task.vertexBuffer.push_back({ v.x, v.y, 1.0f, 1.0f, colour, 0, 0, 0,0, 0, 0, 0, 0 });
	task.tint = tint;
	return task;
}

GPUTask olc::Draw2D::TaskFillPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	return GPUTask();
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
	return task;
}

GPUTask Draw2D::Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Line,
			transformCombined.transform<float>({ p1, p2 }),
			col,
			olc::Colour::WHITE
		));
}

GPUTask Draw2D::Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel c1, const olc::Pixel c2)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Line,
			transformCombined.transform<float>({ p1, p2 }),
			{ c1, c2 },
			olc::Colour::WHITE
		));
}

GPUTask olc::Draw2D::Rect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Fan,
			transformCombined.transform<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			col,
			olc::Colour::WHITE
		));
}


GPUTask olc::Draw2D::FillRect(const olc::vf2d& pos, const olc::vf2d& size, const olc::Pixel col)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskFillPolygon(
			GPUTask::Structure::Fan,
			transformCombined.transform<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			col,
			olc::Colour::WHITE
		));
}

GPUTask olc::Draw2D::Image(olc::Image& image, const olc::vf2d& pos, const olc::vf2d& size)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskTexturedPolygon(
			GPUTask::Structure::Fan,
			transformCombined.transform<float>({ { pos.x, pos.y }, { pos.x + size.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { pos.x, pos.y + size.y } }),
			{ olc::Colour::WHITE, olc::Colour::WHITE, olc::Colour::WHITE, olc::Colour::WHITE },
			{ {0,0}, {1,0}, {1,1}, {0,1} },
			&image
		));
}


//! END IMPLEMENTATION