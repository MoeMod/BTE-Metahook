#include <metahook.h>

#include <gl/gl.h>
#include <gl/glext.h>

PFNGLBINDBUFFERARBPROC glBindBufferARB;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
PFNGLGENBUFFERSARBPROC glGenBuffersARB;
PFNGLGENBUFFERSARBPROC glIsBufferARB;
PFNGLBUFFERDATAARBPROC glBufferDataARB;
PFNGLBUFFERSUBDATAARBPROC glBufferSubDataARB;
PFNGLGETBUFFERSUBDATAARBPROC glGetBufferSubDataARB;
PFNGLMAPBUFFERARBPROC glMapBufferARB;
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB;
PFNGLGETBUFFERPOINTERVARBPROC glGetBufferPointervARB;

void PBO_Init(void)
{
	glBindBufferARB = (PFNGLBINDBUFFERARBPROC)wglGetProcAddress("glBindBufferARB");
	glDeleteBuffersARB = (PFNGLDELETEBUFFERSARBPROC)wglGetProcAddress("glDeleteBuffersARB");
	glGenBuffersARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glGenBuffersARB");
	glIsBufferARB = (PFNGLGENBUFFERSARBPROC)wglGetProcAddress("glIsBufferARB");
	glBufferDataARB = (PFNGLBUFFERDATAARBPROC)wglGetProcAddress("glBufferDataARB");
	glBufferSubDataARB = (PFNGLBUFFERSUBDATAARBPROC)wglGetProcAddress("glBufferSubDataARB");
	glGetBufferSubDataARB = (PFNGLGETBUFFERSUBDATAARBPROC)wglGetProcAddress("glGetBufferSubDataARB");
	glMapBufferARB = (PFNGLMAPBUFFERARBPROC)wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB = (PFNGLUNMAPBUFFERARBPROC)wglGetProcAddress("glUnmapBufferARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)wglGetProcAddress("glGetBufferParameterivARB");
	glGetBufferPointervARB = (PFNGLGETBUFFERPOINTERVARBPROC)wglGetProcAddress("glGetBufferPointervARB");
}