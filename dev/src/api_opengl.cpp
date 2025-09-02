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
		// Load all the OpenGL API entry points
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

		if (!bLoaded)
			return false; // API load has failed



		return bLoaded;
	}

	void gl::glGenTextures(GLsizei n, GLuint* textures)
	{
		::glGenTextures(n, textures);
	}

	void gl::glBindTexture(GLenum target, GLuint texture)
	{
		::glBindTexture(target, texture);
	}

	void gl::glTexParameteri(GLenum target, GLenum pname, GLint param)
	{
		::glTexParameteri(target, pname, param);
	}

	void gl::glTexEnvf(GLenum target, GLenum pname, GLfloat param)
	{
		::glTexEnvf(target, pname, param);
	}

	void gl::glDeleteTextures(GLsizei n, const GLuint* textures)
	{
		::glDeleteTextures(n, textures);
	}

	void gl::glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels)
	{
		::glTexImage2D(target, level, internalformat, width, height, border, format, type, pixels);
	}

	void gl::glClear(GLbitfield mask)
	{
		::glClear(mask);
	}

	void gl::glViewport(GLint x, GLint y, GLsizei width, GLsizei height)
	{
		::glViewport(x, y, width, height);
	}

	void gl::glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
	{
		::glClearColor(red, green, blue, alpha);
	}
}
//! END IMPLEMENTATION