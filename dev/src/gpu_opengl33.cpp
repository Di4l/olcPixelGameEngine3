#include "gpu_opengl33.h"

//! START IMPLEMENTATION
namespace olc::gpu
{
	std::string Shader_GLSL33::Compile()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		nCompiledShaderID = gl.glCreateProgram();

		// Fragment Shader
		if (!srcPixelShader.empty())
		{
			nPixelShaderID = gl.glCreateShader(gl.GL_FRAGMENT_SHADER_X);
			const char* s = srcPixelShader.c_str();
			gl.glShaderSource(nPixelShaderID, 1, &s, nullptr);
			gl.glCompileShader(nPixelShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nPixelShaderID);
		}

		// Vertex Shader
		if (!srcVertexShader.empty())
		{
			nVertexShaderID = gl.glCreateShader(gl.GL_VERTEX_SHADER_X);
			const char* s = srcVertexShader.c_str();
			gl.glShaderSource(nVertexShaderID, 1, &s, nullptr);
			gl.glCompileShader(nVertexShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nVertexShaderID);
		}


		// Geometry Shader
		if (!srcGeometryShader.empty())
		{
			nGeometryShaderID = gl.glCreateShader(gl.GL_GEOMETRY_SHADER_X);
			const char* s = srcGeometryShader.c_str();
			gl.glShaderSource(nGeometryShaderID, 1, &s, nullptr);
			gl.glCompileShader(nGeometryShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nGeometryShaderID);
		}

		gl.glLinkProgram(nCompiledShaderID);

		return "OK";
	}

	int32_t Shader_GLSL33::CreateUniform(const std::string& name)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		const char* s = name.c_str();
		mapUniforms.insert({ name, gl.glGetUniformLocation(nCompiledShaderID, s) });
		return GetUniform(name);
	}



	bool Renderer_OGL33::CreateDevice(std::vector<void*> os_win_id, const RendererConfig& cfg)
	{
		config = cfg;


		
#if OLC_HOST == OLC_HOST_WINDOWS
		// Create OpenGL Device Context
		if (!PrepareWindowTarget(os_win_id))
		{
			return false;
		}

		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));

		// Create OpenGL Render Context
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

#if OLC_HOST == OLC_HOST_LINUX_X11
		const auto window_handle = reinterpret_cast<X11::Window>(os_win_id[0]);
		auto* display = reinterpret_cast<X11::Display*>(os_win_id[1]);
        GLint olc_GLAttribs[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

		X11::XVisualInfo* olc_VisualInfo = X11::glXChooseVisual(display, 0, olc_GLAttribs);
		glRenderContext = X11::glXCreateContext(display, olc_VisualInfo, nullptr, GL_TRUE);
		glXMakeCurrent(display, window_handle, glRenderContext);

		X11::XWindowAttributes gwa;
		X11::XGetWindowAttributes(display, window_handle, &gwa);
		glViewport(0, 0, gwa.width, gwa.height);
#endif

#if OLC_HOST == OLC_HOST_MACOS
        
		// os_win_id[0] is the OLC OpenGL Device Context      
        glRenderContext = (olc::apis::opengl::glRenderContext_t)os_win_id[0];
        if (!glRenderContext) {
            lastError = RendererError::FailedToCreateRenderContext;
            return false;
        }

#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
	const auto canvasId = reinterpret_cast<std::string*>(os_win_id[0]);

	EGLint const attribute_list[] = { EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_ALPHA_SIZE, 8, EGL_DEPTH_SIZE, 16, EGL_NONE };
	EGLint const context_config[] = { EGL_CONTEXT_CLIENT_VERSION , 2, EGL_NONE };
	EGLint num_config;

	glRenderContext.display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(glRenderContext.display, nullptr, nullptr);
	eglChooseConfig(glRenderContext.display, attribute_list, &glRenderContext.config, 1, &num_config);
	
	/* create an EGL rendering context */
	glRenderContext.context = eglCreateContext(glRenderContext.display, glRenderContext.config, EGL_NO_CONTEXT, context_config);
	glRenderContext.surface = eglCreateWindowSurface(glRenderContext.display, glRenderContext.config, NULL, nullptr);
	if(!eglMakeCurrent(glRenderContext.display, glRenderContext.surface, glRenderContext.surface, glRenderContext.context))
	{
		lastError = RendererError::FailedToCreateRenderContext;
		return false;
	}
#endif

		// Can't load OpenGL API until context is loaded
		auto& gl = olc::apis::opengl::gl::Get();
		if (!gl.HasLoaded())
		{
			std::cout << "Error: Could not Load OpenGL!\n";
			lastError = RendererError::NoError;
			return false;
		}
		

		// Create "Default" Shader
#if OLC_HOST != OLC_HOST_EMSCRIPTEN
		shaderDefault.SetPixelShaderSource(R"(
			#version 330 core
			layout(location = 0) out vec4 pixel;
			in vec2 oTex;
			in vec4 oCol;
			uniform sampler2D sprTex;

			void main()
			{
				// Was just this
				//pixel = texture(sprTex, oTex) * oCol;

				// But to premultiply alpha correctly, we now do this:	
				vec4 texColor = texture(sprTex, oTex) * oCol;
				pixel = vec4(texColor.rgb * texColor.a, texColor.a);
			}
		)");

		shaderDefault.SetVertexShaderSource(R"(
			#version 330 core
			layout(location = 0) in vec4 aPos;
			layout(location = 1) in vec4 aCol;
			layout(location = 2) in vec2 aTex;
			uniform mat4 mvp;
			uniform int drawtype;
			uniform vec4 tint;
			uniform vec2 target;
			uniform vec2 invtarget;
			out vec2 oTex;
			out vec4 oCol;

			void main()
			{ 																																				  
				if(drawtype == 2) // 3D																																  
				{																																			  
					gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0); 																					  
					oTex = aTex;																															  
				} 				 
			
				else if(drawtype == 1) // 2D Line																																		  
				{																																			  
					float p = 1.0 / aPos.z; 																												  
					gl_Position = p * vec4(vec2(2.0 * (floor(aPos.xy) + 0.5) * invtarget - 1.0), 0.0, 1.0);	  
					oTex = aTex;																										  
				} 			  
			
				else if(drawtype == 0) // 2D Polygon																																		  
				{																																			  
					float p = 1.0 / aPos.z; 																												  
					gl_Position = p * vec4(vec2(2.0 * (aPos.xy + 0.25) * invtarget - 1.0), 0.0, 1.0);	 
					oTex = p * vec2(aTex.x, aTex.y);																										  
				} 
				
				else  // Balanced default
				{
					gl_Position = aPos;
					oTex = aTex;
				} 																																			  
																																			  
				oCol = aCol * tint;																															  
			}
		)");
#else
		shaderDefault.SetPixelShaderSource(R"(#version 300 es
			precision mediump float;
			layout(location = 0) out vec4 pixel;
			in vec2 oTex;
			in vec4 oCol;
			uniform sampler2D sprTex;

			void main()
			{
				// Was just this
				//pixel = texture(sprTex, oTex) * oCol;

				// But to premultiply alpha correctly, we now do this:
				vec4 texColor = texture(sprTex, oTex) * oCol;
				pixel = vec4(texColor.rgb * texColor.a, texColor.a);
			}
		)");

		shaderDefault.SetVertexShaderSource(R"(#version 300 es
			precision mediump float;
			layout(location = 0) in vec4 aPos;
			layout(location = 1) in vec4 aCol;
			layout(location = 2) in vec2 aTex;
			uniform mat4 mvp;
			uniform int drawtype;
			uniform vec4 tint;
			uniform vec2 target;
			uniform vec2 invtarget;
			out vec2 oTex;
			out vec4 oCol;

			void main()
			{ 																																				  
				if(drawtype == 2) // 3D																																  
				{																																			  
					gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0); 																					  
					oTex = aTex;																															  
				} 				 
			
				else if(drawtype == 1) // 2D Line																																		  
				{																																			  
					float p = 1.0 / aPos.z; 																												  
					gl_Position = p * vec4(vec2(2.0 * (floor(aPos.xy) + 0.5) * invtarget - 1.0), 0.0, 1.0);	  
					oTex = aTex;																										  
				} 			  
			
				else if(drawtype == 0) // 2D Polygon																																		  
				{																																			  
					float p = 1.0 / aPos.z; 																												  
					gl_Position = p * vec4(vec2(2.0 * (aPos.xy + 0.25) * invtarget - 1.0), 0.0, 1.0);	 
					oTex = p * vec2(aTex.x, aTex.y);																										  
				} 
				
				else  // Balanced default
				{
					gl_Position = aPos;
					oTex = aTex;
				} 																																			  
																																			  
				oCol = aCol * tint;																															  
			}
		)");
#endif

		shaderDefault.Compile();
		shaderDefault.CreateUniform("mvp");
		shaderDefault.CreateUniform("drawtype");
		shaderDefault.CreateUniform("tint");
		shaderDefault.CreateUniform("target");
		shaderDefault.CreateUniform("invtarget");

		// Create "Default" Vertex Buffer / Vertex Attributes. This buffer is reused
		// for all drawing operations. It's possible future versions may allow the
		// creation of additional named buffers for repeated drawing operations with
		// minimal overhead.
		gl.glGenBuffers(1, &nDefaultVB);
		gl.glGenVertexArrays(1, &nDefaultVA);
		gl.glBindVertexArray(nDefaultVA);
		gl.glBindBuffer(gl.GL_ARRAY_BUFFER_X, nDefaultVB);

		// A big one is allocated to reduce shuffles in GPU memory
		GPUTask::Vertex verts[OLC_GPU_MAX_VERTICES];
		gl.glBufferData(gl.GL_ARRAY_BUFFER_X, sizeof(GPUTask::Vertex) * OLC_GPU_MAX_VERTICES, verts, gl.GL_STREAM_DRAW_X);
		
		// Float Index 0 = x, 1 = y, 2 = z, 3 = w
		gl.glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex),        (void*)(0 * sizeof(float)));
		gl.glEnableVertexAttribArray(0);		
		// Float Index 4 = (RGBA 8-bit x4)
		gl.glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GPUTask::Vertex), (void*)(4 * sizeof(float)));	
		gl.glEnableVertexAttribArray(1);
		// Float Index 5 = u0, 6 = v0
		gl.glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex),        (void*)(5 * sizeof(float)));
		gl.glEnableVertexAttribArray(2);
		// Float Index 7 = u1, 8 = v1
		gl.glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex), (void*)(7 * sizeof(float)));
		gl.glEnableVertexAttribArray(3);
		// Float Index 9 = u2, 10 = v2
		gl.glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex), (void*)(9 * sizeof(float)));
		gl.glEnableVertexAttribArray(4);
		// Float Index 11 = u3, 12 = v4
		gl.glVertexAttribPointer(5, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex), (void*)(11 * sizeof(float)));
		gl.glEnableVertexAttribArray(5);

		// Buffers are configured, unbind for now
		gl.glBindBuffer(gl.GL_ARRAY_BUFFER_X, 0);
		gl.glBindVertexArray(0);


		


		// Create a null-texture so sampler doesnt fail. We don't have some of the core's helper
		// functions here, so we construct it manually
		imgBlank.Create({ 1,1 });
		imgBlank.SetGPUID(CreateTexture(imgBlank.Size()));
		imgBlank.BindCPU();
		imgBlank.Pixel({ 0,0 }) = olc::Colour::WHITE;
		imgBlank.BindGPU();
		WriteTexture(imgBlank.GetGPUID(), imgBlank);

		// Create a Frame Buffer Object for off-screen rendering things
		gl.glGenFramebuffers(1, (GLuint*)&nDefaultFBO);
		gl.glBindFramebuffer(36160U, nDefaultFBO); // GL_FRAMEBUFFER
		// Attach 4 colour buffers
		std::array<GLenum, 4> attachments = { {36064U, 36065U, 36066U, 36067U} };
		gl.glDrawBuffers(4, attachments.data());
		// Unlink them from any existing image textures
		//gl.glFramebufferTexture2D(36160U, attachments[0], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(36160U, attachments[1], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(36160U, attachments[2], GL_TEXTURE_2D, 0, 0);
		//gl.glFramebufferTexture2D(36160U, attachments[3], GL_TEXTURE_2D, 0, 0);
		// Unbind the FBO
		gl.glBindFramebuffer(36160U, 0);


		// Create FBOs for MSAA resolve operations
		gl.glGenFramebuffers(1, &nResolveFBO_Draw);
		gl.glGenFramebuffers(1, &nResolveFBO_Read);

#if OLC_HOST != OLC_HOST_EMSCRIPTEN
		gl.glEnable(GL_TEXTURE_2D); // Turn on texturing
		gl.glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
#endif
		gl.glEnable(GL_BLEND);

		lastError = RendererError::NoError;
		return true;
	}

	bool Renderer_OGL33::DestroyDevice()
	{
		auto& gl = olc::apis::opengl::gl::Get();

#if OLC_HOST == OLC_HOST_WINDOWS
		wglDeleteContext(glRenderContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS
        //TODO: Add MacOS destroy context code
#endif
#if OLC_HOST == OLC_HOST_LINUX_X11
		auto* display = X11::XOpenDisplay(nullptr);
		X11::glXMakeCurrent(display, 0, NULL);
		X11::glXDestroyContext(display, glRenderContext);
#endif
#if OLC_HOST == OLC_HOST_EMSCRIPTEN
		eglMakeCurrent(glRenderContext.display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
		eglDestroyContext(glRenderContext.display, glRenderContext.context);
		eglDestroySurface(glRenderContext.display, glRenderContext.surface);
		eglTerminate(glRenderContext.display);
		glRenderContext.display = EGL_NO_DISPLAY;
		glRenderContext.surface = EGL_NO_SURFACE;
		glRenderContext.context = EGL_NO_CONTEXT;
#endif
		return false;
	}

	bool Renderer_OGL33::RetargetDevice(std::vector<void*> os_win_id)
	{
#if OLC_HOST == OLC_HOST_WINDOWS
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));

		if (!wglMakeCurrent(glDeviceContext, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			auto err = ::GetLastError();
			return false;
		}
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS

		CGLContextObj cglContext = (CGLContextObj)glRenderContext;
		if (!CGLSetCurrentContext(cglContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}
#endif
#if OLC_HOST == OLC_HOST_LINUX_X11
		const auto window = reinterpret_cast<X11::Window>(os_win_id[0]);
		auto* display = reinterpret_cast<X11::Display*>(os_win_id[1]);
		if(!X11::glXMakeCurrent(display, window, glRenderContext))
		{
			lastError = RendererError::FailedToSwitchRenderContext;
			return false;
		}
#endif
#if OLC_HOST == OLC_HOST_EMSCRIPTEN
	if(!eglMakeCurrent(glRenderContext.display, glRenderContext.surface, glRenderContext.surface, glRenderContext.context))
	{
		lastError = RendererError::FailedToSwitchRenderContext;
		return false;
	}
#endif		
		return true;
	}

	bool Renderer_OGL33::PrepareWindowTarget(std::vector<void*> os_win_id)
	{
#if OLC_HOST == OLC_HOST_WINDOWS
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));

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
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif
#if OLC_HOST == OLC_HOST_MACOS
        
		// params[0] is the OLC OpenGL Device Context      
        glRenderContext = (olc::apis::opengl::glRenderContext_t)os_win_id[0];
        if (!glRenderContext) {
            lastError = RendererError::FailedToCreateRenderContext;
            return false;
        }

#endif
		return true;
	}

	uint32_t Renderer_OGL33::CreateTexture(const olc::vi2d& vSize, const olc::ImageConfig& cfg)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// Curiously OpenGL doesnt actually care about the size of the texture
		// as part of its creation.	This matters later when we Write to texture
		// resources on GPU
		
		uint32_t id = 0;

		// Helper function to create regular (non-MSAA) texture
		auto CreateRegularTexture = [&]()
		{
			uint32_t new_id = 0;
			gl.glGenTextures(1, &new_id);
			glBindTexture(GL_TEXTURE_2D, new_id);

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

			return new_id;
		};

		// Regular textures and MSAA textures are handled differently
		if (!cfg.MSAA)
		{
			id = CreateRegularTexture();
		}
		else
		{
			// Texture is MSAA, so we need to create a special
			// "resolved" texture later for sampling when the 
			// MSAA texture is used as a source
			gl.glGenTextures(1, &id);
			glBindTexture(gl.GL_TEXTURE_2D_MULTISAMPLE_X, id);

			// Allocate MSAA texture storage
			uint32_t regular_id = CreateRegularTexture();
			mapMSAAToResolved[id] = regular_id;

			// Note this "bonus" texture is quite hidden
			// from the user.
		}

		
#if OLC_HOST != OLC_HOST_EMSCRIPTEN
#if OLC_HOST != OLC_HOST_MACOS		
		gl.glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
#endif
#endif

		// We need to store the size in case we
		// need it later when resolving MSAA textures
		mapTextureSizes[id] = vSize;
		return id;
	}

	bool Renderer_OGL33::WriteTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		if (mapMSAAToResolved.contains(texid))
		{
			// Texture is MSAA
			gl.glBindTexture(gl.GL_TEXTURE_2D_MULTISAMPLE_X, texid);

			// Allocate MSAA texture storage
			gl.glTexImage2DMultisample(gl.GL_TEXTURE_2D_MULTISAMPLE_X, image.GetConfig().MSAASamples,
				GL_RGBA, image.Size().x, image.Size().y, GL_TRUE);

			// Also allocate the resolve texture - we dont care
			// about the contents as it will be overwritten on
			// an MSAA resolve
			uint32_t resolvedId = mapMSAAToResolved[texid];
			gl.glBindTexture(GL_TEXTURE_2D, resolvedId);

			// Initialize resolve with transparent black
			std::vector<uint8_t> a(image.Size().area() * sizeof(olc::Pixel), 0);
			gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size().x, image.Size().y, 0, 
				GL_RGBA, GL_UNSIGNED_BYTE, a.data());

		}
		else
		{
			// Texture is regular
			gl.glBindTexture(GL_TEXTURE_2D, texid);
			gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size().x, image.Size().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());

		}
	

		// Update in case texture was resized
		mapTextureSizes[texid] = image.Size();

		return true;
	}

	bool Renderer_OGL33::ReadTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		// Read the teture data back into the image
		gl.glBindTexture(GL_TEXTURE_2D, image.GetGPUID());

#if OLC_HOST != OLC_HOST_EMSCRIPTEN
		gl.glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
		// Note: For MSAA textures, this reads the resolved texture, which
		// is probably what you want anyway
#else		
		gl.glReadPixels(0, 0, image.Size().x, image.Size().y, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
#endif
		return true;
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

		// This function binds a texture to a texture slot for sampling. If the
		// texture is an MSAA texture, we need to use the resolved version for sampling

		// Check if this is an MSAA texture...
		uint32_t actualTexId = texid;
		if (mapMSAAToResolved.contains(texid))
		{
			// ...yes it is, so use the resolved texture for sampling
			actualTexId = mapMSAAToResolved[texid];
		}

		// If the requested source texture is currently attached as the render target,
		// unbind the framebuffer to avoid sampling from a texture that's being written to.
		if (actualTexId == nCurrentTextureTarget && actualTexId != 0)
		{
#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning ATS: Requested source is currently attached as target (" << actualTexId << ") - unbinding FBO\n";
#endif
			gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, 0);
			nCurrentTextureTarget = 0;
		}

		//if (nCurrentTextureSource == texid)
		//	return true;

		// Bind texture to specified texture slot
		gl.glActiveTexture(gl.GL_TEXTURE0_X + slot);
		gl.glBindTexture(GL_TEXTURE_2D, actualTexId);

		// Record currently bound source texture
		nCurrentTextureSource = actualTexId;
		return true;
	}

	// Assign a texture as the current render target. 
	// If texid is 0, bind the default framebuffer (screen)
	bool Renderer_OGL33::AssignTextureTarget(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		// This function attaches a texture to the FBO for rendering.

		// If the requested target texture is currently bound as a source, unbind it
		// from all texture units to ensure we do not sample from a texture that's
		// attached to the FBO (undefined behavior).
		if (texid != 0 && texid == nCurrentTextureSource)
		{
#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
			std::cout << "Warning ATT: Requested target is currently bound as source (" << texid << ") - unbinding texture units\n";
#endif
			// Unbind from a reasonable number of texture units (0..7) used by this renderer
			for (int i = 0; i < 8; ++i)
			{
				gl.glActiveTexture(gl.GL_TEXTURE0_X + i);
				gl.glBindTexture(GL_TEXTURE_2D, 0);
			}

			// Reset to texture unit 0
			gl.glActiveTexture(gl.GL_TEXTURE0_X);
			nCurrentTextureSource = 0;
		}
		
		if (texid == 0)
		{
			// Unbind the FBO (bind default framebuffer)
			gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, 0);
			glDisable(gl.GL_MULTISAMPLE_X);
			return true;
		}	
		
		// Bind FBO
		gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nDefaultFBO);

		// Allocate target buffers - pick the single attachment corresponding to 'slot'
		std::array<GLenum, 8> attachments =
		{ { 
			gl.GL_COLOR_ATTACHMENT0_X + 0,
			gl.GL_COLOR_ATTACHMENT0_X + 1,
			gl.GL_COLOR_ATTACHMENT0_X + 2,
			gl.GL_COLOR_ATTACHMENT0_X + 3,
			gl.GL_COLOR_ATTACHMENT0_X + 4,
			gl.GL_COLOR_ATTACHMENT0_X + 5,
			gl.GL_COLOR_ATTACHMENT0_X + 6,
			gl.GL_COLOR_ATTACHMENT0_X + 7
		} };
		GLenum draw = attachments[slot];
		
		// Set the draw buffer to the selected attachment
		gl.glDrawBuffers(1, &draw);
		
		// If target texture is MSAA, enable multisampling
		if (mapMSAAToResolved.contains(texid))
		{
			glEnable(gl.GL_MULTISAMPLE_X);
			// Attach MSAA texture to FBO
			gl.glFramebufferTexture2D(gl.GL_FRAMEBUFFER_X, gl.GL_COLOR_ATTACHMENT0_X + slot, gl.GL_TEXTURE_2D_MULTISAMPLE_X, texid, 0);
		}
		else
		{
			glDisable(gl.GL_MULTISAMPLE_X);
			// Attach regular texture to FBO
			gl.glFramebufferTexture2D(gl.GL_FRAMEBUFFER_X, gl.GL_COLOR_ATTACHMENT0_X + slot, GL_TEXTURE_2D, texid, 0);

		}

		// Record currently bound target texture
		nCurrentTextureTarget = texid;		
		return true;
	}

	bool Renderer_OGL33::ResolveMSAA(const uint32_t msaaTexId)
	{
		if (!mapMSAAToResolved.contains(msaaTexId))
			return true;  // Not an error, just not MSAA

		auto& gl = olc::apis::opengl::gl::Get();

		// Get resolved texture ID. This is linked at 
		// texture creation time for MSAA textures
		uint32_t resolvedId = mapMSAAToResolved[msaaTexId];

		// Ensure all rendering to MSAA texture is finished
		glFinish();

		// Bind MSAA texture to read FBO
		gl.glBindFramebuffer(gl.GL_READ_FRAMEBUFFER_X, nResolveFBO_Read);
		gl.glFramebufferTexture2D(gl.GL_READ_FRAMEBUFFER_X, gl.GL_COLOR_ATTACHMENT0_X, gl.GL_TEXTURE_2D_MULTISAMPLE_X, msaaTexId, 0);

#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
		// Check read framebuffer status
		GLenum readStatus = gl.glCheckFramebufferStatus(gl.GL_READ_FRAMEBUFFER);
		if (readStatus != gl.GL_FRAMEBUFFER_COMPLETE)
		{
			std::cout << "ResolveMSAA ERROR: Read framebuffer incomplete!\n";
			return false;
		}
#endif

		// Bind resolved texture to draw FBO
		gl.glBindFramebuffer(gl.GL_DRAW_FRAMEBUFFER_X, nResolveFBO_Draw);
		gl.glFramebufferTexture2D(gl.GL_DRAW_FRAMEBUFFER_X, gl.GL_COLOR_ATTACHMENT0_X, GL_TEXTURE_2D, resolvedId, 0);

#if defined(OLC_GPU_ERRORCHECK) && OLC_GPU_ERRORCHECK == 1
		// Check draw framebuffer status
		GLenum drawStatus = gl.glCheckFramebufferStatus(gl.GL_DRAW_FRAMEBUFFER_X);
		if (drawStatus != gl.GL_FRAMEBUFFER_COMPLETE_X)
		{
			std::cout << "ResolveMSAA ERROR: Draw framebuffer incomplete!";
			return false;
		}
#endif

		// Blit from MSAA to resolved
		olc::vi2d size = mapTextureSizes[msaaTexId];
		gl.glBlitFramebuffer(
			0, 0, size.x, size.y,
			0, 0, size.x, size.y,
			GL_COLOR_BUFFER_BIT,
			GL_NEAREST
		);

		// Restore to default framebuffer (screen)
		gl.glBindFramebuffer(gl.GL_FRAMEBUFFER_X, nDefaultFBO);
		return true;
	}

	bool Renderer_OGL33::ApplyShader(const Shader& shader)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glUseProgram(shader.GetShaderID());
		return true;
	}

	bool Renderer_OGL33::ApplyDefaultShader()
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glUseProgram(shaderDefault.GetShaderID());
		return true;
	}

	bool Renderer_OGL33::DoGPUTask(const olc::GPUTask& task)
	{
		auto& gl = olc::apis::opengl::gl::Get();

		switch (task.task)
		{
			case GPUTask::Task::NullTask:
			break;
			case GPUTask::Task::DrawPolygon:
			{
				
				if (task.pImage == nullptr)
					AssignTextureSource(0, imgBlank.GetGPUID());
				else
				{
					if (nCurrentTextureSource != task.pImage->GetGPUID())
						AssignTextureSource(0, task.pImage->GetGPUID());
				}

				// Bind generic vertex buffer
				gl.glBindVertexArray(nDefaultVA);
				gl.glBindBuffer(gl.GL_ARRAY_BUFFER_X, nDefaultVB);
				
				// Copy data from CPU to GPU
				gl.glBufferData(gl.GL_ARRAY_BUFFER_X, sizeof(GPUTask::Vertex) * task.vertexBuffer.size(), task.vertexBuffer.data(), gl.GL_STREAM_DRAW_X);
				
				

				// Shader: Apply MVP Matrix
				//gl.glUniformMatrix4fv(shaderDefault.GetUniform("mvp"), 1, true, task.mvpMatrix.data());

				// Shader: Apply Global Tint
				float f[4] = { 
					float(task.tint.r) / 255.0f, 
					float(task.tint.g) / 255.0f, 
					float(task.tint.b) / 255.0f, 
					float(task.tint.a) / 255.0f 
				};
				gl.glUniform4fv(shaderDefault.GetUniform("tint"), 1, f);

				f[0] = 64.0f;
				f[1] = 64.0f;
				gl.glUniform2fv(shaderDefault.GetUniform("target"), 1, vTargetSize.a().data());
				gl.glUniform2fv(shaderDefault.GetUniform("invtarget"), 1, ((1.0f / vTargetSize)).a().data());

				// Apply Culling modes
				//if (task.cullmode == GPUTask::CullMode::None)
				//{
				//	gl.glCullFace(GL_FRONT);
				//	gl.glDisable(GL_CULL_FACE);
				//}
				//else if (task.cullmode == GPUTask::CullMode::ClockWise)
				//{
				//	gl.glCullFace(GL_FRONT);
				//	gl.glEnable(GL_CULL_FACE);
				//}
				//else if (task.cullmode == GPUTask::CullMode::CounterClockWise)
				//{
				//	gl.glCullFace(GL_BACK);
				//	gl.glEnable(GL_CULL_FACE);
				//}

				//// Apply Depth Testing (if required)
				//if (task.bDepth)
				//	gl.glEnable(GL_DEPTH_TEST);

				gl.glEnable(GL_BLEND);
				//gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
				gl.glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

				if (task.bWireframe)
					gl.glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

				if (task.structure == olc::Structure::Point)
					gl.glUniform1i(shaderDefault.GetUniform("drawtype"), 1);
				else if (task.structure == olc::Structure::Line)
					gl.glUniform1i(shaderDefault.GetUniform("drawtype"), 1);
				else if (task.structure == olc::Structure::LineLoop)
					gl.glUniform1i(shaderDefault.GetUniform("drawtype"), 1);
				else
					gl.glUniform1i(shaderDefault.GetUniform("drawtype"), 0);

				if (task.structure == olc::Structure::Fan)
					gl.glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::Strip)
					gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::List)
					gl.glDrawArrays(GL_TRIANGLES, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::Line)
					gl.glDrawArrays(GL_LINE_STRIP, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::LineLoop)
					gl.glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)task.vertexBuffer.size());
				else if (task.structure == olc::Structure::Point)
					gl.glDrawArrays(GL_POINTS, 0, (GLsizei)task.vertexBuffer.size());


				if (task.bWireframe)
					gl.glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

				if (task.bDepth)
					gl.glDisable(GL_DEPTH_TEST);

			}
			break;
		}

		return true;
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
		vTargetSize = size;
		return true;
	}

	bool Renderer_OGL33::DisplayPrepare()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		gl.glEnable(GL_BLEND);
		gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl.glDepthFunc(GL_LESS);
		return false;
	}

	bool Renderer_OGL33::DisplayDraw(std::vector<void*> os_win_id, bool bVerticalSyncNow)
	{
		auto& gl = olc::apis::opengl::gl::Get();

#if OLC_HOST == OLC_HOST_WINDOWS
		auto glDeviceContext = GetDC((HWND)(os_win_id[0]));
		SwapBuffers(glDeviceContext);
		ReleaseDC((HWND)(os_win_id[0]), glDeviceContext);
#endif	

#if OLC_HOST == OLC_HOST_MACOS
        glFlushRenderAPPLE();
        glSwapAPPLE();
       
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11
		const auto window_handle = reinterpret_cast<X11::Window>(os_win_id[0]);
		auto* display = reinterpret_cast<X11::Display*>(os_win_id[1]);
		X11::glXSwapBuffers(display, window_handle);
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
		eglSwapInterval(glRenderContext.display, bVerticalSyncNow ? 1 : 0);
#endif

		return true;
	}


}
//! END IMPLEMENTATION
