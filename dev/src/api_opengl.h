#pragma once


//! START CUSTOMHEADER
#include "config.h"
//! END CUSTOMHEADER


//! START OPENGL_CONFIG

#if OLC_HOST == OLC_HOST_WINDOWS
	#include <Windows.h>
	#pragma comment(lib, "gdi32.lib")
	#pragma comment(lib, "opengl32.lib")
	#include <gl/GL.h>
	#define CALLSTYLE __stdcall
	// ooof... was getting a bunch of spurious C4191 from MSVC 17.14.9, so round trip via void-town
	#define OGL_LOAD(t) reinterpret_cast<t##_t*>(reinterpret_cast<void*>(wglGetProcAddress(#t)))
#endif

#if OLC_HOST == OLC_HOST_LINUX_X11 || OLC_HOST == OLC_HOST_LINUX_WAYLAND
	#include <GL/gl.h>
	#if OLC_HOST == OLC_HOST_LINUX_X11
		namespace X11
		{
			#include <GL/glx.h>
		}
	#endif
#endif

#if OLC_HOST == OLC_HOST_MACOS
	#define GL_SILENCE_DEPRECATION
	#include <OpenGL/OpenGL.h>
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
#endif

#if OLC_HOST == OLC_HOST_EMSCRIPTEN
	#include <EGL/egl.h>
	#include <GLES2/gl2.h>
	#define GL_GLEXT_PROTOTYPES
	#include <GLES2/gl2ext.h>
	#include <emscripten/emscripten.h>
	#define GL_CLAMP GL_CLAMP_TO_EDGE
#endif

#if !defined(CALLSTYLE)
	#define CALLSTYLE
#endif


//! END OPENGL_CONFIG

//! START DECLARATION
namespace olc
{
	namespace apis::opengl
	{
#if OLC_HOST == OLC_HOST_WINDOWS
		// "Target Host Window Handle"
		typedef HDC glDeviceContext_t;
		// "State of OpenGL Machinary"
		typedef HGLRC glRenderContext_t;
#endif

		typedef char GLchar;
		typedef ptrdiff_t GLsizeiptr;

		typedef GLuint CALLSTYLE glCreateShader_t(GLenum type);
		typedef GLuint CALLSTYLE glCreateProgram_t(void);
		typedef void CALLSTYLE glDeleteShader_t(GLuint shader);
		typedef void CALLSTYLE glCompileShader_t(GLuint shader);
		typedef void CALLSTYLE glLinkProgram_t(GLuint program);
		typedef void CALLSTYLE glDeleteProgram_t(GLuint program);
		typedef void CALLSTYLE glAttachShader_t(GLuint program, GLuint shader);
		typedef void CALLSTYLE glBindBuffer_t(GLenum target, GLuint buffer);
		typedef void CALLSTYLE glBufferData_t(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
		typedef void CALLSTYLE glGenBuffers_t(GLsizei n, GLuint* buffers);
		typedef void CALLSTYLE glVertexAttribPointer_t(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
		typedef void CALLSTYLE glEnableVertexAttribArray_t(GLuint index);
		typedef void CALLSTYLE glUseProgram_t(GLuint program);
		typedef void CALLSTYLE glBindVertexArray_t(GLuint array);
		typedef void CALLSTYLE glGenVertexArrays_t(GLsizei n, GLuint* arrays);
		typedef void CALLSTYLE glGetShaderInfoLog_t(GLuint shader, GLsizei bufSize, GLsizei* length, GLchar* infoLog);
		typedef GLint CALLSTYLE glGetUniformLocation_t(GLuint program, const GLchar* name);
		typedef void CALLSTYLE glUniform1f_t(GLint location, GLfloat v0);
		typedef void CALLSTYLE glUniform1i_t(GLint location, GLint v0);
		typedef void CALLSTYLE glUniform2fv_t(GLint location, GLsizei count, const GLfloat* value);
		typedef void CALLSTYLE glUniform4fv_t(GLint location, GLsizei count, const GLfloat* value);
		typedef void CALLSTYLE glUniformMatrix4fv_t(GLint location, GLsizei count, GLboolean trasnpose, const GLfloat* value);
		typedef void CALLSTYLE glActiveTexture_t(GLenum texture);
		typedef void CALLSTYLE glGenFrameBuffers_t(GLsizei n, GLuint* ids);
		typedef void CALLSTYLE glBindFrameBuffer_t(GLenum target, GLuint fb);
		typedef GLenum CALLSTYLE glCheckFrameBufferStatus_t(GLenum target);
		typedef void CALLSTYLE glDeleteFrameBuffers_t(GLsizei n, const GLuint* fbs);
		typedef void CALLSTYLE glFrameBufferTexture2D_t(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
		typedef void CALLSTYLE glDrawBuffers_t(GLsizei n, const GLenum* bufs);
		typedef void CALLSTYLE glBlendFuncSeparate_t(GLenum srcRGB, GLenum dstRGB, GLenum srcAlpha, GLenum dstAlpha);

		

#if OLC_HOST == OLC_HOST_WINDOWS
		typedef void CALLSTYLE glSwapInterval_t(GLsizei n);
#endif

		// A little GL class (singleton)
		class gl
		{
		public:
			static gl& Get();

		private:
			gl() = default;
			static bool bLoaded;

		private:
			bool LoadAll();

		public:
			glCreateShader_t* glCreateShader = nullptr;
			glCreateProgram_t* glCreateProgram = nullptr;
			glDeleteShader_t* glDeleteShader = nullptr;
			glCompileShader_t* glCompileShader = nullptr;
			glLinkProgram_t* glLinkProgram = nullptr;
			glDeleteProgram_t* glDeleteProgram = nullptr;
			glAttachShader_t* glAttachShader = nullptr;
			glBindBuffer_t* glBindBuffer = nullptr;
			glBufferData_t* glBufferData = nullptr;
			glGenBuffers_t* glGenBuffers = nullptr;
			glVertexAttribPointer_t* glVertexAttribPointer = nullptr;
			glEnableVertexAttribArray_t* glEnableVertexAttribArray = nullptr;
			glUseProgram_t* glUseProgram = nullptr;
			glBindVertexArray_t* glBindVertexArray = nullptr;
			glGenVertexArrays_t* glGenVertexArrays = nullptr;
			glGetShaderInfoLog_t* glGetShaderInfoLog = nullptr;
			glGetUniformLocation_t* glGetUniformLocation = nullptr;
			glUniform1f_t* glUniform1f = nullptr;
			glUniform1i_t* glUniform1i = nullptr;
			glUniform2fv_t* glUniform2fv = nullptr;
			glUniform4fv_t* glUniform4fv = nullptr;
			glUniformMatrix4fv_t* glUniformMatrix4fv = nullptr;
			glActiveTexture_t* glActiveTexture = nullptr;
			glGenFrameBuffers_t* glGenFrameBuffers = nullptr;
			glBindFrameBuffer_t* glBindFrameBuffer = nullptr;
			glCheckFrameBufferStatus_t* glCheckFrameBufferStatus = nullptr;
			glDeleteFrameBuffers_t* glDeleteFrameBuffers = nullptr;
			glFrameBufferTexture2D_t* glFrameBufferTexture2D = nullptr;
			glDrawBuffers_t* glDrawBuffers = nullptr;
			glBlendFuncSeparate_t* glBlendFuncSeparate = nullptr;

			// OpenGL1.2 Proxies (just keeps things tidy imo)
			void glGenTextures(GLsizei n, GLuint* textures);
			void glBindTexture(GLenum target, GLuint texture);
			void glTexParameteri(GLenum target, GLenum pname, GLint param);
			void glTexEnvf(GLenum target, GLenum pname, GLfloat param);
			void glDeleteTextures(GLsizei n, const GLuint* textures); 
			void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid* pixels);
			void glClear(GLbitfield mask);
			void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);
			void glClearColor(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);

		};
	}
	


//#if defined(OLC_PLATFORM_X11)
//	typedef int(locSwapInterval_t)(X11::Display* dpy, X11::GLXDrawable drawable, int interval);
//#endif
//
//#if defined(OLC_PLATFORM_EMSCRIPTEN)
//	typedef void CALLSTYLE locShaderSource_t(GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
//	typedef EGLBoolean(locSwapInterval_t)(EGLDisplay display, EGLint interval);
//#else
//	typedef void CALLSTYLE locShaderSource_t(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
//#endif

} // olc namespace
//! END DECLARATION





