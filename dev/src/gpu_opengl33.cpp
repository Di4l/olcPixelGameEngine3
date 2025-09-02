#include "gpu_opengl33.h"

//! START IMPLEMENTATION
namespace olc::gpu
{
	bool Renderer_OGL33::CreateDevice(std::vector<void*> params, const RendererConfig& cfg)
	{
		config = cfg;


		// Create OpenGL Device Context
#if OLC_HOST == OLC_HOST_WINDOWS
		// "wgl*" all live in WinGDI
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

#if OLC_HOST == OLC_HOST_WINDOWS
		wglDeleteContext(glRenderContext);
#endif
		return false;
	}

	uint32_t Renderer_OGL33::CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// Curiously OpenGL doesnt actually care about the size of the texture
		// as part of its creation.	This matters later when we Write to texture
		// resources on GPU
		
		uint32_t id = 0;
		gl.glGenTextures(1, &id);
		gl.glBindTexture(GL_TEXTURE_2D, id);

		if (cfg.Filtered)
		{
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		}

		if (cfg.Clamp)
		{
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		}
		else
		{
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			gl.glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		}

#if OLC_HOST != OLC_HOST_EMSCRIPTEN
		gl.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
		return id;
	}

	bool Renderer_OGL33::WriteTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size().x, image.Size().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
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
		gl.glDeleteTextures(1, &texid);
		return true;
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

	bool Renderer_OGL33::DoGPUTask(const olc::pgeguts::GPUTask& task)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	bool Renderer_OGL33::ClearViewport(const olc::Pixel col, bool bDepth, bool bStencil)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glClearColor(float(col.r) / 255.0f, float(col.g) / 255.0f, float(col.b) / 255.0f, float(col.a) / 255.0f);
		gl.glClear(GL_COLOR_BUFFER_BIT | (bDepth ? GL_DEPTH_BUFFER_BIT : 0) | (bStencil ? GL_STENCIL_BUFFER_BIT : 0));		
		return true;
	}

	bool Renderer_OGL33::SetViewport(const olc::vf2d& pos, const olc::vf2d& size)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glViewport(int(pos.x), int(pos.y), int(size.x), int(size.y));
		return false;
	}

	bool Renderer_OGL33::DisplayPrepare()
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
	}

	bool Renderer_OGL33::DisplayDraw(bool bVerticalSyncNow)
	{
		auto& gl = olc::apis::opengl::gl::Get();


#if OLC_HOST == OLC_HOST_WINDOWS
		SwapBuffers(glDeviceContext);		
#endif	

		return false;
	}
}
//! END IMPLEMENTATION