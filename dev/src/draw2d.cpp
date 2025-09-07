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
	pTarget = &image;
}

void Draw2D::ClearTransform()
{
	transform = olc::tf2d();
}

void Draw2D::SetTransform(const olc::tf2d& trans)
{
	transform = trans;
}

const tf2d& olc::Draw2D::GetTransform()
{
	return transform;
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

void Draw2D::Pixel(const olc::vf2d& pos, const olc::Pixel col)
{
	PrepareTargetForSW();
	pTarget->Data(transform.forward(pos)) = col;
}

GPUTask olc::Draw2D::TaskDrawPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	return GPUTask();
}

GPUTask olc::Draw2D::TaskDrawPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const olc::Pixel colour, const olc::Pixel tint)
{	
	GPUTask task;
	task.bWireframe = true;
	for (const auto& v : vPoints)
		task.vertexBuffer.push_back({ v.x, v.y, 1.0f, 1.0f, 0.0f, 0.0f, colour });
	task.tint = tint;
	return task;
}

GPUTask olc::Draw2D::TaskFillPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	return GPUTask();
}

GPUTask olc::Draw2D::TaskFillPolygon(GPUTask::Structure structure, const std::vector<olc::vf2d>& vPoints, const olc::Pixel colour, const olc::Pixel tint)
{
	return GPUTask();
}

GPUTask olc::Draw2D::TaskTexturedPolygon(GPUTask::Structure structure, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	return GPUTask();
}

GPUTask Draw2D::Line(const olc::vf2d& p1, const olc::vf2d& p2, const olc::Pixel col)
{
	PrepareTargetForHW();
	return vecGPUTasks.emplace_back(
		TaskDrawPolygon(
			GPUTask::Structure::Line,
			transform.forward<float>({ p1, p2 }),
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
			transform.forward<float>({ p1, p2 }),
			{ c1, c2 },
			olc::Colour::WHITE
		));
}


//! END IMPLEMENTATION