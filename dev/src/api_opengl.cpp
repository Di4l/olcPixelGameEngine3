#include "api_opengl.h"


//! START IMPLEMENTATION
namespace olc::apis::opengl
{
	bool gl::bLoaded = false;

	gl& gl::Get()
	{
		static olc::apis::opengl::gl me;
		if (!me.bLoaded) me.LoadAll();
		return me;
	}

	bool gl::LoadAll()
	{
		bLoaded = true;
		bLoaded &= (glCreateShader = OGL_LOAD(glCreateShader)) != nullptr;
		bLoaded &= (glCreateProgram	= OGL_LOAD(glCreateProgram)) != nullptr;
		bLoaded &= (glDeleteShader = OGL_LOAD(glDeleteShader)) != nullptr;
		bLoaded &= (glCompileShader = OGL_LOAD(glCompileShader)) != nullptr;
		bLoaded &= (glLinkProgram = OGL_LOAD(glLinkProgram)) != nullptr;
		bLoaded &= (glDeleteProgram = OGL_LOAD(glDeleteProgram)) != nullptr;
		bLoaded &= (glAttachShader = OGL_LOAD(glAttachShader)) != nullptr;
		bLoaded &= (glBindBuffer = OGL_LOAD(glBindBuffer)) != nullptr;
		bLoaded &= (glBufferData = OGL_LOAD(glBufferData)) != nullptr;
		bLoaded &= (glGenBuffers = OGL_LOAD(glGenBuffers)) != nullptr;
		bLoaded &= (glVertexAttribPointer = OGL_LOAD(glVertexAttribPointer)) != nullptr;
		bLoaded &= (glEnableVertexAttribArray = OGL_LOAD(glEnableVertexAttribArray)) != nullptr;
		bLoaded &= (glUseProgram = OGL_LOAD(glUseProgram)) != nullptr;
		bLoaded &= (glBindVertexArray = OGL_LOAD(glBindVertexArray)) != nullptr;
		bLoaded &= (glGenVertexArrays = OGL_LOAD(glGenVertexArrays)) != nullptr;
		bLoaded &= (glGetShaderInfoLog = OGL_LOAD(glGetShaderInfoLog)) != nullptr;
		bLoaded &= (glGetUniformLocation = OGL_LOAD(glGetUniformLocation)) != nullptr;
		bLoaded &= (glUniform1f = OGL_LOAD(glUniform1f)) != nullptr;
		bLoaded &= (glUniform1i = OGL_LOAD(glUniform1i)) != nullptr;
		bLoaded &= (glUniform2fv = OGL_LOAD(glUniform2fv)) != nullptr;
		bLoaded &= (glUniform4fv = OGL_LOAD(glUniform4fv)) != nullptr;
		bLoaded &= (glUniformMatrix4fv = OGL_LOAD(glUniformMatrix4fv)) != nullptr;
		bLoaded &= (glActiveTexture = OGL_LOAD(glActiveTexture)) != nullptr;
		bLoaded &= (glGenFrameBuffers = OGL_LOAD(glGenFrameBuffers)) != nullptr;
		bLoaded &= (glBindFrameBuffer = OGL_LOAD(glBindFrameBuffer)) != nullptr;
		bLoaded &= (glCheckFrameBufferStatus = OGL_LOAD(glCheckFrameBufferStatus)) != nullptr;
		bLoaded &= (glDeleteFrameBuffers = OGL_LOAD(glDeleteFrameBuffers)) != nullptr;
		bLoaded &= (glFrameBufferTexture2D = OGL_LOAD(glFrameBufferTexture2D)) != nullptr;
		bLoaded &= (glDrawBuffers = OGL_LOAD(glDrawBuffers)) != nullptr;
		bLoaded &= (glBlendFuncSeparate = OGL_LOAD(glBlendFuncSeparate)) != nullptr;
		return bLoaded;
	}
}
//! END IMPLEMENTATION