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
			nPixelShaderID = gl.glCreateShader(0x8B30);			
			const char* s = srcPixelShader.c_str();
			gl.glShaderSource(nPixelShaderID, 1, &s, nullptr);
			gl.glCompileShader(nPixelShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nPixelShaderID);
		}

		// Vertex Shader
		if (!srcVertexShader.empty())
		{
			nVertexShaderID = gl.glCreateShader(0x8B31);
			const char* s = srcVertexShader.c_str();
			gl.glShaderSource(nVertexShaderID, 1, &s, nullptr);
			gl.glCompileShader(nVertexShaderID);
			// TODO: Error Check
			gl.glAttachShader(nCompiledShaderID, nVertexShaderID);
		}


		// Geometry Shader
		if (!srcGeometryShader.empty())
		{
			nGeometryShaderID = gl.glCreateShader(0x8DD9);
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

		// Create "Default" Shader
		shaderDefault.SetPixelShaderSource(
			"#version 330 core\n"
			"out vec4 pixel;\n"
			"in vec2 oTex;\n"
			"in vec4 oCol;\n"
			"uniform sampler2D sprTex;\n"
			"void main(){pixel = texture(sprTex, oTex) * oCol;}"
		);

		shaderDefault.SetVertexShaderSource(
			"#version 330 core\n"
			"layout(location = 0) in vec4 aPos;\n"
			"layout(location = 1) in vec2 aTex;\n"
			"layout(location = 2) in vec4 aCol;\n"
			"uniform mat4 mvp;\n"
			"uniform int is3d;\n"
			"uniform vec4 tint;\n"
			"out vec2 oTex;\n"
			"out vec4 oCol;\n"
			"void main(){ if(is3d!=0) {gl_Position = mvp * vec4(aPos.x, aPos.y, aPos.z, 1.0); oTex = aTex;} else {float p = 1.0 / aPos.z; gl_Position = p * vec4(aPos.x, aPos.y, 0.0, 1.0); oTex = p * aTex;} oCol = aCol * tint;}"
		);

		shaderDefault.Compile();
		shaderDefault.CreateUniform("mvp");
		shaderDefault.CreateUniform("is3d");
		shaderDefault.CreateUniform("tint");

		// Create "Default" Vertex Buffer / Vertex Attributes. This buffer is reused
		// for all drawing operations. It's possible future versions may allow the
		// creation of additional named buffers for repeated drawing operations with
		// minimal overhead.
		gl.glGenBuffers(1, &nDefaultVB);
		gl.glGenVertexArrays(1, &nDefaultVA);
		gl.glBindVertexArray(nDefaultVA);
		gl.glBindBuffer(0x8892, nDefaultVB);

		// A big one is allocated to reduce shuffles in GPU memory
		GPUTask::Vertex verts[OLC_GPU_MAX_VERTICES];
		gl.glBufferData(0x8892, sizeof(GPUTask::Vertex) * OLC_GPU_MAX_VERTICES, verts, 0x88E0);
		
		// Float Index 0 = x, 1 = y, 2 = z, 3 = w
		gl.glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex),        (void*)(0 * sizeof(float)));
		gl.glEnableVertexAttribArray(0);
		// Float Index 4 = u, 5 = v
		gl.glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GPUTask::Vertex),        (void*)(4 * sizeof(float))); 
		gl.glEnableVertexAttribArray(1);
		// Float Index 6 = (RGBA 8-bit x4)
		gl.glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(GPUTask::Vertex), (void*)(6 * sizeof(float)));	
		gl.glEnableVertexAttribArray(2);

		// Buffers are configured, unbind for now
		gl.glBindBuffer(0x8892, 0);
		gl.glBindVertexArray(0);


		// Create a null-texture so sampler doesnt fail. We don't have some of the core's helper
		// functions here, so we construct it manually
		imgBlank.Create({ 1,1 });
		imgBlank.SetGPUID(CreateTexture(imgBlank.Size()));
		imgBlank.BindCPU();
		imgBlank.Data({ 0,0 }) = olc::Colour::WHITE;
		imgBlank.BindGPU();
		WriteTexture(imgBlank.GetGPUID(), imgBlank);



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
		gl.glBindTexture(GL_TEXTURE_2D, image.GetGPUID());
		gl.glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.Size().x, image.Size().y, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
		return true;
	}

	bool Renderer_OGL33::ReadTexture(const uint32_t texid, olc::Image& image)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		gl.glBindTexture(GL_TEXTURE_2D, image.GetGPUID());
		gl.glReadPixels(0, 0, image.Size().x, image.Size().y, GL_RGBA, GL_UNSIGNED_BYTE, image.Data());
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
		return false;
	}

	bool Renderer_OGL33::AssignTextureTarget(const uint32_t slot, const uint32_t texid)
	{
		auto& gl = olc::apis::opengl::gl::Get();
		return false;
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
			case GPUTask::Task::DrawPolygon:
			{
				// Bind generic vertex buffer
				gl.glBindVertexArray(nDefaultVA);
				gl.glBindBuffer(0x8892, nDefaultVB);
				
				// Copy data from CPU to GPU
				gl.glBufferData(0x8892, sizeof(GPUTask::Vertex) * task.vertexBuffer.size(), task.vertexBuffer.data(), 0x88E0);
				
				// Shader: Configure Rendering Mode
				gl.glUniform1i(shaderDefault.GetUniform("is3d"), 0);

				// Shader: Apply MVP Matrix
				gl.glUniformMatrix4fv(shaderDefault.GetUniform("mvp"), 1, false, task.mvpMatrix.data());

				// Shader: Apply Global Tint
				float f[4] = { 
					float(task.tint.r) / 255.0f, 
					float(task.tint.g) / 255.0f, 
					float(task.tint.b) / 255.0f, 
					float(task.tint.a) / 255.0f 
				};
				gl.glUniform4fv(shaderDefault.GetUniform("tint"), 1, f);

				// Apply Culling modes
				if (task.cullmode == GPUTask::CullMode::None)
				{
					gl.glCullFace(GL_FRONT);
					gl.glDisable(GL_CULL_FACE);
				}
				else if (task.cullmode == GPUTask::CullMode::ClockWise)
				{
					gl.glCullFace(GL_FRONT);
					gl.glEnable(GL_CULL_FACE);
				}
				else if (task.cullmode == GPUTask::CullMode::CounterClockWise)
				{
					gl.glCullFace(GL_BACK);
					gl.glEnable(GL_CULL_FACE);
				}

				// Apply Depth Testing (if required)
				if (task.bDepth)
					gl.glEnable(GL_DEPTH_TEST);

				// Draw the thing!
				if (task.bWireframe)
				{
					gl.glDrawArrays(GL_LINE_LOOP, 0, (GLsizei)task.vertexBuffer.size());
				}
				else
				{
					if (task.structure == GPUTask::Structure::Fan)
						gl.glDrawArrays(GL_TRIANGLE_FAN, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::Strip)
						gl.glDrawArrays(GL_TRIANGLE_STRIP, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::List)
						gl.glDrawArrays(GL_TRIANGLES, 0, (GLsizei)task.vertexBuffer.size());
					else if (task.structure == GPUTask::Structure::Line)
						gl.glDrawArrays(GL_LINES, 0, (GLsizei)task.vertexBuffer.size());
				}

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
		return true;
	}

	bool Renderer_OGL33::DisplayPrepare()
	{
		auto& gl = olc::apis::opengl::gl::Get();

		//gl.glUseProgram(shaderDefault.GetShaderID());
		gl.glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		gl.glDepthFunc(GL_LESS);
		gl.glBindTexture(GL_TEXTURE_2D, imgBlank.GetGPUID());

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