#include "draw3d.h"
#include "gpu_iface.h"

using namespace olc;

//! START IMPLEMENTATION
thread_local Draw3D::buffer<olc::vf4d> Draw3D::buffPoints;
thread_local Draw3D::buffer<olc::Pixel> Draw3D::buffColours;
thread_local Draw3D::buffer<olc::GPUTask> Draw3D::vecGPUTasks;

olc::Draw3D::Draw3D(olc::Draw2D& d2d) : draw2d(d2d)
{
	MatrixReset();
}

void olc::Draw3D::SetGPU(olc::gpu::Renderer* const renderer)
{
	draw2d.SetGPU(renderer);
}

void olc::Draw3D::ProcessGPUTasks()
{
	draw2d.ProcessGPUTasks();
}

void olc::Draw3D::SetTarget(olc::Image& image)
{
	draw2d.SetTarget(image);
}

olc::Image& olc::Draw3D::GetTarget()
{
	return draw2d.GetTarget();
}

olc::vi2d olc::Draw3D::GetTargetSize()
{
	return draw2d.GetTargetSize();
}

void olc::Draw3D::SetViewport(const olc::vi2d& pos, const olc::vi2d& size)
{
	draw2d.pRenderer->SetViewport(pos, size);
}

bool olc::Draw3D::SetShader(const olc::gpu::Shader& shader)
{
	return draw2d.SetShader(shader);
}

bool olc::Draw3D::ResetShader()
{
	return draw2d.ResetShader();
}

bool olc::Draw3D::SetShaderUniform(const std::string& name, const float value)
{
	return draw2d.SetShaderUniform(name, value);
}

bool olc::Draw3D::SetShaderUniform(const std::string& name, const olc::vf2d& value)
{
	return draw2d.SetShaderUniform(name, value);
}

bool olc::Draw3D::SetShaderUniform(const std::string& name, const olc::Pixel value)
{
	return draw2d.SetShaderUniform(name, value);
}

bool olc::Draw3D::SetShaderTexture(const uint32_t nSlot, olc::Image& image)
{
	return draw2d.SetShaderTexture(nSlot, image);
}

void olc::Draw3D::PrepareTargetForSW()
{
	draw2d.PrepareTargetForSW();
}

void olc::Draw3D::PrepareTargetForHW()
{
	draw2d.PrepareTargetForHW();
}

void olc::Draw3D::PrepareImageForSW(olc::Image& image)
{
	draw2d.PrepareImageForSW(image);
}

void olc::Draw3D::PrepareImageForHW(olc::Image& image)
{
	draw2d.PrepareImageForHW(image);
}





void olc::Draw3D::MatrixReset()
{
	matMVP.identity();
	matModel.identity();	
	matView.identity();
	matProjection.identity();
}

void olc::Draw3D::SetModelMatrix(const olc::mf4d& mat)
{
	matModel = mat;	
	matMVP = matVP * matModel;
}

const olc::mf4d& olc::Draw3D::GetModelMatrix() const
{
	return matModel;
}

void olc::Draw3D::SetViewMatrix(const olc::mf4d& mat)
{
	matView = mat;
	matVP =  matProjection * matView;
	matMVP = matVP * matModel;
}

const olc::mf4d& olc::Draw3D::GetViewMatrix() const
{
	return matView;
}

void olc::Draw3D::SetProjectionMatrix(const olc::mf4d& mat)
{
	matProjection = mat;
	matVP = matProjection * matView;
	matMVP = matVP * matModel;
}

const olc::mf4d& olc::Draw3D::GetProjectionMatrix() const
{
	return matProjection;
}

void olc::Draw3D::SetMVPMatrix(const olc::mf4d& mat)
{
	matMVP = mat;
}

const olc::mf4d& olc::Draw3D::GetMVPMatrix() const
{
	return matMVP;
}

void olc::Draw3D::SetCullMode(const olc::GPUTask::CullMode mode)
{
	cullMode = mode;
}

void olc::Draw3D::EnableDepth(const bool bEnable)
{
	bDepth = bEnable;
}



GPUTask olc::Draw3D::TaskWireMesh(olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.tint = tint;
	task.vertexBuffer.resize(vPoints.size());
	task.bWireframe = true;
	task.bDepth = bDepth;
	task.cullmode = cullMode;
	task.bIs3D = true;
	task.mvpMatrix = matMVP.m;

	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vPoints[i].z, vPoints[i].w}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	return task;
}

GPUTask olc::Draw3D::TaskFillMesh(olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.tint = tint;
	task.vertexBuffer.resize(vPoints.size());
	task.bDepth = bDepth;
	task.cullmode = cullMode;
	task.bIs3D = true;
	task.mvpMatrix = matMVP.m;

	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vPoints[i].z, vPoints[i].w}, vColours[i], {0, 0}, {0, 0}, {0, 0}, {0, 0} };
	return task;
}

GPUTask olc::Draw3D::TaskTexturedMesh(olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vTexCoords, olc::Image* const image, const olc::Pixel tint)
{
	GPUTask task;
	task.structure = structure;
	task.tint = tint;
	task.vertexBuffer.resize(vPoints.size());
	task.pImage = image;
	task.bIs3D = true;
	task.bDepth = bDepth;
	task.cullmode = cullMode;
	task.mvpMatrix = matMVP.m;
	for (size_t i = 0; i < vPoints.size(); i++)
		task.vertexBuffer[i] = { {vPoints[i].x, vPoints[i].y, vPoints[i].z, vPoints[i].w}, vColours[i], {vTexCoords[i].x, vTexCoords[i].y}, {0, 0}, {0, 0}, {0, 0} };
	return task;
}

void olc::Draw3D::Clear(const olc::Pixel& col)
{
	draw2d.Clear(col);
}

GPUTask& olc::Draw3D::Line(const olc::vf4d& vStart, const olc::vf4d& vEnd, const olc::Pixel& col, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return draw2d.vecGPUTasks.data.emplace_back(std::move(
		TaskWireMesh(
			olc::Structure::Line,
			{ vStart, vEnd },
			{ col, col },
			tint
		)));
}

GPUTask& olc::Draw3D::Mesh(const olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const olc::Pixel tint)
{
	PrepareTargetForHW();

	return draw2d.vecGPUTasks.data.emplace_back(std::move(
		TaskWireMesh(
			structure,
			vPoints,
			vColours,
			tint
		)));

}

GPUTask& olc::Draw3D::Mesh(const olc::Structure structure, const std::vector<olc::vf4d>& vPoints, const std::vector<olc::Pixel>& vColours, const std::vector<olc::vf2d>& vUVs, olc::Image& texture, const olc::Pixel tint)
{
	PrepareImageForHW(texture);
	PrepareTargetForHW();
	return draw2d.vecGPUTasks.data.emplace_back(std::move(
		TaskTexturedMesh(
			structure,
			vPoints,
			vColours,
			vUVs,
			&texture,
			tint
		)));
}






//! END IMPLEMENTATION

