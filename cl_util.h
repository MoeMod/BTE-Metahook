#ifndef CL_UTIL_H
#define CL_UTIL_H

#include "cdll_dll.h"
#include <cvardef.h>
//#include "cmd.h"

#define HOOK_MESSAGE(x) MSG_HookUserMsg(#x, __MsgFunc_##x);

#define DECLARE_MESSAGE(y, x) \
int __MsgFunc_##x(const char *pszName, int iSize, void *pbuf) \
{ \
	return gHUD.##y.MsgFunc_##x(pszName, iSize, pbuf); \
}

#define DECLARE_MESSAGE2(x) \
int __MsgFunc_##x(const char *pszName, int iSize, void *pbuf) \
{ \
	return gHUD.MsgFunc_##x(pszName, iSize, pbuf); \
}

#define HOOK_COMMAND(x, y) Cmd_HookCmd(x, __CmdFunc_##y);
#define REMOVE_COMMAND(x) Cmd_RemoveCmd(x);

#define DECLARE_COMMAND(y, x) \
void __CmdFunc_##x(void)\
{ \
	gHUD.##y.UserCmd_##x(); \
}

#define DECLARE_COMMAND2(x) \
void __CmdFunc_##x(void)\
{ \
	gHUD.UserCmd_##x(); \
}

#define FillRGBA (*gEngfuncs.pfnFillRGBA)

#define GetScreenInfo (*gEngfuncs.pfnGetScreenInfo)
#define ServerCmd (*gEngfuncs.pfnServerCmd)
#define ClientCmd (*gEngfuncs.pfnClientCmd)
#define SetCrosshair (*gEngfuncs.pfnSetCrosshair)
#define AngleVectors (*gEngfuncs.pfnAngleVectors)

#define ScreenHeight (gHUD.m_scrinfo.iHeight)
#define ScreenWidth (gHUD.m_scrinfo.iWidth)

#define XPROJECT(x)	((1.0f + (x)) * ScreenWidth * 0.5f)
#define YPROJECT(y) ((1.0f - (y)) * ScreenHeight * 0.5f)

#define XRES(x) ((int)(float(x) * ((float)ScreenWidth / 640.0f) + 0.5f))
#define YRES(y) ((int)(float(y) * ((float)ScreenHeight / 480.0f) + 0.5f))

#define CVAR_GET_FLOAT gEngfuncs.pfnGetCvarFloat
#define SPR_Load gEngfuncs.pfnSPR_Load
#define SPR_GetList gEngfuncs.pfnSPR_GetList
#define SPR_Set gEngfuncs.pfnSPR_Set
#define SPR_Draw gEngfuncs.pfnSPR_Draw
#define SPR_DrawAdditive gEngfuncs.pfnSPR_DrawAdditive
#define SPR_DrawHoles gEngfuncs.pfnSPR_DrawHoles
#define SPR_DrawGeneric gEngfuncs.pfnSPR_DrawGeneric

inline void PlaySound(char *szSound, float vol) { gEngfuncs.pfnPlaySoundByName(szSound, vol); }
inline void PlaySound(int iSound, float vol) { gEngfuncs.pfnPlaySoundByIndex(iSound, vol); }

inline int SPR_Height(HSPRITE x, int f) { return gEngfuncs.pfnSPR_Height(x, f); }
inline int SPR_Width(HSPRITE x, int f) { return gEngfuncs.pfnSPR_Width(x, f); }

inline client_textmessage_t *TextMessageGet(const char *pName) { return gEngfuncs.pfnTextMessageGet(pName); }

inline int TextMessageDrawChar(int x, int y, int number, int r, int g, int b)
{
	return gEngfuncs.pfnDrawCharacter(x, y, number, r, g, b);
}

inline int DrawConsoleString(int x, int y, const char *string)
{
	return gEngfuncs.pfnDrawConsoleString(x, y, (char *)string);
}

inline void GetConsoleStringSize(const char *string, int *width, int *height)
{
	gEngfuncs.pfnDrawConsoleStringLen(string, width, height);
}

inline int ConsoleStringLen(const char *string)
{
	int _width, _height;
	GetConsoleStringSize(string, &_width, &_height);
	return _width;
}

inline void ConsolePrint(const char *string)
{
	gEngfuncs.pfnConsolePrint(string);
}

inline void CenterPrint(const char *string)
{
	gEngfuncs.pfnCenterPrint(string);
}

#define GetPlayerInfo (*gEngfuncs.pfnGetPlayerInfo)

#define DotProduct(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}

inline void VectorClear(float *a) { a[0] = 0.0; a[1] = 0.0; a[2] = 0.0; }
float Length(const float *v);
void VectorMA(const float *veca, float scale, const float *vecb, float *vecc);
void VectorScale(const float *in, float scale, float *out);
float VectorNormalize(float *v);
void VectorInverse(float *v);

extern vec3_t vec3_origin;

#pragma warning(disable: 4244)
#pragma warning(disable: 4305)

inline void UnpackRGB(int &r, int &g, int &b, unsigned long ulRGB)
{
	r = (ulRGB & 0xFF0000) >> 16;
	g = (ulRGB & 0xFF00) >> 8;
	b = ulRGB & 0xFF;
}

inline void ScaleColors(int &r, int &g, int &b, int a)
{
	float x = (float)a / 255;

	r = (int)(r * x);
	g = (int)(g * x);
	b = (int)(b * x);
}

HSPRITE LoadSprite(const char *pszName);
char *VarArgs(char *format, ...);
const wchar_t *NumAsWString(int val);
wchar_t *BufWPrintf(wchar_t *buf, int& len, const wchar_t *fmt, ...);
char *BufPrintf(char *buf, int& len, const char *fmt, ...);



#endif