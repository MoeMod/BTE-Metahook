void Shader_Init(void);

#include <winsani_in.h>
#include <windows.h>
#include <winsani_out.h>
#include <qgl.h>

extern PROC(WINAPI *qwglGetProcAddress)(LPCSTR);

extern PFNGLCREATESHADEROBJECTARBPROC qglCreateShaderObjectARB;
extern PFNGLDELETEOBJECTARBPROC qglDeleteObjectARB;
extern PFNGLSHADERSOURCEARBPROC qglShaderSourceARB;
extern PFNGLCOMPILESHADERARBPROC qglCompileShaderARB;
extern PFNGLCREATEPROGRAMOBJECTARBPROC qglCreateProgramObjectARB;
extern PFNGLATTACHOBJECTARBPROC qglAttachObjectARB;
extern PFNGLLINKPROGRAMARBPROC qglLinkProgramARB;
extern PFNGLUSEPROGRAMOBJECTARBPROC qglUseProgramObjectARB;
extern PFNGLGETUNIFORMLOCATIONARBPROC qglGetUniformLocationARB;
extern PFNGLUNIFORM1IARBPROC qglUniform1iARB;
extern PFNGLUNIFORM2IARBPROC qglUniform2iARB;
extern PFNGLUNIFORM3IARBPROC qglUniform3iARB;
extern PFNGLUNIFORM4IARBPROC qglUniform4iARB;
extern PFNGLUNIFORM1FARBPROC qglUniform1fARB;
extern PFNGLUNIFORM2FARBPROC qglUniform2fARB;
extern PFNGLUNIFORM3FARBPROC qglUniform3fARB;
extern PFNGLUNIFORM4FARBPROC qglUniform4fARB;
extern PFNGLUNIFORM3FVARBPROC qglUniform3fvARB;
extern PFNGLUNIFORM4FVARBPROC qglUniform4fvARB;
extern PFNGLGETSHADERIVPROC qglGetShaderiv;
extern PFNGLGETSHADERINFOLOGPROC qglGetShaderInfoLog;
extern PFNGLGETINFOLOGARBPROC qglGetInfoLogARB;
/*
extern PFNGLGENFRAMEBUFFERSEXTPROC qglGenFramebuffers;
extern PFNGLGENRENDERBUFFERSPROC qglGenRenderbuffers;
extern PFNGLBINDFRAMEBUFFERPROC qglBindFramebuffer;
extern PFNGLBINDRENDERBUFFERPROC qglBindRenderbuffer;
extern PFNGLRENDERBUFFERSTORAGEPROC qglRenderbufferStorage;
extern PFNGLFRAMEBUFFERTEXTURE2DPROC qglFramebufferTexture2D;
extern PFNGLFRAMEBUFFERRENDERBUFFERPROC qglFramebufferRenderbuffer;

extern PFNGLBLITFRAMEBUFFERPROC qglBlitFramebuffer;

extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
*/
typedef struct
{
	int tex0;
	int rt_w;
	int rt_h;
	int FXAA_SPAN_MAX;
	int FXAA_REDUCE_MUL;
	int FXAA_SUBPIX_SHIFT;
}uniformFXAA_t;

extern int progFXAA;
extern uniformFXAA_t uniformFXAA;