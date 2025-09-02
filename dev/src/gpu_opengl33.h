#pragma once

#include "gpu_iface.h"
#include "api_opengl.h"

//! START DECLARATION
#if !defined(PGE_RENDERER_OPENGL33_DECLARED)
namespace olc
{
	namespace gpu
	{
		class Renderer_OGL33 : public olc::gpu::Renderer
		{
		public: // Device Stuff
			// Constructs a GPU Device interface
			bool CreateDevice(std::vector<void*> params, const RendererConfig& cfg) override;
			// Destroys a GPU device interface
			bool DestroyDevice() override;

		public: // Texture Resource Stuff
			// Allocates a new texture resource in VRAM, returns handle
			uint32_t CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg = olc::ImageConfig()) override;
			// Writes to / updates an existing texture resource in VRAM, using existing Image in SRAM
			bool WriteTexture(const uint32_t texid, olc::Image& image) override;
			// Writes to / updates an existing Image in SRAM, from existing texture resource in VRAM
			bool ReadTexture(const uint32_t texid, olc::Image& image) override;
			// Destroys and releases texture resource for given handle
			bool DeleteTexture(const uint32_t texid) override;
			// Makes active the given texture resource (for subsequent sampling operations)
			bool AssignTextureSource(const uint32_t slot, const uint32_t texid) override;
			// Makes active the given texture resource (for subsequent rendering operations)
			bool AssignTextureTarget(const uint32_t slot, const uint32_t texid) override;


		public: // GPU Task Stuff
			virtual bool DoGPUTask(const olc::pgeguts::GPUTask& task) override;

		public: // Swap Chain Stuff
			// Clears the viewport to a specific colour and depth
			virtual bool ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil) override;
			// Sets the viewport area of the drawing space
			virtual bool SetViewport(const olc::vf2d& pos, const olc::vf2d& size) override;
			// Configures defaults prior to drawing
			virtual bool DisplayPrepare() override;
			// Displays the final output
			virtual bool DisplayDraw(bool bVerticalSyncNow) override;

		
		protected: // These may need some thinking about re multiple window
			olc::apis::opengl::glDeviceContext_t glDeviceContext = 0;
			olc::apis::opengl::glRenderContext_t glRenderContext = 0;

		};
	}
}
#define PGE_RENDERER_OPENGL33_DECLARED 1
#endif
//! END DECLARATION

