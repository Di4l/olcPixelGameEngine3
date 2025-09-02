#pragma once

//! START STDHEADER GLOBAL
#include <cstdint>
#include <vector>
//! END STDHEADER

//! START CUSTOMHEADER
#include "config.h"
#include "pixel.h"
#include "vector2d.h"
#include "gputask.h"
#include "image.h"
//! END CUSTOMHEADER

//! START DECLARATION
#if !defined(PGE_RENDERER_IFACE_DECLARED)
namespace olc
{
	namespace gpu
	{
		struct RendererConfig
		{
			bool FullScreen = false;
			bool VerticalSync = false;
		};

		enum class RendererError
		{
			None,
			InvalidDCPixelFormat,
			FailedToSetDCPixelFormat,
			FailedToCreateRenderContext,
			FailedToSwitchRenderContext,
		};

		class Renderer
		{
		public:
			Renderer() = default;
			virtual ~Renderer() {};

		public:
			// Check/Get last error
			inline RendererError GetLastError() const { return lastError; }
			inline const RendererConfig& GetConfig() const { return config; }

		public: // Device Stuff
			// Constructs a GPU Device interface
			virtual bool CreateDevice(std::vector<void*> params, const RendererConfig& cfg) = 0;
			// Destroys a GPU device interface
			virtual bool DestroyDevice() = 0;

		public: // Texture Resource Stuff
			// Allocates a new texture resource in VRAM, returns handle
			virtual uint32_t CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg = olc::ImageConfig()) = 0;
			// Writes to / updates an existing texture resource in VRAM, using existing Image in SRAM
			virtual bool WriteTexture(const uint32_t texid, olc::Image& image) = 0;
			// Writes to / updates an existing Image in SRAM, from existing texture resource in VRAM
			virtual bool ReadTexture(const uint32_t texid, olc::Image& image) = 0;
			// Destroys and releases texture resource for given handle
			virtual bool DeleteTexture(const uint32_t texid) = 0;
			// Makes active the given texture resource (for subsequent sampling operations)
			virtual bool AssignTextureSource(const uint32_t slot, const uint32_t texid) = 0;
			// Makes active the given texture resource (for subsequent rendering operations)
			virtual bool AssignTextureTarget(const uint32_t slot, const uint32_t texid) = 0;

		public: // Shader Construction Stuff

		public: // GPU Task Processing Stuff
			virtual bool DoGPUTask(const olc::pgeguts::GPUTask& task) = 0;

		public: // Swap Chain Stuff
			// Clears the viewport to a specific colour and depth
			virtual bool ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil) = 0;
			// Sets the viewport area of the drawing space
			virtual bool SetViewport(const olc::vf2d& pos, const olc::vf2d& size) = 0;
			// Configures defaults prior to drawing
			virtual bool DisplayPrepare() = 0;
			// Displays the final output
			virtual bool DisplayDraw(bool bVerticalSyncNow = false) = 0;


		protected:
			RendererConfig config;
			RendererError lastError = RendererError::None;
		};
	}
}
#define PGE_RENDERER_IFACE_DECLARED 1
#endif
//! END DECLARATION