#pragma once

const char *FileExtension( const char *in );
int FileExist(const char *szFile);
int LogToFile(char *szLogText, ...);
void *InterceptDLLCall(HMODULE hModule, char *szDllName, char *szFunctionName, DWORD pNewFunction);
DWORD FindPattern(char *szPattern, int iLen, DWORD dwStart, DWORD dwLen);
client_sprite_t *GetSpriteList(client_sprite_t *pList, const char *psz, int iRes, int iCount);
void UnpackRGB(int &r, int &g, int &b, unsigned long ulRGB);
void ScaleColors(int &r, int &g, int &b, int a);
#if 0
#define VectorSubtract(a,b,c) {(c)[0]=(a)[0]-(b)[0];(c)[1]=(a)[1]-(b)[1];(c)[2]=(a)[2]-(b)[2];}
#define VectorAdd(a,b,c) {(c)[0]=(a)[0]+(b)[0];(c)[1]=(a)[1]+(b)[1];(c)[2]=(a)[2]+(b)[2];}
#define VectorCopy(a,b) {(b)[0]=(a)[0];(b)[1]=(a)[1];(b)[2]=(a)[2];}
#endif
inline void VectorClear(float *a) { a[0]=0.0;a[1]=0.0;a[2]=0.0;}

#define AngleVectors (*gEngfuncs.pfnAngleVectors)
#define SPR_Load (*gEngfuncs.pfnSPR_Load)
#define SPR_Set (*gEngfuncs.pfnSPR_Set)
#define SPR_Height (*gEngfuncs.pfnSPR_Height)
#define SPR_Width (*gEngfuncs.pfnSPR_Width)
#define SPR_DrawAdditive (*gEngfuncs.pfnSPR_DrawAdditive)
#define SPR_GetList (*gEngfuncs.pfnSPR_GetList)
#define CVAR_GET_FLOAT (*gEngfuncs.pfnGetCvarFloat)

#define RANDOM_LONG (*gEngfuncs.pfnRandomLong)
#define RANDOM_FLOAT (*gEngfuncs.pfnRandomFloat)

#define HOOK_MESSAGE(x) gEngfuncs.pfnHookUserMsg(#x, __MsgFunc_##x );
#define MESSAGE_BEGIN MSG_BeginWrite
#define WRITE_BYTE MSG_WriteByte
#define WRITE_COORD MSG_WriteCoord
#define WRITE_SHORT MSG_WriteShort
#define MESSAGE_END MSG_EndWrite

#define DECLARE_MESSAGE(y, x) int __MsgFunc_##x(const char *pszName, int iSize, void *pbuf) \
{ \
	return gHUD.y.MsgFunc_##x(pszName, iSize, pbuf); \
}
#define HOOK_COMMAND(x, y) gEngfuncs.pfnAddCommand( x, __CmdFunc_##y );
#define DECLARE_COMMAND(y, x) void __CmdFunc_##x( void ) \
{ \
	gHUD.y.UserCmd_##x(); \
}

void VectorAngles(const float *forward, float *angles);
#if 0
inline void VectorScale(const float *in, vec_t scale, float *out)
{
	out[0] = in[0] * scale;
	out[1] = in[1] * scale;
	out[2] = in[2] * scale;
}

inline void VectorScale(const float *in, const float *scale, float *out)
{
	out[0] = in[0] * scale[0];
	out[1] = in[1] * scale[1];
	out[2] = in[2] * scale[2];
}
#endif

void ClientPrint(int msg_dest, const char *msg_name, const char *param1 = NULL, const char *param2 = NULL, const char *param3 = NULL, const char *param4 = NULL);

wchar_t *UTF8ToUnicode( const char* str );
wchar_t *ANSIToUnicode( const char* str );
char *UnicodeToANSI( const wchar_t* str );
char *UnicodeToUTF8( const wchar_t* str );
wchar_t *GetLangUni(char *pLeft);
wchar_t *GetLangUni2(char *pLeft);
char *GetLangUtf(char *pLeft);

int UTIL_SharedRandomLong(unsigned int seed, int low, int high);
float UTIL_SharedRandomFloat(unsigned int seed, float low, float high);