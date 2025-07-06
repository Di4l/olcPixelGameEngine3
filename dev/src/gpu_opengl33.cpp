#include "gpu_opengl33.h"

//! START IMPLEMENTATION
namespace olc::gpu
{
	bool Renderer_OGL33::CreateDevice(std::vector<void*> params, const RendererConfig& cfg)
	{
		// Create OpenGL Device Context
#if OLC_HOST == OLC_HOST_WINDOWS
		glDeviceContext = GetDC((HWND)(params[0]));

		PIXELFORMATDESCRIPTOR pfd =
		{
			sizeof(PIXELFORMATDESCRIPTOR), 1,
			PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
			PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			PFD_MAIN_PLANE, 0, 0, 0, 0
		};

		int pf = 0;
		if (!(pf = ChoosePixelFormat(glDeviceContext, &pfd)))
		{
			lastError = RendererError::InvalidDCPixelFormat;
			return false;
		}

		if (!SetPixelFormat(glDeviceContext, pf, &pfd))
		{
			lastError = RendererError::FailedToSetDCPixelFormat;
			return false;
		}

		if (!(glRenderContext = wglCreateContext(glDeviceContext))) 
		{
			lastError = RendererError::FailedToCreateRenderContext;
			return false;
		}

		if (!wglMakeCurrent(glDeviceContext, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}

		//// Set Vertical Sync
		//glSwapInterval = OGL_LOAD(glSwapInterval);
		//if (locSwapInterval && !bVSYNC) locSwapInterval(0);
		//bSync = bVSYNC;

#endif

		// Can't load OpenGL API until context is loaded
		auto& gl = olc::apis::opengl::gl::Get();


		lastError = RendererError::None;
		return true;
	}

	bool Renderer_OGL33::DestroyDevice()
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	uint32_t Renderer_OGL33::CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return 0;
	}

	bool Renderer_OGL33::WriteTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	bool Renderer_OGL33::ReadTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	bool Renderer_OGL33::DeleteTexture(const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	bool Renderer_OGL33::AssignTextureSource(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	bool Renderer_OGL33::AssignTextureTarget(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}
}
//! END IMPLEMENTATION