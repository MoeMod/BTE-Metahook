#include <base.h>
#include "configs.h"
#include "shader.h"
#include "sys.h"
#include "qgl.h"

GLuint R_CompileShader(const char *vscode, const char *fscode, const char *vsfile, const char *fsfile);
int LoadShader(char *vs, char *fs);


//PROC(WINAPI *qwglGetProcAddress)(LPCSTR) = NULL;

PFNGLCREATESHADEROBJECTARBPROC qglCreateShaderObjectARB = NULL;
PFNGLDELETEOBJECTARBPROC qglDeleteObjectARB = NULL;
PFNGLSHADERSOURCEARBPROC qglShaderSourceARB = NULL;
PFNGLCOMPILESHADERARBPROC qglCompileShaderARB = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC qglCreateProgramObjectARB = NULL;
PFNGLATTACHOBJECTARBPROC qglAttachObjectARB = NULL;
PFNGLLINKPROGRAMARBPROC qglLinkProgramARB = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC qglUseProgramObjectARB = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC qglGetUniformLocationARB = NULL;
PFNGLUNIFORM1IARBPROC qglUniform1iARB = NULL;
PFNGLUNIFORM2IARBPROC qglUniform2iARB = NULL;
PFNGLUNIFORM3IARBPROC qglUniform3iARB = NULL;
PFNGLUNIFORM4IARBPROC qglUniform4iARB = NULL;
PFNGLUNIFORM1FARBPROC qglUniform1fARB = NULL;
PFNGLUNIFORM2FARBPROC qglUniform2fARB = NULL;
PFNGLUNIFORM3FARBPROC qglUniform3fARB = NULL;
PFNGLUNIFORM4FARBPROC qglUniform4fARB = NULL;
PFNGLUNIFORM3FVARBPROC qglUniform3fvARB = NULL;
PFNGLUNIFORM4FVARBPROC qglUniform4fvARB = NULL;
PFNGLGETSHADERIVPROC qglGetShaderiv = NULL;
PFNGLGETSHADERINFOLOGPROC qglGetShaderInfoLog = NULL;
PFNGLGETINFOLOGARBPROC qglGetInfoLogARB = NULL;
/*
PFNGLGENFRAMEBUFFERSEXTPROC qglGenFramebuffers = NULL;
PFNGLGENRENDERBUFFERSPROC qglGenRenderbuffers = NULL;
PFNGLBINDFRAMEBUFFERPROC qglBindFramebuffer = NULL;
PFNGLBINDRENDERBUFFERPROC qglBindRenderbuffer = NULL;
PFNGLRENDERBUFFERSTORAGEPROC qglRenderbufferStorage = NULL;
PFNGLFRAMEBUFFERTEXTURE2DPROC qglFramebufferTexture2D = NULL;
PFNGLFRAMEBUFFERRENDERBUFFERPROC qglFramebufferRenderbuffer = NULL;
PFNGLBLITFRAMEBUFFERPROC qglBlitFramebuffer = NULL;

PFNGLGENERATEMIPMAPPROC glGenerateMipmap = NULL;
*/

int progFXAA;
uniformFXAA_t uniformFXAA;



void Shader_Init(void)
{
	if (!gConfigs.bEnableShader)
		return;
	
	//取额外所需qgl函数的地址
	HMODULE hOpenGL = GetModuleHandle("opengl32.dll");
	*(FARPROC *)&qwglGetProcAddress = GetProcAddress(hOpenGL, "wglGetProcAddress");

	//检查扩展
	const char *extension = (const char *)glGetString(GL_EXTENSIONS);
	if (!strstr(extension, "GL_ARB_shader_objects") || !strstr(extension, "GL_ARB_vertex_shader") || !strstr(extension, "GL_ARB_fragment_shader"))
	{		
		Sys_ErrorEx((!strcmp(gConfigs.szLanguage, "schinese")) ? "你的显卡不支持 ARB Shader" : "Your GPU not support ARB Shader");
		return;
	}
	
	qglCreateShaderObjectARB = (PFNGLCREATESHADEROBJECTARBPROC)qwglGetProcAddress("glCreateShaderObjectARB");
	qglDeleteObjectARB = (PFNGLDELETEOBJECTARBPROC)qwglGetProcAddress("glDeleteObjectARB");
	qglShaderSourceARB = (PFNGLSHADERSOURCEARBPROC)qwglGetProcAddress("glShaderSourceARB");
	qglCompileShaderARB = (PFNGLCOMPILESHADERARBPROC)qwglGetProcAddress("glCompileShaderARB");
	qglCreateProgramObjectARB = (PFNGLCREATEPROGRAMOBJECTARBPROC)qwglGetProcAddress("glCreateProgramObjectARB");
	qglAttachObjectARB = (PFNGLATTACHOBJECTARBPROC)qwglGetProcAddress("glAttachObjectARB");
	qglLinkProgramARB = (PFNGLLINKPROGRAMARBPROC)qwglGetProcAddress("glLinkProgramARB");
	qglUseProgramObjectARB = (PFNGLUSEPROGRAMOBJECTARBPROC)qwglGetProcAddress("glUseProgramObjectARB");
	qglGetUniformLocationARB = (PFNGLGETUNIFORMLOCATIONARBPROC)qwglGetProcAddress("glGetUniformLocationARB");
	
	qglUniform1iARB = (PFNGLUNIFORM1IARBPROC)qwglGetProcAddress("glUniform1iARB");
	qglUniform2iARB = (PFNGLUNIFORM2IARBPROC)qwglGetProcAddress("glUniform2iARB");
	qglUniform3iARB = (PFNGLUNIFORM3IARBPROC)qwglGetProcAddress("glUniform3iARB");
	qglUniform4iARB = (PFNGLUNIFORM4IARBPROC)qwglGetProcAddress("glUniform4iARB");
	qglUniform1fARB = (PFNGLUNIFORM1FARBPROC)qwglGetProcAddress("glUniform1fARB");
	qglUniform2fARB = (PFNGLUNIFORM2FARBPROC)qwglGetProcAddress("glUniform2fARB");
	qglUniform3fARB = (PFNGLUNIFORM3FARBPROC)qwglGetProcAddress("glUniform3fARB");
	qglUniform4fARB = (PFNGLUNIFORM4FARBPROC)qwglGetProcAddress("glUniform4fARB");
	qglUniform3fvARB = (PFNGLUNIFORM3FVARBPROC)qwglGetProcAddress("glUniform3fvARB");
	qglUniform4fvARB = (PFNGLUNIFORM4FVARBPROC)qwglGetProcAddress("glUniform4fvARB");

	qglGetShaderiv = (PFNGLGETSHADERIVPROC)qwglGetProcAddress("glGetShaderiv");
	qglGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)qwglGetProcAddress("glGetShaderInfoLog");
	qglGetInfoLogARB = (PFNGLGETINFOLOGARBPROC)qwglGetProcAddress("glGetInfoLogARB");
	/*
	qglGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC)qwglGetProcAddress("glGenFramebuffers");
	qglGenRenderbuffers = (PFNGLGENRENDERBUFFERSPROC)qwglGetProcAddress("glGenRenderbuffers");
	qglBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)qwglGetProcAddress("glBindFramebuffer");
	qglBindRenderbuffer = (PFNGLBINDRENDERBUFFERPROC)qwglGetProcAddress("glBindRenderbuffer");
	qglRenderbufferStorage = (PFNGLRENDERBUFFERSTORAGEPROC)qwglGetProcAddress("glRenderbufferStorage");
	qglFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)qwglGetProcAddress("glFramebufferTexture2D");
	qglFramebufferRenderbuffer = (PFNGLFRAMEBUFFERRENDERBUFFERPROC)qwglGetProcAddress("glFramebufferRenderbuffer");
	qglBlitFramebuffer = (PFNGLBLITFRAMEBUFFERPROC)qwglGetProcAddress("glBlitFramebuffer");

	glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)qwglGetProcAddress("glGenerateMipmap");
	*/
	progFXAA = LoadShader("shader/pp_fxaa.vsh", "shader/pp_fxaa.fsh");

	uniformFXAA.tex0 = qglGetUniformLocationARB(progFXAA, "tex0");
	uniformFXAA.rt_w = qglGetUniformLocationARB(progFXAA, "rt_w");
	uniformFXAA.rt_h = qglGetUniformLocationARB(progFXAA, "rt_h");
	uniformFXAA.FXAA_SPAN_MAX = qglGetUniformLocationARB(progFXAA, "FXAA_SPAN_MAX");
	uniformFXAA.FXAA_REDUCE_MUL = qglGetUniformLocationARB(progFXAA, "FXAA_REDUCE_MUL");
	uniformFXAA.FXAA_SUBPIX_SHIFT = qglGetUniformLocationARB(progFXAA, "FXAA_SUBPIX_SHIFT");
}

int LoadShader(char *vs, char *fs)
{
	const char *vscode = (const char *)gEngfuncs.COM_LoadFile(vs, 5, 0);
	const char *fscode = (const char *)gEngfuncs.COM_LoadFile(fs, 5, 0);

	if (!vscode || !fscode)
	{
		Sys_ErrorEx((!strcmp(gConfigs.szLanguage, "schinese")) ? "着色器代码加载失败" : "shader file load failed");
		return 0;
	}

	int program = R_CompileShader(vscode, fscode, vs, fs);

	if (!program)
	{
		Sys_ErrorEx((!strcmp(gConfigs.szLanguage, "schinese")) ? "着色器编译失败" : "shader complie failed");
		return 0;
	}

	return program;
}

qboolean GL_IsShaderError(GLuint shader, const char *filename)
{
	char compiler_log[1000];
	int blen;

	qglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &blen);
	if (blen > 1)
	{
		int slen;
		qglGetInfoLogARB(shader, 1000, &slen, compiler_log);

		if (strstr(compiler_log, "No errors"))
			return false;

		//Sys_ErrorEx(compiler_log);
		if (!strstr(compiler_log, "error"))
		{
			gEngfuncs.Con_Printf("Shader %s compiled with warning: %s\n", filename, compiler_log);
			return false;
		}
		gEngfuncs.Con_Printf("Shader %s compiled with error: %s\n", filename, compiler_log);
		return true;
	}
	
	return false;
}

GLuint R_CompileShader(const char *vscode, const char *fscode, const char *vsfile, const char *fsfile)
{
	GLuint vs = qglCreateShaderObjectARB(GL_VERTEX_SHADER_ARB);
	qglShaderSourceARB(vs, 1, &vscode, NULL);
	qglCompileShaderARB(vs);
	if (GL_IsShaderError(vs, vsfile))
	{
		qglDeleteObjectARB(vs);
		return 0;
	}

	GLuint fs = qglCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB);
	qglShaderSourceARB(fs, 1, &fscode, NULL);
	qglCompileShaderARB(fs);
	if (GL_IsShaderError(fs, fsfile))
	{
		qglDeleteObjectARB(fs);
		return 0;
	}

	GLuint prog = qglCreateProgramObjectARB();
	qglAttachObjectARB(prog, vs);
	qglAttachObjectARB(prog, fs);
	qglLinkProgramARB(prog);

	return prog;
}
