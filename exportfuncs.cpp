#include "base.h"
#include "hud.h"
#include "configs.h"
#include "events.h"
#include "BaseUI.h"
#include "BugReport/CrashHandle.h"
#include "msghook.h"
#include "sprite.h"
#include "playaudio.h"
#include "engfuncs.h"
#include "HUD3D.h"
#include "HUD3D_ZB.h"
#include "muzzleflash.h"
#include "message.h"
#include "msg_util.h"
#include "sys.h"
#include "Fonts.h"
#include "common.h"
#include "R.h"
#include "IMEInput.h"
#include "CVARs.h"
#include "Window.h"

#include "TriAPI.h"
#include "display.h"
#include "engfunchook.h"
#include "exportfuncs.h"

#include "MGUI/BTEPanel.h"
#include "MGUI/mgui.h"
#include "MGUI/TeamMenu.h"
#include "MGUI/NormalZombieMenu.h"

#include "shader.h"
#include "util.h"
#include "weapons.h"
#include "weaponinfo.h"
#include "Client/PlayerClassManager.h"
#include "Client/WeaponManager.h"

#include "Client/HUD/health.h"
#include "Client/HUD/DrawTABPanel.h"
#include "Client/HUD/overview.h"
#include "Client/HUD/nvg.h"
#include "Client/HUD/sfsniper.h"
#include "Client/HUD/destroyer.h"
#include "Client/HUD/buffawp.h"
#include "Client/HUD/zb3ui.h"

#include "Renderer/pbo.h"

using namespace vgui;

//HFont g_hCurrentFont;

cvar_t *cl_ghost_view_render;
cvar_t *cl_lw = NULL;
cvar_t *gl_fxaa;
cvar_t *g_pcvarEnableConsole = NULL;

cl_enginefunc_t gEngfuncs;
cl_enginefunc_t *engine = &gEngfuncs;

engine_studio_api_t IEngineStudio;
event_api_s gEventAPI;
efx_api_s gEfxAPI;
bool bLoadedSky = false;
bool bLoadedTga = false;

int g_iUser1;
int g_iUser2;
int g_iUser3;
int g_iTeamNumber;
int g_iFreezeTimeOver;
int g_iPlayerFlags;
float g_flPlayerSpeed;
int g_iWeaponFlags;

int g_iMod;
int g_iWeaponLimit;
float g_Entity_Index[1024];
float key_last_press = 0.0;
float g_Next_Key_CanUse = 0.0;

int g_iHookVAngle;
int g_iShowCustomCrosshair;
float g_vecVAngle[3];
float v_angles[3];
float punchangle[3];
float g_velocity[3];
float vieworg[3];

int g_iScreenTexture;

int g_iSniperScope[4];

float g_flNextMouseEvent;
float g_flNextSkillKeyPress = 0;

int g_iZBNvg = 0;
float g_flZESFade = 0;

int g_bFlashActive;
int g_iFlashBat = 100;

float g_flTutorClose;

int g_iWeaponAnim = 0;

int(__fastcall *g_pfnHudSniperScope_Draw)(void *, int, float flTime) = (int(__fastcall *)(void *, int, float))0x1961AD0;
int __fastcall HudSniperScope_Draw(void *p, int i, float f);
void(__fastcall *g_pfnCHudSayText_Draw)(void *, int, float) = (void(__fastcall *)(void *, int, float))0x1960A10;

cl_entity_t **r_currententity;

void __fastcall CHudSayText_Draw(void *pthis, int, float flTime)
{
	if (gConfigs.bEnableNewHud)
	{
		return;
	}
	return g_pfnCHudSayText_Draw(pthis, 0, flTime);
}

char TranslateKeyCharacter(int keynum, BOOL bIgnoreShift, BOOL bIgnorePad)
{
	if (bIgnorePad)
	{
		if (keynum >= 160 && keynum <= 162)
			return keynum - 160 + '7';
		if (keynum >= 163 && keynum <= 165)
			return keynum - 163 + '4';
		if (keynum >= 166 && keynum <= 168)
			return keynum - 166 + '1';
		if (keynum == 169)
			return '0';
	}
	if (GetKeyState(VK_SHIFT) < 0 && !bIgnoreShift)	// Shift key has been pressed
	{
		switch (keynum)
		{
		case '`':
			return '~';
		case '1':
			return '!';
		case '2':
			return '@';
		case '3':
			return '#';
		case '4':
			return '$';
		case '5':
			return '%';
		case '6':
			return '^';
		case '7':
			return '&';
		case '8':
			return '*';
		case '9':
			return '(';
		case '0':
			return ')';
		case '\\':
			return '|';
		case '[':
			return '{';
		case ']':
			return '}';
		case ';':
			return ':';
		case '\'':
			return '\"';
		case ',':
			return '<';
		case '.':
			return '>';
		case '/':
			return '?';
		default:
			break;
		}
	}
	return keynum;
}

void IN_Accumulate(void)
{
	if (g_iMenuActive) return;
	if (g_mgui_candraw) return;
	return gExportfuncs.IN_Accumulate();
}
void IN_ClearStates(void)
{
	return gExportfuncs.IN_ClearStates();
}
void IN_MouseEvent(int x)
{
	g_iMouseOldEvent = g_iMouseEvent;
	g_iMouseEvent = x;
	g_mgui_oldmouseevent = g_mgui_mouseevent;
	g_mgui_mouseevent = x;

	if (g_mgui_candraw || g_iMenuActive)
		g_flNextMouseEvent = cl.time + 0.3f;

	if (cl.time > g_flNextMouseEvent)
		return gExportfuncs.IN_MouseEvent(x);
	else
		return;
}

#if 1
int *cl_numvisedicts;

int cl_maxvisedicts = 512;
cl_entity_t **cl_visedicts_old;
cl_entity_t **cl_visedicts_new;

void CL_VisEdicts_Patch(void)
{
	cl_maxvisedicts = 2048;
	const char *s_num;
	if (g_pInterface->CommandLine->CheckParm("-visedicts", &s_num))
	{
		if (s_num && s_num[0] && s_num[0] >= '0' && s_num[0] <= '9')
		{
			int i_num = atoi(s_num);
			cl_maxvisedicts = min(max(i_num, 512), 4096);
		}
	}

	//search in CL_CreateVisibleEntity

	DWORD addr_cl_numvisedicts = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gEngfuncs.CL_CreateVisibleEntity, 0x50, "\x8B\x0D\x2A\x2A\x2A\x2A\x81\xF9\x00\x02\x00\x00", sizeof("\x8B\x0D\x2A\x2A\x2A\x2A\x81\xF9\x00\x02\x00\x00") - 1);

	if (!addr_cl_numvisedicts)
		Sys_ErrorEx("cl_numvisedicts not found");

	cl_numvisedicts = *(int **)(addr_cl_numvisedicts + 2);

	//search cl_visedicts

	DWORD addr_cl_visedicts = (DWORD)g_pMetaHookAPI->SearchPattern((void *)gEngfuncs.CL_CreateVisibleEntity, 0x100, "\xB8\x01\x00\x00\x00\x89\x14\x8D\x2A\x2A\x2A\x2A\x41", sizeof("\xB8\x01\x00\x00\x00\x89\x14\x8D\x2A\x2A\x2A\x2A\x41") - 1);

	if (!addr_cl_visedicts)
		Sys_ErrorEx("cl_visedicts not found");

	cl_visedicts_old = *(cl_entity_t ***)(addr_cl_visedicts + 8);

	//not replaced

	if (cl_maxvisedicts <= 512)
	{
		cl_visedicts_new = cl_visedicts_old;
		return;
	}

	//alloc here

	cl_visedicts_new = (cl_entity_t **)malloc(cl_maxvisedicts*sizeof(cl_entity_t *));

	if (!cl_visedicts_new)
	{
		Sys_ErrorEx("CL_VisEdicts_Patch: out of memory");
		return;
	}

	//replace this first

	g_pMetaHookAPI->WriteDWORD((void *)(cl_visedicts_old + 8), (DWORD)cl_visedicts_new);

	gEngfuncs.Con_DPrintf("CL_VisEdicts_Patch: CL_CreateVisibleEntity patched.\n");

	//replace all "cmp cl_numvisedicts, 200h"

	char sig[32];

	int count;
	DWORD addr, size, end;

	end = g_dwEngineBase + g_dwEngineSize;

	count = 0;
	memcpy(sig, "\x81\x3D\x2A\x2A\x2A\x2A\x00\x02\x00\x00", 10);
	*(DWORD *)(&sig[2]) = (DWORD)cl_numvisedicts;
	addr = g_dwEngineBase;
	while (1)
	{
		size = end - addr;
		addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)addr, size, sig, 10);
		if (addr == 0)
			break;
		g_pMetaHookAPI->WriteDWORD((void *)(addr + 6), (DWORD)cl_maxvisedicts);
		count++;
		addr += 10;
	}
	gEngfuncs.Con_DPrintf("CL_VisEdicts_Patch: %d of \"cmp cl_numvisedicts, 200h\" patched.\n", count);

	//patch ClientDLL_AddEntity

	memcpy(sig, "\x89\x34\x85\x2A\x2A\x2A\x2A\x40\xA3\x2A\x2A\x2A\x2A", 13);

	*(DWORD *)(&sig[3]) = (DWORD)cl_visedicts_old;
	*(DWORD *)(&sig[9]) = (DWORD)cl_numvisedicts;
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, 13);
	if (!addr)
		Sys_ErrorEx("ClientDLL_AddEntity->cl_visedicts");

	g_pMetaHookAPI->WriteDWORD((void *)(addr + 3), (DWORD)cl_visedicts_new);
	gEngfuncs.Con_DPrintf("CL_VisEdicts_Patch: ClientDLL_AddEntity patched.\n");

	//patch CL_MoveAiments

	memcpy(sig, "\x8B\x04\x95\x2A\x2A\x2A\x2A\x8B\x88\x44\x03\x00\x00", 13);

	*(DWORD *)(&sig[3]) = (DWORD)cl_visedicts_old;
	addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, sig, 13);
	if (!addr)
		Sys_ErrorEx("CL_MoveAiments->cl_visedicts");

	g_pMetaHookAPI->WriteDWORD((void *)(addr + 3), (DWORD)cl_visedicts_new);
	gEngfuncs.Con_DPrintf("CL_VisEdicts_Patch: CL_MoveAiments patched.\n");

	//patch R_DrawEntitiesOnList



	count = 0;
	memcpy(sig, "\x8B\x2A\x2A\x2A\x2A\x2A\x2A", 7);
	*(DWORD *)(&sig[3]) = (DWORD)cl_visedicts_old;
	
	addr = (DWORD)(g_dwEngineBase + 0x1D45420 - 0x1D01000);/*gRefFuncs.R_DrawEntitiesOnList;*/
	size = 0x350;
	end = size + addr;
	do
	{
		size = end - addr;
		addr = (DWORD)g_pMetaHookAPI->SearchPattern((void *)addr, size, sig, 7);
		if (!addr)
			break;
		g_pMetaHookAPI->WriteDWORD((void *)(addr + 3), (DWORD)cl_visedicts_new);
		count++;
		addr += 7;
	} while (1);

	gEngfuncs.Con_DPrintf("CL_VisEdicts_Patch: %d replaced in R_DrawEntitiesOnList\n", count);
}

#endif

void SVC_Init(void);
void GameUI_Init();
void EngFunc_SPR_Set(HSPRITE hSPR, int r, int g, int b);

TEMPENTITY *g_pPetrolboomerFlame = nullptr;
TEMPENTITY *g_pRunebladeEffect = nullptr;
TEMPENTITY *g_pDualSwordEffect1 = nullptr;
TEMPENTITY *g_pDualSwordEffect2 = nullptr;

int Initialize(struct cl_enginefuncs_s *pEnginefuncs, int iVersion)
{
	//	pEnginefuncs->pfnDrawCharacter = &DrawCharacter;

	memcpy(&gEngfuncs, pEnginefuncs, sizeof(gEngfuncs));
	memcpy(&gEventAPI, pEnginefuncs->pEventAPI, sizeof(event_api_s));
	memcpy(&gEfxAPI, pEnginefuncs->pEfxAPI, sizeof(efx_api_s));

	InitCrashHandle();
	INEIN_InstallHook();
	SVC_Init();
	PBO_Init();

	Tri_Extension_InstallHook();
	pEnginefuncs->pfnHookUserMsg = &EngFunc_HookUserMsg;
	pEnginefuncs->pfnSPR_DrawAdditive = &EngFunc_SPR_DrawAdditive;
	pEnginefuncs->pfnFillRGBA = &EngFunc_FillRGBA;
	pEnginefuncs->pfnSPR_Load = &Engfunc_SPR_Load;
	pEnginefuncs->pfnSPR_Set = &EngFunc_SPR_Set;
	pEnginefuncs->LoadMapSprite = &Engfunc_LoadMapSprite;
	pEnginefuncs->COM_LoadFile = &Engfunc_COM_LoadFile;
	pEnginefuncs->COM_ParseFile = &Engfunc_COM_ParseFile;
	pEnginefuncs->COM_FreeFile = &Engfunc_COM_FreeFile;
	pEnginefuncs->pfnAddCommand = &EngFunc_AddCommand;
	pEnginefuncs->pfnSPR_GetList = &EngFunc_SPR_GetList;
	pEnginefuncs->pEfxAPI->R_BloodSprite = &Engfunc_BloodSprite;
	pEnginefuncs->pEfxAPI->R_BloodStream = &Engfunc_BloodStream;
	//pEnginefuncs->pEventAPI->EV_PlaySound = &Engfunc_PlaySound;
	pEnginefuncs->pEventAPI->EV_WeaponAnimation = &Engfunc_WeaponAnim;
	//pEnginefuncs->pEfxAPI->Draw_DecalIndex = &EfxAPI_DecalIndex;
	//pEnginefuncs->pEfxAPI->Draw_DecalIndexFromName = &EfxAPI_DecalIndexFromName;
//	pEnginefuncs->pEfxAPI->R_BulletImpactParticles = &EfxAPI_BulletImpactParticles;
	pEnginefuncs->pfnClientCmd = &Engfunc_ClientCmd;
	//pEnginefuncs->pEventAPI->EV_PlayerTrace = &EventAPI_PlayerTrace;
	//pEnginefuncs->pEfxAPI->R_StreakSplash = &EfxAPI_R_StreakSplash;

	pEnginefuncs->pfnPlaySoundByName = &Engfunc_PlaySoundByName;

	pEnginefuncs->pfnClientCmd = EngFunc_ClCMD;

	//pEnginefuncs->pfnDrawConsoleString = &Engfunc_DrawConsoleString;
	//pEnginefuncs->pfnVGUI2DrawCharacter = &Engfunc_VGUI2DrawCharacter;

	MSG_Init();

	int iResult = gExportfuncs.Initialize(pEnginefuncs, iVersion);

	//GameUI_Init();

	return iResult;
}

void DrawTexture(int index, int iX, int iY, int iW, int iH, int alpha)
{
	Tri_Enable(GL_TEXTURE_2D);
	Tri_Enable(GL_BLEND);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tri_BindTexture(GL_TEXTURE_2D, index);
	gEngfuncs.pTriAPI->Color4ub(255, 255, 255, alpha);
	gEngfuncs.pTriAPI->Begin(TRI_QUADS);
	gEngfuncs.pTriAPI->TexCoord2f(0.01, 0.99);
	gEngfuncs.pTriAPI->Vertex3f(iX, iY + iH, 0);
	gEngfuncs.pTriAPI->TexCoord2f(0.99, 0.99);
	gEngfuncs.pTriAPI->Vertex3f(iX + iW, iY + iH, 0);
	gEngfuncs.pTriAPI->TexCoord2f(0.99, 0.01);
	gEngfuncs.pTriAPI->Vertex3f(iX + iW, iY, 0);
	gEngfuncs.pTriAPI->TexCoord2f(0.01, 0.01);
	gEngfuncs.pTriAPI->Vertex3f(iX, iY, 0);
	gEngfuncs.pTriAPI->End();
}

int __fastcall HudSniperScope_Draw(void *p, int i, float f)
{
	if (WeaponManager().GetCurWeapon().iSniperScopeType == 1 && 0.0 < Hud().m_FOV && Hud().m_FOV <= 40.0)
	{
		int iH = ScreenHeight / 592.0f * 256.0f;
		int iY = ScreenHeight / 2 - iH;
		int iX = ScreenWidth / 2 - iH;
		DrawTexture(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[0]].iTexture, iX, iY, iH, iH, 255);
		DrawTexture(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[1]].iTexture, ScreenWidth / 2, iY, iH, iH, 255);
		DrawTexture(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[2]].iTexture, iX, ScreenHeight / 2, iH, iH, 255);
		DrawTexture(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[3]].iTexture, ScreenWidth / 2, ScreenHeight / 2, iH, iH, 255);

		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + iH, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + iH, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);

		/*gEngfuncs.pfnFillRGBABlend(ScreenWidth/2,0,1,ScreenHeight,0,0,0,255);
		gEngfuncs.pfnFillRGBABlend(0,ScreenHeight/2+1,ScreenWidth,1,0,0,0,255);*/
		return g_pfnHudSniperScope_Draw(p, i, f);
	}
	if (WeaponManager().GetCurWeapon().iSniperScopeType == 2 && 0.0 < Hud().m_FOV && Hud().m_FOV <= 40.0)
	{
		int iH = ScreenHeight / 588.0f * 256.0f;
		int iY = ScreenHeight / 2 - iH;
		int iX = ScreenWidth / 2 - iH;
		DrawTexture(g_Texture[g_iSniperScope[0]].iTexture, iX, iY, iH, iH, 255);
		DrawTexture(g_Texture[g_iSniperScope[1]].iTexture, ScreenWidth / 2, iY, iH, iH, 255);
		DrawTexture(g_Texture[g_iSniperScope[2]].iTexture, iX, ScreenHeight / 2, iH, iH, 255);
		DrawTexture(g_Texture[g_iSniperScope[3]].iTexture, ScreenWidth / 2, ScreenHeight / 2, iH, iH, 255);

		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + iH, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + iH, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);

		/*gEngfuncs.pfnFillRGBABlend(ScreenWidth/2,0,1,ScreenHeight,0,0,0,255);
		gEngfuncs.pfnFillRGBABlend(0,ScreenHeight/2+1,ScreenWidth,1,0,0,0,255);*/
		return 0/*g_pfnHudSniperScope_Draw(p,i,f)*/;
	}
	if (pCvar_DrawScope->value != 0 && g_iBTEWeapon != WPN_SFSNIPER && g_iBTEWeapon != WPN_DESTROYER && g_iBTEWeapon != WPN_BUFFAWP && 0.0 < Hud().m_FOV && Hud().m_FOV <= 40.0)
	{
		//LogToFile("BreakPoint");
		int tid;
		float scale;
		float x, y, w, h;

		if (g_iBTEWeapon == WPN_M200)
			tid = Hud().m_TGA.GetTGA("other\\scope_m200");
		else
			tid = Hud().m_TGA.GetTGA("other\\scope");

		scale = ScreenHeight / 0.85 / g_MHTga[tid].height;

		w = g_MHTga[tid].width * scale;
		h = g_MHTga[tid].height * scale;

		x = ScreenWidth / 2 - w / 2;
		y = ScreenHeight / 2 - h / 2;

		Tri_Enable(GL_TEXTURE_2D);
		Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[tid].texid);
		if (g_iVideoMode)
		{
			glColor4ub(255, 255, 255, 255);
		}
		else gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		Tri_Enable(GL_BLEND);
		Tri_Enable(GL_ALPHA_TEST);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tri_AlphaFunc(GL_GREATER, 0.0);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(x, y + h, 0);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(x + w, y + h, 0);
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(x + w, y, 0);
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(x, y, 0);
		gEngfuncs.pTriAPI->End();

		int LENGTH_SCOPE = w / 2;

		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + LENGTH_SCOPE, 0, ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + LENGTH_SCOPE, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE, 0, 0, 0, 255);

		// Don't know why
		gEngfuncs.pfnDrawConsoleString(-10, -10, "!");

		return 0;
	}

	if (g_iBTEWeapon == WPN_SFSNIPER)
	{
		HudSfsniperScope().Draw(f);

		return 0;
	}

	if (g_iBTEWeapon == WPN_DESTROYER)
	{
		HudDestroyerSniperScope().Draw(f);

		return 0;
	}

	if (g_iBTEWeapon == WPN_BUFFAWP)
	{
		HudBuffAWPSniperScope().Draw(f);
	}

	return g_pfnHudSniperScope_Draw(p, i, f);
}

int(__fastcall *g_pfnCHudAmmo_DrawCrosshair)(void *, int, float flTime, int weaponid) = (int(__fastcall *)(void *, int, float, int))0x1940430;

int __fastcall CHudAmmo_DrawCrosshair(void *pthis, int, float flTime, int weaponid)
{
	return 1;
}
void Cmd_ShowPos()
{
	char text[256];
	// origin angle v_angle
	sprintf(text, "%f %f %f %f %f %f %f %f %f 0",
		g_vecOrigin[0], g_vecOrigin[1], g_vecOrigin[2],
		g_vecVAngles[0], g_vecVAngles[1], g_vecVAngles[2],
		v_angles[0], v_angles[1], v_angles[2]);

	gEngfuncs.Con_Printf("%s\n", text);

	if (OpenClipboard(NULL))
	{
		EmptyClipboard();
		HGLOBAL hClipboardData;
		hClipboardData = GlobalAlloc(GMEM_DDESHARE, strlen(text) + 1);
		char *pchData;
		pchData = (char*)GlobalLock(hClipboardData);
		strcpy(pchData, text);
		GlobalUnlock(hClipboardData);
		SetClipboardData(CF_TEXT, hClipboardData);
		CloseClipboard();
	}
}

void Cmd_ShowIcon()
{
	if (pCvar_TipIcon->value)
		gEngfuncs.Cvar_SetValue("mh_showtipicon", 0);
	else
		gEngfuncs.Cvar_SetValue("mh_showtipicon", 1);
}

void HUD_Init(void)
{
	//CL_VisEdicts_Patch();

	pCvar_TipIcon = gEngfuncs.pfnRegisterVariable("mh_showtipicon", "1", FCVAR_ARCHIVE);
	pCvar_Name = gEngfuncs.pfnRegisterVariable("mh_teammatename", "1", FCVAR_ARCHIVE);
	//pCvar_Blood = gEngfuncs.pfnRegisterVariable("mh_bloodcolor", "1", FCVAR_ARCHIVE);
	mh_scoreboard = gEngfuncs.pfnRegisterVariable("mh_scoreboard", "1", FCVAR_ARCHIVE);
	pCvar_DrawBink = gEngfuncs.pfnRegisterVariable("mh_drawbink", "1", FCVAR_ARCHIVE);
	pCvar_DrawPanel = gEngfuncs.pfnRegisterVariable("mh_drawpanel", "1", FCVAR_ARCHIVE);
	pCvar_DrawScope = gEngfuncs.pfnRegisterVariable("mh_drawscope", "1", FCVAR_ARCHIVE);
	//pCvar_ZombieMenu = gEngfuncs.pfnRegisterVariable("mh_zombiemenu", "1", FCVAR_ARCHIVE);

	cl_crosshair_type = gEngfuncs.pfnRegisterVariable("cl_crosshair_type", "0", FCVAR_ARCHIVE);
	cl_crosshair_hit = gEngfuncs.pfnRegisterVariable("cl_crosshair_hit", "0", FCVAR_ARCHIVE);

	cl_gunsmoke = gEngfuncs.pfnRegisterVariable("cl_gunsmoke", "1", FCVAR_ARCHIVE);
	cl_gunstreak = gEngfuncs.pfnRegisterVariable("cl_gunstreak", "1", FCVAR_ARCHIVE);

	hud_fastswitch_wheel = gEngfuncs.pfnRegisterVariable("hud_fastswitch_wheel", "1", FCVAR_ARCHIVE);

	cl_ghost_view_render = gEngfuncs.pfnRegisterVariable("cl_ghost_view_render", "0", FCVAR_ARCHIVE);

	gl_fxaa = gEngfuncs.pfnRegisterVariable("gl_fxaa", "0", FCVAR_ARCHIVE);
	//cl_menu_font = gEngfuncs.pfnRegisterVariable("cl_menu_font", "0", FCVAR_ARCHIVE);

	/*cl_righthand = gEngfuncs.pfnGetCvarPointer("cl_righthand");

	if (!cl_righthand)
	cl_righthand = gEngfuncs.pfnRegisterVariable("cl_righthand", "1", FCVAR_ARCHIVE);*/

	g_pcvarEnableConsole = gEngfuncs.pfnRegisterVariable("bte_enable_console", "1", FCVAR_ARCHIVE);

	Hud().Init();
	
	developer = gEngfuncs.pfnGetCvarPointer("developer");
	
	BTEPanel_Init();
	BTEBuyMenu_Init();
	g_TeamMenu.Init();
	g_NormalZombieMenu.Init();

	EV_HookEvents();
	UI_Init();
	gHud3D.Init();
	gHud3D_ZB.Init();
	gMuzzleflashManager.Init();

	g_iSniperScope[0] = Hud().m_TGA.FindTexture("sprites\\scope_arc_nw");
	g_iSniperScope[1] = Hud().m_TGA.FindTexture("sprites\\scope_arc_ne");
	g_iSniperScope[2] = Hud().m_TGA.FindTexture("sprites\\scope_arc_sw");
	g_iSniperScope[3] = Hud().m_TGA.FindTexture("sprites\\scope_arc");

	g_iScreenTexture = vgui::surface()->CreateNewTextureID();
	
	/*vgui::surface()->DrawSetTextureFile(g_iScreenTexture,"test",true,true);*/

	/*int g_iScreenTexture = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(g_iScreenTexture,"models\\texture\\#256256m2_p",true,true);*/

	g_pMetaHookAPI->InlineHook(g_pfnCHudAmmo_DrawCrosshair, CHudAmmo_DrawCrosshair, (void *&)g_pfnCHudAmmo_DrawCrosshair);
	g_pMetaHookAPI->InlineHook(g_pfnHudSniperScope_Draw, HudSniperScope_Draw, (void *&)g_pfnHudSniperScope_Draw);

	Weapon_Init();

	g_pMetaHookAPI->InlineHook(g_pfnCHudSayText_Draw, CHudSayText_Draw, (void *&)g_pfnCHudSayText_Draw);



	/*g_hCurrentFont = vgui::surface()->CreateFont();
	vgui::surface()->AddCustomFontFile("df_gb_y9.ttf");
	g_hCurrentFont = g_pSchemeManager->("a");*/

	gEngfuncs.pfnAddCommand("showpos", Cmd_ShowPos);
	gEngfuncs.pfnAddCommand("showicon", Cmd_ShowIcon);

	gExportfuncs.HUD_Init();

	cl_lw = gEngfuncs.pfnGetCvarPointer("cl_lw");
}

void Event_VidInit(void);

extern char g_szModelPrecache[512][MAX_QPATH];
extern int g_iModelPrecacheNums;

int HUD_VidInit(void)
{
	g_iMod = 0;
	g_iWeaponLimit = 0;
	g_bAlive = false;
	bLoadedSky = false;
	g_mgui_candraw = 0;
	g_fCountDown = 0;
	g_iHookVAngle = 0;
	g_iShowCustomCrosshair = 0;
	g_flNextMouseEvent = 0.0f;
	g_flNextSkillKeyPress = 0.0f;

	Hud().VidInit();

	WeaponManager().Reset();
	PlayerClassManager().Reset();

	DisplayInitialize();
	MessageInitialize();
	BTEPanel_TeamSuit_Reset();
	BTEPanel_BuyMenu_Reset();
	g_TeamMenu.Reset();
	g_NormalZombieMenu.Reset();
	
	gHud3D.VidInit();
	gHud3D_ZB.VidInit();

	Event_VidInit();

	g_fMuzzleTime = 0;
	key_last_press = 0.0;
	g_Next_Key_CanUse = 0.0;
	g_flZESFade = 0;
	g_iZBNvg = 0;

	memset(g_Entity_Index, 0, sizeof(g_Entity_Index));
	memset(g_szWeaponSkin, 0, sizeof(g_szWeaponSkin));
	memset(vPlayer, 0, sizeof(vPlayer));
	memset(g_PlayerExtraInfo, 0, sizeof(g_PlayerExtraInfo));
	memset(g_vecHostagePos, 0, sizeof(g_vecHostagePos));


	memset(g_szModelPrecache, 0, sizeof(g_szModelPrecache));
	g_iModelPrecacheNums = 0;

	cl_righthand = gEngfuncs.pfnGetCvarPointer("cl_righthand");

	cl_dynamiccrosshair = gEngfuncs.pfnGetCvarPointer("cl_dynamiccrosshair");
	cl_crosshair_color = gEngfuncs.pfnGetCvarPointer("cl_crosshair_color");
	cl_crosshair_size = gEngfuncs.pfnGetCvarPointer("cl_crosshair_size");
	cl_crosshair_translucent = gEngfuncs.pfnGetCvarPointer("cl_crosshair_translucent");

	tracerred = gEngfuncs.pfnGetCvarPointer("tracerred");
	tracergreen = gEngfuncs.pfnGetCvarPointer("tracergreen");
	tracerblue = gEngfuncs.pfnGetCvarPointer("tracerblue");
	traceralpha = gEngfuncs.pfnGetCvarPointer("traceralpha");
	tracerlength = gEngfuncs.pfnGetCvarPointer("tracerlength");
	traceroffset = gEngfuncs.pfnGetCvarPointer("traceroffset");
	tracerspeed = gEngfuncs.pfnGetCvarPointer("tracerspeed");

	viewmodel_widescreen = gEngfuncs.pfnGetCvarPointer("viewmodel_widescreen");

	gEngfuncs.pfnClientCmd("-duck;");

	Weapon_VidInit();

	return gExportfuncs.HUD_VidInit();
}
int HUD_UpdateClientData(client_data_t *pClientData, float flTime)
{
	return Hud().UpdateClientData(pClientData, flTime);
}

void HUD_TxferPredictionData(struct entity_state_s *ps, const struct entity_state_s *pps, struct clientdata_s *pcd, const struct clientdata_s *ppcd, struct weapon_data_s *wd, const struct weapon_data_s *pwd)
{
	gExportfuncs.HUD_TxferPredictionData(ps, pps, pcd, ppcd, wd, pwd);

	/*if (ps->effects & EF_DIMLIGHT)
	ps->effects &= ~EF_DIMLIGHT;*/
}

void HUD_TxferLocalOverrides(struct entity_state_s *state, const struct clientdata_s *client)
{
	gExportfuncs.HUD_TxferLocalOverrides(state, client);

	/*if (state->effects & EF_DIMLIGHT)
	state->effects &= ~EF_DIMLIGHT;*/
}

void HUD_ProcessPlayerState(struct entity_state_s *dst, const struct entity_state_s *src)
{
	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();

	if (dst->number == pPlayer->index)
	{
		//g_iTeamNumber = g_PlayerExtraInfo[pPlayer->index].teamnumber;
		g_iUser1 = src->iuser1;
		g_iUser2 = src->iuser2;
		g_iUser3 = src->iuser3;

		//g_iWeaponAnim = src->weaponanim;
	}

	if (src->modelindex != dst->modelindex)
	{
		model_t *pmodel = IEngineStudio.GetModelByIndex(src->modelindex);

		if (pmodel->name)
		{
			if (strlen(pmodel->name) > 17)
			{
				char name[64];
				strcpy(name, pmodel->name + 14);
				name[strlen(name) - 4] = 0;
				name[strlen(name) / 2] = 0;


				PlayerClassManager().SetPlayerClass(src->number, name);
				//g_iCustomClass[src->number] = FindClassData(name);
			}
		}
	}

	if (src->weaponmodel != dst->weaponmodel)
	{
		model_t *pweaponmodel = IEngineStudio.GetModelByIndex(src->weaponmodel);
		if (pweaponmodel->name)
		{
			char name[64];
			strcpy(name, pweaponmodel->name);

			if (strstr(name, "_2.mdl"))
				name[strlen(name) - 6] = 0;
			else
				name[strlen(name) - 4] = 0;

			static int iSlot;
			auto &Weapon = WeaponManager()[name + 9];
			if (Weapon)
			{
				if ((iSlot = Weapon.iSlot))
				{
					WeaponManager().SetPlayerWeapon(src->number, 0, Weapon);
					WeaponManager().SetPlayerWeapon(src->number, iSlot, Weapon);
				}
					//g_iCustomWeapon[src->number][0] = g_iCustomWeapon[src->number][iSlot] = iBteWpn;
			}
		}
	}

	gExportfuncs.HUD_ProcessPlayerState(dst, src);
}


int HUD_Redraw(float time, int intermission)
{
	HudNVG().Draw(time);
	
	int iResult = gExportfuncs.HUD_Redraw(time, intermission);
	
	DisplayRedraw(time, intermission);
	
	Hud().Redraw(time, intermission);

	MGUI_Redraw();

	Tri_Enable(GL_BLEND);
	Tri_Enable(GL_ALPHA_TEST);
	Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	Tri_AlphaFunc(GL_GREATER, 0.0);

	if (g_flTutorClose < time)
		UTIL_TutorClose();

	return iResult;
}

void HUD_DrawTransparentTriangles(void)
{
	gExportfuncs.HUD_DrawTransparentTriangles();

	if (HudOverview().CanDraw())
		HudOverview().EntityESP();
		
	//gEngfuncs.Con_Printf("%d\n", glGetError());
}

float g_skycolor[3];
#include "screenfade.h"
#include "shake.h"
//#include "BaseUI.h"

bool g_bNvg;

#include <list>
extern void(*g_pfnR_DrawSpriteModel)(cl_entity_t *);

void V_CalcRefdef(struct ref_params_s *pParams)
{
	if (g_iMod == MOD_ZSE)
	{
		screenfade_t sf;
		gEngfuncs.pfnGetScreenFade(&sf);

		if (g_iZBNvg)
		{
			if (!g_isZombie && !g_bAlive)
			{
				if (g_flZESFade == -1)
					g_flZESFade = cl.time;

				sf.fader = 110 * (cl.time - g_flZESFade) / 3.0;
				sf.fadeg = 0;
				sf.fadeb = 0;
				sf.fadealpha = 80 * (cl.time - g_flZESFade) / 3.0;
				sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

				HudZombieMod3().m_iNVG = 1;
			}
			else if (g_isZombie)
			{
				sf.fader = 110;
				sf.fadeg = 0;
				sf.fadeb = 0;
				sf.fadealpha = 80;
				sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

				HudZombieMod3().m_iNVG = 1;
			}
			else
			{
				sf.fader = 0;
				sf.fadeg = 0;
				sf.fadeb = 0;
				sf.fadealpha = 0;
				sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

				HudZombieMod3().m_iNVG = 0;
			}
		}
		else
		{
			sf.fader = 0;
			sf.fadeg = 0;
			sf.fadeb = 0;
			sf.fadealpha = 0;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			HudZombieMod3().m_iNVG = 0;
		}

		gEngfuncs.pfnSetScreenFade(&sf);
	}

	if (g_iMod == MOD_ZB4)
	{
		screenfade_t sf;
		gEngfuncs.pfnGetScreenFade(&sf);

		if (g_iTeam == 1 || !g_bAlive)
			g_iZ4Nvg = 0;

		switch (g_iZ4Nvg)
		{
		case 0:
		{
			sf.fader = 0;
			sf.fadeg = 0;
			sf.fadeb = 0;
			sf.fadealpha = 0;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			break;
		}
		case 1:
		{
			sf.fader = 85;
			sf.fadeg = 74;
			sf.fadeb = 131;
			sf.fadealpha = 80;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			break;
		}
		case 2:
		{
			sf.fader = 124;
			sf.fadeg = 66;
			sf.fadeb = 74;
			sf.fadealpha = 66;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			break;
		}
		case 3:
		{
			if (cl.time - g_flZ4Nvg > 10.0f)
			{
				g_iZ4Nvg = 2;
				break;
			}

			sf.fader = 85 + (124 - 85) * (cl.time - g_flZ4Nvg) / 10.0f;
			sf.fadeg = 74 + (66 - 74) * (cl.time - g_flZ4Nvg) / 10.0f;
			sf.fadeb = 131 + (47 - 131) * (cl.time - g_flZ4Nvg) / 10.0f;
			sf.fadealpha = 80 + (66 - 80) * (cl.time - g_flZ4Nvg) / 10.0f;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;


			break;
		}
		case 4:
		{
			if (cl.time > g_flZ4Nvg)
			{
				g_iZ4Nvg = 1;
				break;
			}

			sf.fader = 85 + (124 - 85) * (g_flZ4Nvg - cl.time) / 10.0f;
			sf.fadeg = 74 + (66 - 74) * (g_flZ4Nvg - cl.time) / 10.0f;
			sf.fadeb = 131 + (47 - 131) * (g_flZ4Nvg - cl.time) / 10.0f;
			sf.fadealpha = 80 + (66 - 80) * (g_flZ4Nvg - cl.time) / 10.0f;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			break;
		}
		}

		switch (g_iZ4Nvg2)
		{
		case 5:
		{
			sf.fader = 255;
			sf.fadeg = 0;
			sf.fadeb = 0;
			sf.fadealpha = 180;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			if (!g_vecVAngle[0] && !g_vecVAngle[1])
			{
				g_vecVAngle[0] = pParams->cl_viewangles[0];
				g_vecVAngle[1] = pParams->cl_viewangles[1];
				g_vecVAngle[2] = pParams->cl_viewangles[2];
			}

			pParams->cl_viewangles[0] = g_vecVAngle[0];
			pParams->cl_viewangles[1] = g_vecVAngle[1];
			pParams->cl_viewangles[2] = g_vecVAngle[2];

			if (cl.time >= g_flZ4Nvg2)
			{
				g_flZ4Nvg2 = g_flZ4Nvg2 + 1.0f;
				g_iZ4Nvg2 = 6;
				break;
			}

			break;
		}
		case 6:
		{
			if (cl.time > g_flZ4Nvg2)
			{
				g_iZ4Nvg2 = 0;
				break;
			}

			sf.fader = 255;
			sf.fadeg = 0;
			sf.fadeb = 0;
			sf.fadealpha = 180 * (g_flZ4Nvg2 - cl.time) / 1.0f;
			sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

			break;
		}
		}
		gEngfuncs.pfnSetScreenFade(&sf);
	}

	screenfade_t sf;
	gEngfuncs.pfnGetScreenFade(&sf);

	if (sf.fader != 0 || sf.fadeg != 0 || sf.fadeb != 0)
		g_bNvg = true;
	else
		g_bNvg = false;

	if (g_iHookVAngle && g_bAlive && g_iBTEWeapon == WPN_M2)
	{
		if (!g_vecVAngle[0] && !g_vecVAngle[1])
		{
			g_vecVAngle[0] = pParams->cl_viewangles[0];
			g_vecVAngle[1] = pParams->cl_viewangles[1];
			g_vecVAngle[2] = pParams->cl_viewangles[2];
		}

		if (pParams->cl_viewangles[0] > 40.0)
			pParams->cl_viewangles[0] = 40.0;
		if (pParams->cl_viewangles[0] < -40.0)
			pParams->cl_viewangles[0] = -40.0;

		if (g_vecVAngle[1] + 75.0 > 360 || g_vecVAngle[1] - 75.0 < 0)
		{
			if (g_vecVAngle[1] + 75.0 > 360)
			{
				if (pParams->cl_viewangles[1] > g_vecVAngle[1] + 75.0 - 360 && pParams->cl_viewangles[1] < g_vecVAngle[1] - 150.0)
					pParams->cl_viewangles[1] = g_vecVAngle[1] + 75.0 - 360;
				if (pParams->cl_viewangles[1] < g_vecVAngle[1] - 75.0 && pParams->cl_viewangles[1] > g_vecVAngle[1] - 150.0)
					pParams->cl_viewangles[1] = g_vecVAngle[1] - 75.0;
			}
			if (g_vecVAngle[1] - 75.0 < 0)
			{
				if (pParams->cl_viewangles[1] > g_vecVAngle[1] + 75.0 && pParams->cl_viewangles[1] < g_vecVAngle[1] + 150.0)
					pParams->cl_viewangles[1] = g_vecVAngle[1] + 75.0;
				if (pParams->cl_viewangles[1] < g_vecVAngle[1] - 75.0 + 360.0 && pParams->cl_viewangles[1] > g_vecVAngle[1] + 150.0)
					pParams->cl_viewangles[1] = g_vecVAngle[1] - 75.0 + 360.0;
			}
		}
		else
		{
			if (pParams->cl_viewangles[1] > g_vecVAngle[1] + 75.0)
				pParams->cl_viewangles[1] = g_vecVAngle[1] + 75.0;
			if (pParams->cl_viewangles[1] < g_vecVAngle[1] - 75.0)
				pParams->cl_viewangles[1] = g_vecVAngle[1] - 75.0;
		}

	}

	if (pParams->movevars->skycolor_b != 255.0 && pParams->movevars->skycolor_r != 255.0 && pParams->movevars->skycolor_g != 255.0)
	{
		g_skycolor[0] = pParams->movevars->skycolor_r;
		g_skycolor[1] = pParams->movevars->skycolor_g;
		g_skycolor[2] = pParams->movevars->skycolor_b;
	}

	if (HudZombieMod3().m_iNVG)
	{
		pParams->movevars->skycolor_b = pParams->movevars->skycolor_r = pParams->movevars->skycolor_g = 255.0;
	}
	else
	{
		pParams->movevars->skycolor_r = g_skycolor[0];
		pParams->movevars->skycolor_g = g_skycolor[1];
		pParams->movevars->skycolor_b = g_skycolor[2];
	}

	HudOverview().CalcRefdef(pParams);
	gExportfuncs.V_CalcRefdef(pParams);

	VectorCopy(pParams->viewangles, v_angles);
	VectorCopy(pParams->punchangle, punchangle);
	VectorCopy(pParams->vieworg, vieworg);

	static cl_entity_t *viewent;
	static model_t *pModel = NULL;
	viewent = gEngfuncs.GetViewModel();

	static int iClipOld = -1;

	static int id[10];

	if (!id[0])
	{
		for (int i = 0; i <= 9; i++)
		{
			char text[32];
			sprintf(text, "models\\texture\\number\\+%d", i);

			id[i] = vgui::surface()->CreateNewTextureID();
			vgui::surface()->DrawSetTextureFile(id[i], text, true, false);
		}

	}

	if (g_iWeaponData[g_iWeapon].iClip != iClipOld && WeaponManager().GetCurWeapon().iViewModelDisplay)
	{
		iClipOld = g_iWeaponData[g_iWeapon].iClip;

		if (viewent->model)
		{
			studiohdr_t	*pstudio;
			mstudiotexture_t	*ptexture;
			pstudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(viewent->model);
			if (!pstudio) return;
			ptexture = (mstudiotexture_t *)(((byte *)pstudio) + pstudio->textureindex);
			for (int i = 0; i < pstudio->numtextures; i++)
			{
				if (ptexture[i].name[0] == '+' && ptexture[i].name[1] == 'A')
					ptexture[i].index = id[iClipOld / 100];

				if (ptexture[i].name[0] == '+' && ptexture[i].name[1] == 'B')
					ptexture[i].index = id[(iClipOld % 100) / 10];

				if (ptexture[i].name[0] == '+' && ptexture[i].name[1] == 'C')
					ptexture[i].index = id[iClipOld % 10];
			}
		}

	}

	if (!g_bAlive && !g_iUser2)
	{
		if (g_pPetrolboomerFlame)
		{
			g_pPetrolboomerFlame->die = 0.0;
			g_pPetrolboomerFlame = NULL;
		}
		if (g_pRunebladeEffect)
		{
			g_pRunebladeEffect->die = 0.0;
			g_pRunebladeEffect = NULL;
		}
		if (g_pDualSwordEffect1)
		{
			g_pDualSwordEffect1->die = 0.0;
			g_pDualSwordEffect1 = NULL;
		}
		if (g_pDualSwordEffect2)
		{
			g_pDualSwordEffect2->die = 0.0;
			g_pDualSwordEffect2 = NULL;
		}
	}

	if (viewent)
	{
		//RebuildTattoo(pModel);

		if (viewent->model->name)
		{
			if (strstr(viewent->model->name, "v_petrolboomer"))
			{
				if (!g_pPetrolboomerFlame)
				{
					struct model_s *model;

					model = IEngineStudio.Mod_ForName("sprites/flame3.spr", 0);

					int frameCount = ModelFrameCount(model);

					g_pPetrolboomerFlame = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(viewent->attachment[1], model);

					g_pPetrolboomerFlame->entity.curstate.rendermode = kRenderTransAdd;
					g_pPetrolboomerFlame->entity.curstate.renderamt = 255;
					g_pPetrolboomerFlame->entity.curstate.renderfx = 0;
					g_pPetrolboomerFlame->entity.curstate.scale = 0.13;
					g_pPetrolboomerFlame->entity.curstate.framerate = 20;

					g_pPetrolboomerFlame->frameMax = frameCount;
					g_pPetrolboomerFlame->die = cl.time + 9999.0f;

					g_pPetrolboomerFlame->flags |= FTENT_PERSIST | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP;

					//Engfunc_Call_AddVisibleEntity(&(g_pPetrolboomerFlame->entity));
				}


			}
			else
			{
				if (g_pPetrolboomerFlame)
				{
					g_pPetrolboomerFlame->die = 0.0;
					g_pPetrolboomerFlame = NULL;
				}
			}

			if (strstr(viewent->model->name, "v_runeblade"))
			{
				if (!g_pRunebladeEffect)
				{
					struct model_s *model;

					model = IEngineStudio.Mod_ForName("sprites/runeblade_eye.spr", 0);

					int frameCount = ModelFrameCount(model);

					g_pRunebladeEffect = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(viewent->attachment[1], model);

					g_pRunebladeEffect->entity.curstate.rendermode = kRenderTransAdd;
					g_pRunebladeEffect->entity.curstate.renderamt = 255;
					g_pRunebladeEffect->entity.curstate.renderfx = 0;
					g_pRunebladeEffect->entity.curstate.scale = 0.072;
					g_pRunebladeEffect->entity.curstate.framerate = 20;

					g_pRunebladeEffect->frameMax = frameCount;
					g_pRunebladeEffect->die = cl.time + 9999.0f;

					g_pRunebladeEffect->flags |= FTENT_PERSIST | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP;

					Engfunc_Call_AddVisibleEntity(&(g_pRunebladeEffect->entity));
				}
				if (viewent->curstate.sequence == 0)
					g_pfnR_DrawSpriteModel(&(g_pRunebladeEffect->entity));

			}
			else
			{
				if (g_pRunebladeEffect)
				{
					g_pRunebladeEffect->die = 0.0;
					g_pRunebladeEffect = NULL;
				}
			}

			if (strstr(viewent->model->name, "v_dualsword"))
			{
				if (!g_pDualSwordEffect1)
				{
					struct model_s *model;

					model = IEngineStudio.Mod_ForName("sprites/ef_dualsword_left.spr", 0);

					int frameCount = ModelFrameCount(model);

					g_pDualSwordEffect1 = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(viewent->attachment[1], model);

					g_pDualSwordEffect1->entity.curstate.rendermode = kRenderTransAdd;
					g_pDualSwordEffect1->entity.curstate.renderamt = 255;
					g_pDualSwordEffect1->entity.curstate.renderfx = 0;
					g_pDualSwordEffect1->entity.curstate.scale = 0.036;
					g_pDualSwordEffect1->entity.curstate.framerate = 20;

					g_pDualSwordEffect1->frameMax = frameCount;
					g_pDualSwordEffect1->die = cl.time + 9999.0f;

					g_pDualSwordEffect1->flags |= FTENT_PERSIST | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP;

					Engfunc_Call_AddVisibleEntity(&(g_pDualSwordEffect1->entity));
				}

				if (!g_pDualSwordEffect2)
				{
					struct model_s *model;

					model = IEngineStudio.Mod_ForName("sprites/ef_dualsword_right.spr", 0);

					int frameCount = ModelFrameCount(model);

					g_pDualSwordEffect2 = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(viewent->attachment[0], model);

					g_pDualSwordEffect2->entity.curstate.rendermode = kRenderTransAdd;
					g_pDualSwordEffect2->entity.curstate.renderamt = 255;
					g_pDualSwordEffect2->entity.curstate.renderfx = 0;
					g_pDualSwordEffect2->entity.curstate.scale = 0.036;
					g_pDualSwordEffect2->entity.curstate.framerate = 20;

					g_pDualSwordEffect2->frameMax = frameCount;
					g_pDualSwordEffect2->die = cl.time + 9999.0f;

					g_pDualSwordEffect2->flags |= FTENT_PERSIST | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP;

					Engfunc_Call_AddVisibleEntity(&(g_pDualSwordEffect2->entity));
				}
				
				//g_pfnR_DrawSpriteModel(&(g_pDualSwordEffect1->entity));
				//g_pfnR_DrawSpriteModel(&(g_pDualSwordEffect2->entity));
			}
			else
			{
				if (g_pDualSwordEffect1)
				{
					g_pDualSwordEffect1->die = 0.0;
					g_pDualSwordEffect1 = nullptr;
				}
				if (g_pDualSwordEffect2)
				{
					g_pDualSwordEffect2->die = 0.0;
					g_pDualSwordEffect2 = nullptr;
				}
			}
		}

		if (viewent->model != pModel)
		{
			iClipOld = -1;
#if 0			
			static int lastid[32];
			static int lastflags[32];
			static int id;

			if (!id)
			{
				id = vgui::surface()->CreateNewTextureID();
				vgui::surface()->DrawSetTextureFile(id, "models\\texture\\#null", true, false);
			}

			if (pModel)
			{

				studiohdr_t	*pstudio;
				mstudiotexture_t	*ptexture;
				if (!pModel->name) return;
				pstudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(pModel);
				if (!pstudio) return;
				ptexture = (mstudiotexture_t *)(((byte *)pstudio) + pstudio->textureindex);
				for (int i = 0; i < pstudio->numtextures; i++)
				{
					if (lastid[i])
						ptexture[i].index = lastid[i];
				}
			}

			if (viewent->model)
			{
				studiohdr_t	*pstudio;
				mstudiotexture_t	*ptexture;
				pstudio = (studiohdr_t*)IEngineStudio.Mod_Extradata(viewent->model);
				if (!pstudio) return;
				ptexture = (mstudiotexture_t *)(((byte *)pstudio) + pstudio->textureindex);
				for (int i = 0; i < pstudio->numtextures; i++)
				{
					lastid[i] = ptexture[i].index;
					lastflags[i] = ptexture[i].flags;
					//if (ptexture[i].flags) continue;
					if (g_iMod == MOD_GHOST && (vPlayer[g_iUser2].team == 2 || (g_iTeam == 2 && g_bAlive)))
						ptexture[i].index = id/*g_Texture[Hud().m_TGA.FindTexture("models\\texture\\#null")].iTexture*/;
					//ptexture[i].height = ptexture[i].width = 256;
					//ptexture[i].flags = 0;
				}
			}
#endif
			//CheckViewEntity();

			pModel = viewent->model;

			if (!viewent->model->name)
				Event_VidInit();
			else if (!strstr(viewent->model->name, "v_sfpistol") || (!g_bAlive && !g_iUser2))
				Event_VidInit();

		}

		if (g_iViewEntityBody >= 0)
			viewent->curstate.body = g_iViewEntityBody;

		/*if (g_iViewEntityRenderMode >= 0)
		{
		viewent->curstate.rendermode = g_iViewEntityRenderMode;
		viewent->curstate.renderfx = g_iViewEntityRenderFX;
		viewent->curstate.renderamt = g_iViewEntityRenderAmout;
		viewent->curstate.rendercolor = g_byViewEntityRenderColor;
		}*/
		viewent->curstate.skin = g_iSetupSkin;

		//viewent->curstate.rendermode = g_iViewEntityRenderMode;

		if (g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB5)
		{
			cl_entity_t *local = gEngfuncs.GetLocalPlayer();

			if (local->curstate.rendermode == kRenderTransAlpha)
			{
				if (vPlayer[g_iUser2].team == 2 || (g_iTeam == 2 && g_bAlive))
				{
					int alpha = local->curstate.renderamt;

					g_iViewEntityRenderMode = kRenderTransAdd;
					g_iViewEntityRenderFX = kRenderFxNone;
					g_byViewEntityRenderColor.r = g_byViewEntityRenderColor.g = g_byViewEntityRenderColor.b = 255;
					g_iViewEntityRenderAmout = alpha;
				}
			}
			else
			{
				g_iViewEntityRenderMode = kRenderNormal;
				g_iViewEntityRenderFX = kRenderFxNone;
				g_iViewEntityRenderAmout = 0;
				g_byViewEntityRenderColor.r = g_byViewEntityRenderColor.g = g_byViewEntityRenderColor.b = 255;
			}
		}

		if (g_iMod == MOD_GHOST)
		{
			if (vPlayer[g_iUser2].team == 2 || (g_iTeam == 2 && g_bAlive))
			{
				//g_iViewEntityRenderAmout = (int)VectorLength(g_velocity) / 250.0 * 56.0;
				//g_velocity[2] = 0;

				cl_entity_t *local = gEngfuncs.GetLocalPlayer();
				int alpha = local->curstate.renderamt;

				if (cl_ghost_view_render->value == 0)
				{
					g_iViewEntityRenderMode = kRenderTransAlpha;
					g_iViewEntityRenderFX = kRenderFxGlowShell;

					g_byViewEntityRenderColor.r = g_byViewEntityRenderColor.g = g_byViewEntityRenderColor.b = alpha * 1.5;
					g_iViewEntityRenderAmout = 0;
				}
				else
				{
					g_iViewEntityRenderMode = kRenderTransAdd;
					g_iViewEntityRenderFX = kRenderFxNone;
					g_byViewEntityRenderColor.r = g_byViewEntityRenderColor.g = g_byViewEntityRenderColor.b = 255;
					g_iViewEntityRenderAmout = alpha;
				}
			}
			else
			{
				g_iViewEntityRenderMode = kRenderNormal;
				g_iViewEntityRenderFX = kRenderFxNone;
				g_iViewEntityRenderAmout = 0;
				g_byViewEntityRenderColor.r = g_byViewEntityRenderColor.g = g_byViewEntityRenderColor.b = 255;
			}
		}
	}

#if 0
	if (!bLoadedSky)
	{
		char levelname[128];
		strcpy(levelname, gEngfuncs.pfnGetLevelName() + 5);
		if (levelname && strstr(levelname, "ze_"))
		{
			Hud().m_ZombieEscape.LoadPoint(levelname);
		}

		char *suf[6] = { "rt", "bk", "lf", "ft", "up", "dn" };
		char sName[128];
		char skyname[64];
		sprintf(skyname, pParams->movevars->skyName);
		int index = 0;

		for (int i = 0; i<6; i++)
		{
			index = 5800 + i;
			sprintf(sName, "cstrike\\gfx\\high_env\\%s%s.tga", skyname, suf[i]);
			imagedata_s tTex;
			if (!Image_LoadImage(sName, &tTex, &index))
			{
				LogToFile("无法打开该地图的高清天空纹理，请使用正常纹理");
				LogToFile("如果下列文件存在 %s，将会应用新天空纹理贴图", sName);
				continue;
			}
			LogToFile("为该地图加载了高清天空纹理贴图[天空名称：%s]", skyname);
		}
		bLoadedSky = true;
	}
#endif

	if (gConfigs.bEnableNewHud)
		return;

	if (pParams->spectator || !HudOverview().CanDraw())
		return;

	if (!pParams->nextView)
	{
		pParams->nextView = 1;
		pParams->onlyClientDraw = false;
	}
	else
	{
		pParams->viewport[0] = pParams->viewport[1] = 0;
		pParams->viewport[2] = 200;
		pParams->viewport[3] = 200;
		pParams->nextView = 0;
		pParams->onlyClientDraw = true;
		pParams->viewangles[0] = 90;

		//pParams->vieworg[2] =  (10+1.2*HudOverview().m_pRect->value)*4.1f/HudOverview().m_flZoom;
	}
	HudOverview().m_iMaxClient = pParams->maxclients;
	HudOverview().m_bCanDraw = pParams->onlyClientDraw ? true : false;

	return;
}

int HUD_GetStudioModelInterface(int iVersion, struct r_studio_interface_s **ppStudioInterface, struct engine_studio_api_s *pEngineStudio)
{
	memcpy(&IEngineStudio, pEngineStudio, sizeof(IEngineStudio));

	int iResult = gExportfuncs.HUD_GetStudioModelInterface(iVersion, ppStudioInterface, pEngineStudio);

	gStudioInterface.StudioDrawModel = (*ppStudioInterface)->StudioDrawModel;
	(*ppStudioInterface)->StudioDrawModel = R_StudioDrawModel;

	gStudioFuncs.R_StudioCheckBBox = pEngineStudio->StudioCheckBBox;
	gStudioFuncs.R_StudioSetupLighting = pEngineStudio->StudioSetupLighting;
	StudioFuncs_InstallHook();

	Window_VidInit();
	
	//Hud().m_scrinfo.iSize = sizeof(Hud().m_scrinfo);
	//gEngfuncs.pfnGetScreenInfo(&Hud().m_scrinfo);
	
	//char *value;

	
	/*
	if (ScreenInfo().iHeight <= 600 && ScreenInfo().iWidth <= 800)
		if (!strcmp(gConfigs.szLanguage, "schinese"))
			MessageBoxA(NULL, "游戏正在不推荐的分辨率运行，请尝试使用更高分辨率。", "警告", MB_OK | MB_ICONWARNING);
	*/
	LogToFile("===初始化模型接口===");

	g_iVideoMode = (IEngineStudio.IsHardware() == 1);
	g_iVideoQuality;

	if (g_iVideoMode)
	{
		LogToFile("视频模式: OpenGL");
		char *p = (char *)glGetString(GL_VERSION);
		if (*p)
		{
			char s = *p;
			LogToFile("OpenGL 版本:%s", p);
			if (s == '1')
			{
				LogToFile("自动检测: 低质量");
				//MessageBox(NULL,"低版本OpenGL 可能会发生白色手臂或TGA变白 请更新驱动","视频信息",MB_OK);

				if (!strcmp(gConfigs.szLanguage, "schinese"))
					MessageBoxA(NULL, "低版本 OpenGL，可能会出现显示错误。\n请尝试更新显卡驱动。", "警告", MB_OK | MB_ICONWARNING);
				else
					MessageBoxA(NULL, "Old version OpenGL, please try update Graphics Driver.", "Warning", MB_OK | MB_ICONWARNING);


				g_iVideoQuality = 0;
			}
			else
			{
				LogToFile("自动检测: 高质量");
				g_iVideoQuality = 1;

			}
		}
	}
	else
	{
		LogToFile("视频模式: D3D");

		if (!strcmp(gConfigs.szLanguage, "schinese"))
			MessageBoxA(NULL, "正在以 D3D 模式运行, 仅支持 OpenGL 模式。\n请在启动器更改显示模式并重新启动游戏。", "错误", MB_ICONERROR);
		else
			MessageBoxA(NULL, "Running in D3D mode, only OpenGL mode is supported.\nPlease change renderer mode and restart game.", "Warning", MB_ICONERROR);

		exit(0);
	}
	//DrawTgaLoadList(); !!!
	MGUI_Init();
	CVAR_Init();
	

	g_Font.Init("font.ttf");
	g_FontBold.Init("font_bold.ttf");

	r_currententity = *(cl_entity_t ***)((DWORD)pEngineStudio->GetCurrentEntity + 0x1);

	//return gExportfuncs.HUD_GetStudioModelInterface(iVersion, ppStudioInterface, pEngineStudio);
	return iResult;
}
void HUD_PlayerMove(struct playermove_s *pPlayerMove, int iServer)
{
	VectorCopy(pPlayerMove->origin, g_vecOrigin);
	gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(g_vecEyePos);
	VectorCopy(pPlayerMove->angles, g_vecVAngles);
	VectorAdd(g_vecEyePos, pPlayerMove->origin, g_vecEyePos);
	VectorAdd(pPlayerMove->origin, pPlayerMove->view_ofs, g_vecEye);
	VectorCopy(pPlayerMove->velocity, g_velocity);
	return gExportfuncs.HUD_PlayerMove(pPlayerMove, iServer);
}
int(*Engfunc_Call_AddVisibleEntity)(struct cl_entity_s *pEntity);
#define AMPLITUDE 0.7
int Engfunc_AddVisibleEntity(struct cl_entity_s *pEntity)
{

	enum
	{
		MUZZLEFLASH_NORMAL = 1,
		MUZZLEFLASH_PPQQ,
		MUZZLEFLASH_GUITAR
	};

	/*
	if (strstr(pEntity->model->name, "zb_skill_headshot"))
	{
		cl_entity_t *pPlayer = NULL;
		pPlayer = gEngfuncs.GetEntityByIndex(pEntity->curstate.aiment);
		if (!pEntity) Engfunc_Call_AddVisibleEntity(pEntity);
		//if(vPlayer[pEntity->curstate.aiment].team==2)
		//pEntity->curstate.renderamt = 0;

		pEntity->origin[0] = pPlayer->origin[0];
		pEntity->origin[1] = pPlayer->origin[1];
		pEntity->origin[2] = pPlayer->origin[2];

		return Engfunc_Call_AddVisibleEntity(pEntity);
	}*/

	/*
	if (pEntity->curstate.scale < 0.3 && pEntity->model->name && strstr(pEntity->model->name, "flame3"))
	{
		cl_entity_t *pViewEntity = gEngfuncs.GetViewModel();
		if (!pEntity) Engfunc_Call_AddVisibleEntity(pEntity);

		pEntity->origin = pViewEntity->attachment[1];

		return Engfunc_Call_AddVisibleEntity(pEntity);
	}
	*/
	if (pEntity->curstate.iuser4 == MUZZLEFLASH_GUITAR)
	{
		float fStart;
		float fFrame;
		//float ff;
		fStart = pEntity->curstate.fuser4;
		//ff = cl.time - pEntity->curstate.fuser4;
		fFrame = (cl.time - fStart) * 10.0f;
		pEntity->curstate.frame = fFrame;
		float fRad = cl.time - fStart;
		float fMultiple = AMPLITUDE * sin(1.5f * M_PI * fRad);

		float vR[3];
		gEngfuncs.pfnAngleVectors(g_vecVAngles, NULL, vR, NULL);
		vR[0] *= fMultiple;
		vR[1] *= fMultiple;
		vR[2] *= fMultiple;
		VectorAdd(pEntity->origin, vR, pEntity->origin);
		pEntity->origin[2] = pEntity->origin[2] + 0.2f;


	}
	else if (pEntity->curstate.iuser4 == MUZZLEFLASH_PPQQ)
	{
		float fFrame;
		fFrame = pEntity->curstate.frame;

		if (pEntity->curstate.fuser3<cl.time)
		{
			pEntity->curstate.fuser3 = cl.time + 0.02f;
			fFrame += 1.0f;
		}
		pEntity->curstate.frame = fFrame;
		static cl_entity_s *pModel;
		pModel = gEngfuncs.GetViewModel();
		if (pModel)
		{
			pEntity->origin[0] = pModel->attachment[pEntity->curstate.iuser3][0];
			pEntity->origin[1] = pModel->attachment[pEntity->curstate.iuser3][1];
			pEntity->origin[2] = pModel->attachment[pEntity->curstate.iuser3][2];
		}
	}
	return Engfunc_Call_AddVisibleEntity(pEntity);
}
int ModelFrameCount(model_t *mod)
{
	if (!mod || !mod->cache.data)
		return 0;

	if (mod->type == mod_sprite)
	{
		return ((msprite_t *)mod->cache.data)->numframes;
	}
	else
		gEngfuncs.Con_DPrintf("ModelFrameCount nonsupport type:%d\n", mod->type);

	return 0;
}

void MuzzleflashCallback(struct tempent_s *ent, float frametime, float currenttime)
{
	if (ent->entity.curstate.iuser2)
	{
		cl_entity_s *pModel;
		pModel = gEngfuncs.GetViewModel();
		if (pModel)
		{
			ent->entity.origin[0] = pModel->attachment[ent->entity.curstate.iuser4][0];
			ent->entity.origin[1] = pModel->attachment[ent->entity.curstate.iuser4][1];
			ent->entity.origin[2] = pModel->attachment[ent->entity.curstate.iuser4][2];
		}
	}

	if (!ent->entity.curstate.iuser1)
	{
		if ((cl.time - ent->entity.curstate.fuser1) > (1.0 / ent->entity.curstate.framerate))
		{
			if (ent->entity.curstate.frame + 1 > ent->frameMax)
				ent->entity.curstate.frame = 0;
			else
				ent->entity.curstate.frame += 1;

			ent->entity.curstate.fuser1 = cl.time;
		}
	}
	else
	{
		if (ent->entity.curstate.frame < ent->frameMax)
			ent->entity.curstate.frame++;
		else
			ent->die = currenttime + 0.01;
	}
}

extern void SmokeRise_Wind(tempent_s *pEntity, float frametime, float currenttime);

void HUD_StudioEvent(const struct mstudioevent_s *pEvent, const struct cl_entity_s *pEntity)
{
	if (pEvent && pEntity)
	{
		if (pEntity == gEngfuncs.GetViewModel() && Hud().m_FOV <= 40.0)
			return;

		if (pEvent->event == 5004 && strstr(pEntity->model->name, "thanatos9"))
		{
			if (pEntity->curstate.sequence == 9 || pEntity->curstate.sequence == 10)
			{
				TEMPENTITY *pEnt;
				float vecOrigin[3];
				pEntity->attachment[RANDOM_LONG(0, 3)].CopyToArray(vecOrigin);
				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(vecOrigin, IEngineStudio.Mod_ForName("sprites/smoke_thanatos9.spr", false));
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 15.0;
				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 150;
				pEnt->entity.curstate.scale = RANDOM_FLOAT(0.3, 0.4);
				pEnt->entity.baseline.origin = Vector(0,0,50);

				pEnt->die = cl.time + 0.57;
				pEnt->callback = SmokeRise_Wind; /*bWind ? SmokeRise_Wind : SmokeRise;*/
				pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PERSIST | FTENT_SPRANIMATELOOP;
			}
			
		}

		if (pEvent->event != 5001 && pEvent->event != 5011 && pEvent->event != 5021 && pEvent->event != 5031)
			return gExportfuncs.HUD_StudioEvent(pEvent, pEntity);
		int iAttachment = pEvent->event - 5000 - 1;
		iAttachment = iAttachment / 10;

		if (gConfigs.bEnableFireDlight && !g_bNvg)
		{
			dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
			VectorCopy(pEntity->attachment[iAttachment], dl->origin);

			if (strstr(pEntity->model->name, "plasma"))
			{
				dl->radius = 150;
				dl->color.r = 172;
				dl->color.g = 249;
				dl->color.b = 172;
				dl->decay = 300;
				dl->die = cl.time + 0.25;
			}
			else if (strstr(pEntity->model->name, "sfsniper"))
			{
				dl->radius = 200;
				dl->color.r = 158;
				dl->color.g = 220;
				dl->color.b = 235;
				dl->decay = 150;
				dl->die = cl.time + 0.75;
			}
			else
			{
				dl->radius = 100;
				dl->color.r = 255;
				dl->color.g = 249;
				dl->color.b = 146;
				dl->decay = 100 / 0.25;
				dl->die = cl.time + 0.25;
			}
		}

		if (pEvent->options[0] == '#')
		{
			int iMuzz/*, iAttachment*/, iFrameS, bA;
			float fScale, fDie, fR;

			bool isCSO = true;

			char options[64];
			strcpy(options, pEvent->options);
			int len = strlen(options);
			options[len] = ' ';
			len += 1;
			for (int i = 1; i<len; i++)
			{
				char result[8], *ptr;
				memset(result, 0, sizeof(result));
				switch (options[i])
				{
				case 'I':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					iMuzz = atoi(result);
					break;
				}
				case 'R':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					fR = atof(result);
					break;
				}
				case 'S':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					fScale = atof(result);
					break;
				}
				case 'T':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					fDie = atof(result);
					break;
				}
				case 'A':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					iAttachment = atoi(result);
					break;
				}
				case '@':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					iFrameS = atoi(result);
					isCSO = false;
					break;
				}
				case '&':
				{
					ptr = strstr(options + i, " ");
					int position = ptr - options;
					strncpy(result, options + i + 1, position - i - 1);
					bA = atoi(result);
					isCSO = false;
					break;
				}
				}
			}
			struct model_s *pModel;
			TEMPENTITY *pEnt;

			char muzz[32];
			sprintf(muzz, "sprites/muzzleflash%d.spr", iMuzz);
			pModel = IEngineStudio.Mod_ForName(muzz, 0);
			if (pModel == NULL)
			{
				gEngfuncs.pfnConsolePrint(va("[BTE] MuzzleFlash: %s NOT FOUND", muzz));
				return;
			}
			int frameCount = ModelFrameCount(pModel);

			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh((float *)&pEntity->attachment[iAttachment - 1], pModel);

			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 255;
			pEnt->entity.curstate.renderfx = 0;
			pEnt->entity.curstate.scale = fScale;
			pEnt->entity.curstate.rendercolor.r = -1;
			pEnt->entity.curstate.rendercolor.g = -1;
			pEnt->entity.curstate.rendercolor.b = -1;
			pEnt->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, frameCount - 1);
			pEnt->entity.angles[0] = 0;
			pEnt->entity.angles[1] = 0;
			//pEnt->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 1) ? 359.9 - gEngfuncs.pfnRandomLong(0, iP/2) : gEngfuncs.pfnRandomLong(0, iP/2);
			if (fR == 2)
				pEnt->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 360);
			else if (fR == 1.5)
				pEnt->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 1) ? 359.9 - gEngfuncs.pfnRandomLong(0, 15) : gEngfuncs.pfnRandomLong(0, 15);
			else
				pEnt->entity.angles[2] = 0;

			pEnt->die = cl.time + fDie;
			pEnt->frameMax = frameCount;

			if (fDie > 0.01)
			{
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = iFrameS;
				pEnt->flags |= FTENT_CLIENTCUSTOM;
				pEnt->callback = MuzzleflashCallback;
				pEnt->frameMax = frameCount - 1;
				pEnt->entity.curstate.iuser1 = isCSO;
				pEnt->entity.curstate.iuser4 = iAttachment - 1;
				pEnt->entity.curstate.iuser3 = bA;
				pEnt->entity.curstate.fuser1 = cl.time;
			}

			Engfunc_Call_AddVisibleEntity(&(pEnt->entity));

			return;

		}
		else/* if(strlen(pEvent->options) > 2)*/
		{
			int type = atoi(pEvent->options);

			int v4 = 10 * type / 100000;
			int v5 = type % 100000;

			if (v5 >= 10000)
				v5 %= 10000;

			int extra = v5 / 1000;
			int v6 = v5 % 1000;
			int v7 = v6 % 100;
			bool v8 = (v6 / 100) == 1;

			int index = v4 + v7 % 10;

			if (index == 3)
				index = 0;
			else if (index > 3)
				index--;

			float scale = (v7 / 10) * 0.1;

			if (scale == 0)
				scale = 0.5;

			if (extra == 0)
			{
				if (gEngfuncs.GetViewModel() == pEntity)
				{
					if (gConfigs.bEnableNewMuzzle)
					{
						if (gMuzzleflashReplaceManager.Add(pEntity->model->name + 7, index, iAttachment, 50 * scale) == 0)
						{
							if (index == 0 || index == 1)
							{
								gMuzzleflashManager.Add(0, gEngfuncs.GetAbsoluteTime(), iAttachment, 50 * scale);
								return;
							}
							else if (index == 2)
							{
								gMuzzleflashManager.Add(2, gEngfuncs.GetAbsoluteTime(), iAttachment, 50 * scale);
								return;
							}
						}
						else
						{
							return;
						}
					}
				}

				static char muzz[32];
				sprintf(muzz, "sprites/muzzleflash%d.spr", index + 1);

				struct model_s *pModel = IEngineStudio.Mod_ForName(muzz, 0);
				if (pModel == NULL)
				{
					gEngfuncs.pfnConsolePrint(va("[BTE] MuzzleFlash: %s NOT FOUND", muzz));
					return;
				}
				float pos[3];
				pos[0] = pEntity->attachment[iAttachment][0];
				pos[1] = pEntity->attachment[iAttachment][1];
				pos[2] = pEntity->attachment[iAttachment][2];

				int frameCount = ModelFrameCount(pModel);
				TEMPENTITY *pTemp = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(pos, pModel);

				if (!pTemp)
					return;

				pTemp->entity.curstate.rendermode = kRenderTransAdd;
				pTemp->entity.curstate.renderamt = 255;
				pTemp->entity.curstate.scale = scale;
				pTemp->entity.curstate.renderfx = kRenderFxNone;
				pTemp->entity.curstate.rendercolor.r = -1;
				pTemp->entity.curstate.rendercolor.g = -1;
				pTemp->entity.curstate.rendercolor.b = -1;
				pTemp->entity.origin[0] = pos[0];
				pTemp->entity.origin[1] = pos[1];
				pTemp->entity.origin[2] = pos[2];
				pTemp->die = cl.time + 0.01;
				pTemp->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, frameCount - 1);
				pTemp->entity.angles[0] = 0;
				pTemp->entity.angles[1] = 0;
				pTemp->entity.angles[2] = 0;
				pTemp->frameMax = frameCount;

				if (!v8)
					pTemp->entity.angles[2] = gEngfuncs.pfnRandomLong(0, index ? 359 : 20);

				Engfunc_Call_AddVisibleEntity(&(pTemp->entity));
				
			}
			return;
		}
	}
	gExportfuncs.HUD_StudioEvent(pEvent, pEntity);
}

void CL_CreateMove(float frametime, struct usercmd_s *cmd, int active)
{
	gExportfuncs.CL_CreateMove(frametime, cmd, active);

	if (HudTABBoard().GetCanDraw() && pCvar_DrawPanel->value)
		cmd->buttons |= IN_SCORE;

	return;
}

int g_iViewModel;

void HUD_WeaponsPostThink(local_state_s *from, local_state_s *to, usercmd_t *cmd, double time, unsigned int random_seed)
{
	static cl_entity_t *player;
	player = gEngfuncs.GetLocalPlayer();
	if (g_iCurrentWeapon != to->client.m_iId && player)
	{
		gEngfuncs.pEventAPI->EV_KillEvents(player->index, "events/blockar.sc");
		gEngfuncs.pEventAPI->EV_KillEvents(player->index, "events/blocksmg.sc");
	}

	if (player && (g_iCurrentWeapon != to->client.m_iId || g_iViewModel != to->client.viewmodel ))
	{
		CheckViewEntity();
	}

	g_iCurrentWeapon = to->client.m_iId;
	g_iViewModel = to->client.viewmodel;


	g_iPlayerFlags = from->client.flags;
	//g_flPlayerSpeed = Length(from->client.velocity);
	if (cl_lw && cl_lw->value)
	{
		g_flPlayerSpeed = from->client.velocity.Length();
	}
	g_iWeaponFlags = (int)from->client.vuser4[0];
	//g_iFreezeTimeOver = from->client.iuser3 & IUSER3_FREEZETIMEOVER;
}

int g_irunninggausspred;
float g_flApplyVel;

void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
	HUD_WeaponsPostThink(from, to, cmd, time, random_seed);
	//gEngfuncs.Con_Printf("%d\n", to->weapondata[g_iCurrentWeapon].m_fInReload);
	g_bInReload = to->weapondata[g_iCurrentWeapon].m_fInReload;
	//g_iWeaponStat = to->weapondata[g_iCurrentWeapon].m_iWeaponState;
	//g_iCurrentWeapon = from->client.m_iId;
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay != 1 && g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay != 5 && g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay != 6)
		g_iShowCustomCrosshair = 0;

	static cl_entity_t *viewmodel;
	viewmodel = gEngfuncs.GetViewModel();

	if (cmd)
	{
		if (cmd->buttons & IN_ATTACK2)
		{
			g_iButton = 1;
			if (g_iBTEWeapon == WPN_GAUSS)
				g_irunninggausspred = true;
		}
		else g_iButton = 0;

		cmd->buttons &= ~IN_RELOAD;
	}

	gExportfuncs.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);

	if (g_irunninggausspred == 1)
	{
		Vector forward;
		gEngfuncs.pfnAngleVectors(v_angles, forward, NULL, NULL);
		to->client.velocity = to->client.velocity - forward * g_flApplyVel * 5;
		g_irunninggausspred = false;
	}
}

void EngFunc_TempEntPlaySound(struct tempent_s *pTemp, float damp)
{
	float	fvol;
	char	soundname[32];
	qboolean	isshellcasing = false;
	int	zvel;

	if (pTemp == NULL)
		return;

	if (pTemp->entity.model && 
		Q_stristr(pTemp->entity.model->name, "block_shell") &&
			(
				pTemp->hitSound == BOUNCE_SHELL ||
				pTemp->hitSound == BOUNCE_SHOTSHELL
			)
		)
	{
		Q_snprintf(soundname, sizeof(soundname), "weapons/blockar1_shell%i.wav", gEngfuncs.pfnRandomLong(1, 3));
		isshellcasing = true; // shell casings have different playback parameters
	}
	else
	{
		switch (pTemp->hitSound)
		{
		case BOUNCE_GLASS:
			Q_snprintf(soundname, sizeof(soundname), "debris/glass%i.wav", gEngfuncs.pfnRandomLong(1, 4));
			break;
		case BOUNCE_METAL:
			Q_snprintf(soundname, sizeof(soundname), "debris/metal%i.wav", gEngfuncs.pfnRandomLong(1, 6));
			break;
		case BOUNCE_FLESH:
			Q_snprintf(soundname, sizeof(soundname), "debris/flesh%i.wav", gEngfuncs.pfnRandomLong(1, 7));
			break;
		case BOUNCE_WOOD:
			Q_snprintf(soundname, sizeof(soundname), "debris/wood%i.wav", gEngfuncs.pfnRandomLong(1, 4));
			break;
		case BOUNCE_SHRAP:
			Q_snprintf(soundname, sizeof(soundname), "weapons/ric%i.wav", gEngfuncs.pfnRandomLong(1, 5));
			break;
		case BOUNCE_SHOTSHELL:
			Q_snprintf(soundname, sizeof(soundname), "weapons/sshell%i.wav", gEngfuncs.pfnRandomLong(1, 3));
			isshellcasing = true; // shell casings have different playback parameters
			break;
		case BOUNCE_SHELL:
			Q_snprintf(soundname, sizeof(soundname), "player/pl_shell%i.wav", gEngfuncs.pfnRandomLong(1, 3));
			isshellcasing = true; // shell casings have different playback parameters
			break;
		case BOUNCE_CONCRETE:
			Q_snprintf(soundname, sizeof(soundname), "debris/concrete%i.wav", gEngfuncs.pfnRandomLong(1, 3));
			break;
		default:	// null sound
			return;
		}
	}

	zvel = abs(pTemp->entity.baseline.origin[2]);

	// only play one out of every n
	if (isshellcasing)
	{
		// play first bounce, then 1 out of 3		
		if (zvel < 200 && gEngfuncs.pfnRandomLong(0, 3))
			return;
	}
	else
	{
		if (gEngfuncs.pfnRandomLong(0, 5))
			return;
	}

	fvol = 1.0f;

	if (damp > 0.0f)
	{
		int	pitch;

		if (isshellcasing)
			fvol *= min(1.0f, ((float)zvel) / 350.0f);
		else fvol *= min(1.0f, ((float)zvel) / 450.0f);

		if (!gEngfuncs.pfnRandomLong(0, 3) && !isshellcasing)
			pitch = gEngfuncs.pfnRandomLong(95, 105);
		else pitch = PITCH_NORM;

		gEngfuncs.pEventAPI->EV_PlaySound(-1, pTemp->entity.origin, CHAN_BODY, soundname, fvol, ATTN_NORM, 0, pitch);
	}
}


void HUD_TempEntUpdate(double dblFrameTime, double dblClientTime, double dblGravity, struct tempent_s **ppTempEntFree, struct tempent_s **ppTempEntActive, int(*pfnAddVisibleEntity)(struct cl_entity_s *pEntity), void(*pfnTempEntPlaySound)(struct tempent_s *pTemp, float flDamp))
{
	Engfunc_Call_AddVisibleEntity = pfnAddVisibleEntity;
	pfnAddVisibleEntity = &Engfunc_AddVisibleEntity;
	pfnTempEntPlaySound = &EngFunc_TempEntPlaySound;
	gExportfuncs.HUD_TempEntUpdate(dblFrameTime, dblClientTime, dblGravity, ppTempEntFree, ppTempEntActive, pfnAddVisibleEntity, pfnTempEntPlaySound);
}

int HUD_AddEntity(int iType, struct cl_entity_s *pEntity, const char *pszModel)
{

	if (pEntity->player)
	{
		int index = pEntity->index;
		vPlayer[index].info = pEntity;
		if (!(g_PlayerExtraInfo[index].iFlag &SCOREATTRIB_DEAD))
			vPlayer[index].killtime = cl.time + 1.0;

	}
	else if (pszModel && Q_stristr(pszModel, "backpack"))
	{
		//g_iBombEntity = pEntity->index;
		//g_fBombEntityTime = cl.time + 0.5f;
		HudOverview().SetBombStatus(pEntity->index, cl.time + 0.5f);
	}
	else if (pszModel && Q_stristr(pszModel, "supply"))
	{
		pEntity->curstate.fuser1 = cl.time + 0.1f;
		pEntity->curstate.iuser1 = 1;
	}
	else if (pszModel && Q_stristr(pszModel, "zombitrap"))
	{
		pEntity->curstate.fuser1 = cl.time + 0.1f;
		pEntity->curstate.iuser1 = 2;
	}
	else if (pszModel && Q_stristr(pszModel, "hostage"))
	{
		pEntity->curstate.iuser1 = 1213;
	}
	else if (pEntity)
	{
		if (pEntity->model->name)
		{
			/*
			if (Q_stristr(pEntity->model->name, "/w_"))
			{
				if (g_Entity_Index[pEntity->index] < cl.time && !(IS_ZOMBIE_MODE && vPlayer[gEngfuncs.GetLocalPlayer()->index].team == 2) && !(pEntity->curstate.velocity.Length()))
				{
					TEMPENTITY *pEffect;
					pEffect = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(pEntity->origin, IEngineStudio.Mod_ForName("models/ef_gundrop.mdl", false));
					pEffect->entity.curstate.frame = 0;
					pEffect->entity.curstate.framerate = 1;
					pEffect->entity.baseline.iuser1 = pEntity->index;
					pEffect->die = cl.time + 5.0;
					pEffect->callback = GunDropEffect_FollowThink;
					
					pEffect->clientIndex = pEntity->index;
					pEffect->flags |= FTENT_PLYRATTACHMENT | FTENT_CLIENTCUSTOM;

					g_Entity_Index[pEntity->index] = cl.time + 5.0;
				}
			}
			*/
			if (Q_stristr(pEntity->model->name, "s_grenade_spark"))
			{
				if (g_Entity_Index[pEntity->index] < cl.time)
				{
					int iIndex;
					gEngfuncs.CL_LoadModel("sprites/smoke_ia.spr", &iIndex);
					g_Entity_Index[pEntity->index] = cl.time + 0.05f;
					gEngfuncs.pEfxAPI->R_SparkShower(pEntity->origin);
				}
			}
			if (Q_stristr(pEntity->model->name, "rpg7_rocket"))
			{
				if (g_Entity_Index[pEntity->index] < cl.time)
				{
					g_Entity_Index[pEntity->index] = cl.time + 0.015f;

					Vector origin = pEntity->origin;
					model_s *pModel;
					TEMPENTITY *pEnt;

					if (1/*gEngfuncs.PM_PointContents(origin, NULL) != CONTENTS_WATER*/)
					{
						pModel = IEngineStudio.Mod_ForName("sprites/smoke_ia.spr", 0);

						origin[2] += gEngfuncs.pfnRandomFloat(-7, 7);

						pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
						pEnt->entity.curstate.rendermode = kRenderTransAdd;
						pEnt->entity.curstate.renderamt = 90;
						pEnt->entity.curstate.scale = gEngfuncs.pfnRandomFloat(0.10, 0.45);
						pEnt->flags |= FTENT_SPRANIMATE;
						pEnt->entity.curstate.frame = 3.0;
						pEnt->frameMax = ModelFrameCount(pModel);

						pEnt->entity.curstate.framerate = gEngfuncs.pfnRandomFloat(3.0, 4.0);
						pEnt->die = cl.time + pEnt->frameMax / pEnt->entity.curstate.framerate;
					}
					

					pModel = IEngineStudio.Mod_ForName("sprites/hotglow.spr", 0);
					pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
					pEnt->entity.curstate.rendermode = kRenderTransAdd;
					pEnt->entity.curstate.renderamt = 200;
					pEnt->entity.curstate.scale = 1.0;
					pEnt->flags |= FTENT_PLYRATTACHMENT | FTENT_COLLIDEKILL | FTENT_COLLIDEWORLD;
					pEnt->clientIndex = pEntity->index;
					pEnt->entity.curstate.frame = 0.0;
					pEnt->die = cl.time + 0.02;

					dlight_t *dl = gEfxAPI.CL_AllocDlight(0);
					VectorCopy(origin, dl->origin);
					dl->radius = 200;
					dl->color.r = 255;
					dl->color.g = 249;
					dl->color.b = 146;
					dl->die = cl.time + 0.015;
					dl->decay = dl->radius / 0.015;
				}
			}
			else if (Q_stristr(pEntity->model->name, "ef_dragontail") && pEntity->curstate.owner == gEngfuncs.GetLocalPlayer()->index)
			{
				Vector vecForw;
				gEngfuncs.pfnAngleVectors(g_vecAngles, vecForw, NULL, NULL);
				pEntity->origin[0] = (g_vecEye[0] + vecForw[0] * 5.0f);
				pEntity->origin[1] = (g_vecEye[1] + vecForw[1] * 5.0f);
				pEntity->origin[2] = (g_vecEye[2] + vecForw[2] * 5.0f);
			}
			/*else if(strstr(pEntity->model->name,"spear.mdl") && !pEntity->curstate.iuser4)
			{
			int iIndex;
			gEngfuncs.CL_LoadModel("sprites/smoke.spr", &iIndex);
			gEfxAPI.R_BeamFollow(pEntity->index, iIndex, 1.0, 0.1, 155, 255, 218, 100);
			pEntity->curstate.iuser4 = 1;
			}*/


			/*if(strstr(pEntity->model->name,"ef_dragonsword")&&pEntity->curstate.owner == gEngfuncs.GetLocalPlayer()->index)
			{
			vec3_t vecForw;
			gEngfuncs.pfnAngleVectors(g_vecAngles,vecForw,NULL,NULL);
			pEntity->origin = (g_vecEye + vecForw*10.0f);
			}*/
			/*if(strstr(pEntity->model->name,"ef_dragontail")&&pEntity->curstate.owner == gEngfuncs.GetLocalPlayer()->index)
			{
			vec3_t vecForw;
			gEngfuncs.pfnAngleVectors(g_vecAngles,vecForw,NULL,NULL);
			pEntity->origin = (g_vecEye + vecForw*10.0f);
			}*/
		}
		/*if(strstr(pEntity->model->name,"svdex_smoke"))
		{
		cl_entity_t *viewent;
		viewent = gEngfuncs.GetViewModel();
		if (viewent && viewent->model->name && strstr(viewent->model->name,"bte_wpn"))
		{
		if(strstr(viewent->model->name,"svdex"))
		pEntity->curstate.frame += 0.2;
		pEntity->origin = gEngfuncs.GetViewModel()->attachment[2];
		}
		else
		{
		float fVec[3];
		fVec[0] = fVec[1] = fVec[2] = 8192.0;
		pEntity->origin = fVec;
		}
		}*/
		/*if(strstr(pEntity->model->name,"_head"))
		{
		if(pEntity->curstate.owner>0 && pEntity->curstate.owner<33)
		{
		float vFor[3];
		cl_entity_s *pEnt = gEngfuncs.GetEntityByIndex(pEntity->curstate.owner);
		gEngfuncs.pfnAngleVectors((float*)pEnt->angles,vFor,NULL,NULL);
		pEntity->origin[2] = pEnt->origin[2] + 17.0f;
		vFor[0] *= 3.0f;
		vFor[1] *= 3.0f;
		//vFor[2] *= 20.0f;

		pEntity->origin[1] = pEnt->origin[1]+vFor[1];
		pEntity->origin[0] = pEnt->origin[0]+vFor[0];
		pEntity->angles[1] = pEnt->angles[1];
		pEntity->angles[0] = pEnt->angles[0];
		}
		}*/
	}
	return gExportfuncs.HUD_AddEntity(iType, pEntity, pszModel);
}
#define KEY_DOT	96
#define KEY_ESC	27
#define KEY_R	114

//
// these are the key numbers that should be passed to Key_Event
//
#define	K_TAB			9
#define	K_ENTER			13
#define	K_ESCAPE		27
#define	K_SPACE			32

int HUD_Key_Event(int eventcode, int keynum, const char *pszCurrentBinding)
{
	/**/
	if (keynum == 53)
	{
		//g_mgui_candraw = 1 - g_mgui_candraw;
	}

	if (g_iMod == MOD_ZB4 && HudHealth().m_iHealth == 1 && g_iTeam == 2 && g_bAlive)
	{
		if (keynum != K_TAB && keynum != K_ENTER && keynum != K_ESCAPE)
			return 0;
	}

	if (g_mgui_candraw)
	{
		if (g_Next_Key_CanUse > cl.time)
		{
			g_mgui_keynum = -1;
			return 0;
		}

		g_mgui_keynum = keynum;
		g_Next_Key_CanUse = cl.time + 0.15;

		//if (keynum == K_ESCAPE)
			//BTEPanel_BuyMenu_Close();

		if ((keynum != 96 && keynum != 27) && (pszCurrentBinding && stricmp(pszCurrentBinding, "snapshot")))
		{
			return 0;
		}
	}

	HudZombieMod3().CheckSkillKey(eventcode, keynum, pszCurrentBinding);

	if (g_Next_Key_CanUse > cl.time)
	{
		g_mgui_keynum = -1;

		return gExportfuncs.HUD_Key_Event(eventcode, keynum, pszCurrentBinding);
	}

	return gExportfuncs.HUD_Key_Event(eventcode, keynum, pszCurrentBinding);
}

void UpdateBeams();

void HUD_CreateEntities()
{
	gExportfuncs.HUD_CreateEntities();

	UpdateBeams();	
}