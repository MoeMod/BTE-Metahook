#include <metahook.h>
#include "hud.h"
#include "exportfuncs.h"
#include "configs.h"
#include "BaseUI.h"
#include "Renderer.h"
#include "modules.h"
#include "perf_counter.h"
#include "Window.h"
#include "R.h"
#include "util.h"
#include "Console.h"
#include "mempatchs.h"
#include "Hook_LoadTGA.h"
#include "Hook_GL.h"

#include "Renderer/qgl.h"

#include "Client/ViewPort_Interface.h"
#include "Client/model.h"
#include "Client/PlayerClassManager.h"
#include "Client/WeaponManager.h"

#include "vgui_controls/Controls.h"

#include "Client/HUD/DrawTABPanel.h"
#include "Client/HUD/nvg.h"
#include "Client/HUD/overview.h"
#include "Client/HUD/Statistics.h"
#include "Client/HUD/DrawTGA.h"

cl_exportfuncs_t gExportfuncs;
metahook_api_t *g_pMetaHookAPI;
mh_interface_t *g_pInterface;
mh_enginesave_t *g_pMetaSave;

IFileSystem *&g_pFileSystem = g_pFullFileSystem;
DWORD g_dwEngineBase, g_dwEngineSize;
DWORD g_dwEngineBuildnum;
bool g_bIsUseSteam;
bool g_bWindowed;
int g_iVideoWidth, g_iVideoHeight, g_iBPP;
// HWND g_hWnd; // moved to window.cpp

#define GetEngfuncsAddress(addr) (g_dwEngineBase+addr-0x1D01000)

#define GL_BIND_SIG "\x8B\x44\x24\x04\x8B\x0D\x2A\x2A\x2A\x2A\x56\x8B\xF0\xC1\xFE\x10\x25\xFF\xFF\x00\x00\x4E\x3B\xC8"
#define GL_BIND_SIG_NEW "\x55\x8B\xEC\x8B\x45\x08\x8B\x0D\x2A\x2A\x2A\x2A\x56\x8B\xF0\xC1\xFE\x10\x25\xFF\xFF\x00\x00\x4E\x3B\xC8"

typedef void(*type_CL_InitTEnt_part2)(void);
type_CL_InitTEnt_part2			g_pfn_CL_InitTEnt_part2;
type_CL_InitTEnt_part2			g_real_CL_InitTEnt_part2;
void CL_InitTEnt_part2(void);

int(*g_pfnCL_Frame)(int);
int CL_Frame(int intermission);



#define CL_GETMODELBYINDEX_SIG "\x83\xEC\x10\x56\x57\x8B\x7C\x24\x1C\x8B\x34\xBD\x2A\x2A\x2A\x2A\x85\xF6"
#define CL_ADDTORESOURCELIST_SIG "\x8B\x44\x24\x04\x8B\x88\x84\x00\x00\x00\x85\xC9"
struct model_s *(*g_pfnCL_GetModelByIndex)(int index);
struct model_s *CL_GetModelByIndex(int index);
hook_t *g_phCL_GetModelByIndex;
void(*g_pfnCL_AddToResourceList)(resource_t *pResource, resource_t *pList);
hook_t *g_phCL_AddToResourceList;
void CL_AddToResourceList(resource_t *pResource, resource_t *pList);

char g_szModelPrecache[512][MAX_QPATH];
int g_iModelPrecacheNums;

typedef void(*type_Draw_CacheWadInitFromFile)(FileHandle_t *hFile, int len, char *name, int cacheMax, void *);
//extern type_Draw_CacheWadInitFromFile	g_real_Draw_CacheWadInitFromFile;
type_Draw_CacheWadInitFromFile g_pfn_Draw_CacheWadInitFromFile;
type_Draw_CacheWadInitFromFile g_real_Draw_CacheWadInitFromFile;
void Draw_CacheWadInitFromFile(FileHandle_t *hFile, int len, char *name, int cacheMax, void *);

int(*g_pfn_COM_MultipleOpenFile)(char *a1, char *a2, char *a3) = 0;
int COM_MultipleOpenFile(char *filename, char *a2, char *a3);

typedef int(*type_COM_MultipleOpenFile)(char *filename, char *a2, char *a3);
//extern type_COM_MultipleOpenFile	g_real_COM_MultipleOpenFile;

//type_COM_MultipleOpenFile g_pfn_COM_MultipleOpenFile;
type_COM_MultipleOpenFile g_real_COM_MultipleOpenFile;

TEMPENTITY *gTempEnts;
TEMPENTITY **gpTempEntFree;

int(*g_pfn_CL_LookupSound)(char *a1) = 0;
void(*g_pfn_GL_Bind)(int texnum) = 0;

void*(*g_pfn_Cache_Check)(cache_user_t *c) = 0;
void(*g_pfnCL_WeaponAnim)(int iAnim, int body) = (void(*)(int, int))0x1D3B490;

#define KEY_EVENT_SIG "\x81\xEC\x00\x04\x00\x00\x8D\x84\x24\x08\x04\x00\x00\x8D\x8C\x24"
void(*g_pfnKey_Event)(int key, int down);
void Key_Event(int key, int down);

int CL_LookupSound(char *a1);

void GL_Bind(int texnum);

#define MEM_ALLOC_SIG "\xE9\xB0\xFE\x2A\x00"
#define MEM_FREE_SIG "\xE9\x56\xF3\x2A\x00"

typedef struct sfx_s
{
	char name[MAX_QPATH];
	cache_user_t cache;
	int servercount;
} sfx_t;
void(*g_pfn_ResampleSfx)(sfx_t *sfx, int inrate, int inwidth, byte *data) = 0;
void ResampleSfx(sfx_t *sfx, int inrate, int inwidth, byte *data);

void *Cache_Check(cache_user_t *c);

void HUD_Frame(double flHostFrameTime);

cvar_t *developer;

extern int g_iVideoMode; // triapi.cpp

void IPlugins::Init(metahook_api_t *pAPI, mh_interface_t *pInterface, mh_enginesave_t *pSave)
{
	g_bIsUseSteam = false;

	g_pMetaHookAPI = pAPI;
	g_pInterface = pInterface;
	g_pMetaSave = pSave;

	Renderer_Init();
	if (g_pRenderer)
		g_pRenderer->Init(pAPI, pInterface, pSave);


	gPerformanceCounter.InitializePerformanceCounter();

	Window_Init();
	Console_Init();


	/*
	auto registry = pInterface->Registry;
	registry->WriteInt("ScreenWidth", 1920);
	registry->WriteInt("ScreenHeight", 1080);
	registry->WriteInt("ScreenBPP", 32);
	*/

	/*HMODULE hShcore = GetModuleHandle("user32.dll");
	if (hShcore)
	{
	SetProcessDpiAwareness = (void(*)(PROCESS_DPI_AWARENESS))GetProcAddress(hShcore, "SetProcessDpiAwarenessInternal");

	if (SetProcessDpiAwareness)
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	}*/

	FILE *fp;
	fp = fopen("metahook\\logs\\metahook.log", "rb");
	if (fp)
	{
		fclose(fp);
		char filepath[256];
		::GetCurrentDirectory(255, filepath);
		sprintf(filepath, "%s\\metahook\\logs\\metahook.log", filepath);
		DeleteFile(filepath);
		LogToFile("已删除之前的log内容!");
	}
	LogToFile("BTE插件已加载[2014/11/22]");

}

void IPlugins::Shutdown()
{
	Renderer_Shutdown();
	Module_Shutdown();
	Console_Shutdown();
}

void IPlugins::LoadEngine(void)
{
	//g_pMetaHookAPI->InlineHook((void *)CreateWindowExA, VID_CreateWindowExA, (void *&)g_pfnCreateWindowExA);

	/* Initialize FileSystem */
	g_pFileSystem = g_pInterface->FileSystem;
	g_iVideoMode = g_pMetaHookAPI->GetVideoMode(&g_iVideoWidth, &g_iVideoHeight, &g_iBPP, &g_bWindowed);
	g_dwEngineBuildnum = g_pMetaHookAPI->GetEngineBuildnum();
	g_iBPP = 32;

	g_pMetaHookAPI->WriteDWORD((void *)0x1DBAE6C, 0x8000000);
	g_pMetaHookAPI->WriteDWORD((void *)0x1DBAE73, 0x8000000);

	//g_hEngineModule = g_pMetaHookAPI->GetEngineModule();
	g_dwEngineBase = g_pMetaHookAPI->GetEngineBase();
	g_dwEngineSize = g_pMetaHookAPI->GetEngineSize();

	QGL_Init();
	Config_Init();
	MemPatch_Start(MEMPATCH_STEP_LOADENGINE);

	GL_InitHook();

	BaseUI_InstallHook();
	Module_InstallHook();
	R_InstallHook();
	LoadTGA_InstallHook(); // must after QGL_Init

	// Unknown function name
	g_pMetaHookAPI->InlineHook((void *)0x1D0E720, CL_Frame, (void *&)g_pfnCL_Frame);

	//g_pMetaHookAPI->InlineHook(g_pfnCL_WeaponAnim, CL_WeaponAnim, (void *&)g_pfnCL_WeaponAnim);

	g_pfnCL_GetModelByIndex = (struct model_s *(*)(int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, CL_GETMODELBYINDEX_SIG, sizeof(CL_GETMODELBYINDEX_SIG) - 1);
	g_pfnCL_AddToResourceList = (void(*)(resource_t *, resource_t *))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, CL_ADDTORESOURCELIST_SIG, sizeof(CL_ADDTORESOURCELIST_SIG) - 1);

	g_phCL_AddToResourceList = g_pMetaHookAPI->InlineHook(g_pfnCL_AddToResourceList, CL_AddToResourceList, (void *&)g_pfnCL_AddToResourceList);
	g_phCL_GetModelByIndex = g_pMetaHookAPI->InlineHook(g_pfnCL_GetModelByIndex, CL_GetModelByIndex, (void *&)g_pfnCL_GetModelByIndex);

	Model_InstallHook();

	g_pfn_CL_InitTEnt_part2 = (type_CL_InitTEnt_part2)0x1D295F0;
	g_pMetaHookAPI->InlineHook((void *)g_pfn_CL_InitTEnt_part2, CL_InitTEnt_part2, (void *&)g_real_CL_InitTEnt_part2);
	gTempEnts = (TEMPENTITY *)0x1EF9EB0;

	g_pfn_Draw_CacheWadInitFromFile = (type_Draw_CacheWadInitFromFile)GetEngfuncsAddress(0x1D31CC0);
	g_pMetaHookAPI->InlineHook((void *)g_pfn_Draw_CacheWadInitFromFile, Draw_CacheWadInitFromFile, (void *&)g_real_Draw_CacheWadInitFromFile);

	g_pfn_COM_MultipleOpenFile = (int(*)(char *, char *, char *))GetEngfuncsAddress(0x1D3C380);
	g_pMetaHookAPI->InlineHook((void *)g_pfn_COM_MultipleOpenFile, COM_MultipleOpenFile, (void *&)g_pfn_COM_MultipleOpenFile);


	
	//g_pMetaHookAPI->InlineHook((void *)g_pfn_COM_MultipleOpenFile, COM_MultipleOpenFile, (void *&)g_pfn_COM_MultipleOpenFile);


	g_pfn_CL_LookupSound = (int(*)(char *))GetEngfuncsAddress(0x1D8FFC0);

	g_pfn_ResampleSfx = (void(*)(sfx_t *, int, int, byte *))GetEngfuncsAddress(0x1D98530);
	g_pMetaHookAPI->InlineHook((void *)g_pfn_ResampleSfx, ResampleSfx, (void *&)g_pfn_ResampleSfx);

	g_pfn_Cache_Check = (void*(*)(cache_user_t *))GetEngfuncsAddress(0x1DCFF60);


	//g_pMetaHookAPI->InlineHook((void *)g_pfn_Cache_Check, Cache_Check, (void *&)g_pfn_Cache_Check);

	//g_pMetaHookAPI->InlineHook((void *)g_pfn_CL_LookupSound, CL_LookupSound, (void *&)g_pfn_CL_LookupSound);

	//g_pfn_GL_Bind = (void(*)(int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, GL_BIND_SIG, sizeof(GL_BIND_SIG) - 1);
	//g_pMetaHookAPI->InlineHook(g_pfn_GL_Bind, GL_Bind, (void *&)g_pfn_GL_Bind);

	g_pfnKey_Event = (void(*)(int, int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, KEY_EVENT_SIG, sizeof(KEY_EVENT_SIG) - 1);

	g_pMetaHookAPI->InlineHook(g_pfnKey_Event, Key_Event, (void *&)g_pfnKey_Event);

	if (g_pRenderer)
		g_pRenderer->LoadEngine();
}

void IPlugins::LoadClient(cl_exportfuncs_t *pExportFunc)
{
	MemPatch_Start(MEMPATCH_STEP_LOADCLIENT);
	Window_LoadClient();

	//LogToFile("MGUI版本[2013/8/17]");
	memcpy(&gExportfuncs, pExportFunc, sizeof(gExportfuncs));

	Module_LoadClient(pExportFunc);
	ViewPort_InstallHook(pExportFunc);

	pExportFunc->Initialize = Initialize;
	pExportFunc->HUD_Init = HUD_Init;
	pExportFunc->HUD_Redraw = HUD_Redraw;
	pExportFunc->V_CalcRefdef = V_CalcRefdef;
	pExportFunc->HUD_VidInit = HUD_VidInit;
	pExportFunc->HUD_DrawTransparentTriangles = HUD_DrawTransparentTriangles;
	pExportFunc->HUD_GetStudioModelInterface = HUD_GetStudioModelInterface;
	pExportFunc->HUD_AddEntity = HUD_AddEntity;
	pExportFunc->HUD_PlayerMove = HUD_PlayerMove;
	pExportFunc->HUD_UpdateClientData = HUD_UpdateClientData;
	pExportFunc->HUD_StudioEvent = HUD_StudioEvent;
	pExportFunc->HUD_TempEntUpdate = HUD_TempEntUpdate;
	pExportFunc->HUD_PostRunCmd = HUD_PostRunCmd;
	pExportFunc->HUD_Key_Event = HUD_Key_Event;
	pExportFunc->HUD_ProcessPlayerState = HUD_ProcessPlayerState;
	pExportFunc->IN_Accumulate = IN_Accumulate;
	pExportFunc->IN_ClearStates = IN_ClearStates;
	pExportFunc->IN_MouseEvent = IN_MouseEvent;
	pExportFunc->HUD_CreateEntities = HUD_CreateEntities;
	pExportFunc->CL_CreateMove = CL_CreateMove;
	pExportFunc->HUD_Frame = HUD_Frame;

	//pExportFunc->HUD_TxferPredictionData = HUD_TxferPredictionData;
	//pExportFunc->HUD_TxferLocalOverrides = HUD_TxferLocalOverrides;
	//pExportFunc->HUD_GetStudioModelInterface = HUD_GetStudioModelInterface;

	if (g_pRenderer)
		g_pRenderer->LoadClient(pExportFunc);

	//CClientVGUI::OnClientLoaded(pExportFunc);
}

void IPlugins::ExitGame(int iResult)
{
	//Fonts_Free();

	if (g_pRenderer)
		g_pRenderer->ExitGame(iResult);

	HudStatistics().Save();
}

void HUD_Frame(double flHostFrameTime)
{
	//Hud().Frame(flHostFrameTime);
}

int CL_Frame(int intermission)
{
	cl.oldtime = cl.time;
	cl.time = *(double *)0x2DE1468;

	HudNVG().DrawBackground();
	HudOverview().DrawEntities();
	int iResult = g_pfnCL_Frame(intermission);
	//gHUD.m_ZBCounter.Draw(cl.time);

	return iResult;
}

void CL_InitTEnt_part2(void)
{
	static TEMPENTITY gTempEntsNew[2500];

	g_real_CL_InitTEnt_part2();

	memset(gTempEntsNew, 0, sizeof(gTempEntsNew));
	gTempEnts[499].next = &gTempEntsNew[0];

	for (int i = 0; i < 2499; ++i)
		gTempEntsNew[i].next = &gTempEntsNew[i + 1];

	gTempEntsNew[2499].next = NULL;
}

struct model_s *CL_GetModelByIndex(int index)
{
	if (index == -1)
		return NULL;

	if (index >= 512)
		return IEngineStudio.Mod_ForName(g_szModelPrecache[index - 512], false);

	return g_pfnCL_GetModelByIndex(index);
}

void CL_AddToResourceList(resource_t *pResource, resource_t *pList)
{
	if (pResource->type == t_model)
	{
		if (strstr(pResource->szFileName, "models/v_") || strstr(pResource->szFileName, "models/p_"))
		{
			for (int i = 0; i < 512; i++)
			{
				if (!strcmp(g_szModelPrecache[i], pResource->szFileName))
					return;
			}

			strcpy(g_szModelPrecache[g_iModelPrecacheNums++], pResource->szFileName);

			char name[64], text[64];
			strcpy(name, pResource->szFileName + 9);
			name[strlen(name) - 4] = 0;

			sprintf(text, "gfx\\vgui\\%s.tga", name);
			if (g_pFileSystem->FileExists(text))
			{
				text[strlen(text) - 4] = 0;
				Hud().m_TGA.FindTexture(text);
			}

			if (pResource->szFileName[7] == 'v' && !strstr(pResource->szFileName, "_2.mdl"))
				WeaponManager().OnPrecacheWeapon(name);
				//LoadWeaponData(name);

			if (strstr(name, "buff"))
			{
				HudTABBoard().SetHostOwnBuff(true);
			}

			return;
		}

		if (strstr(pResource->szFileName, "models/player/"))
		{
			char name[64];
			strcpy(name, pResource->szFileName + 14);
			name[strlen(name) - 4] = 0;
			name[strlen(name) / 2] = 0;

			//LoadClassData(name);
			PlayerClassManager().OnPrecacheClass(name);
		}
	}

	g_pfnCL_AddToResourceList(pResource, pList);
}

void Draw_CacheWadInitFromFile(FileHandle_t *hFile, int len, char *name, int cacheMax, void *wad)
{
	/*if (!strcmp(name, "decals.wad") && gConfigs.bEnableBlood == 0)
	name = "decals_cn.wad";*/

	g_real_Draw_CacheWadInitFromFile(hFile, len, name, cacheMax, wad);
}

int COM_MultipleOpenFile(char *filename, char *a2, char *a3)
{
	if (!strcmp(filename, "decals.wad"))
	{
		a3 = "GAME";

		if (gConfigs.bEnableBlood == 0)
			filename = "decals_cn.wad";
	}

	int result = g_pfn_COM_MultipleOpenFile(filename, a2, a3);

	return result;
}

void CL_WeaponAnim(int iAnim, int body)
{
	static cl_entity_t *ent;
	ent = gEngfuncs.GetViewModel();

	if (ent)
	{
		if (strstr(ent->model->name, "v_dragonsword"))
		{
			if (iAnim == 5)
			{
				/*static*/ model_t *mod = IEngineStudio.Mod_ForName("models/ef_dragonsword.mdl", FALSE);
				TEMPENTITY *te = gEngfuncs.pEfxAPI->CL_TempEntAlloc(ent->origin, mod);
				if (te)
				{
					te->entity.angles = ent->angles;
					te->entity.curstate.sequence = /*gEngfuncs.pfnGetCvarFloat("cl_righthand") ? 2 : */3;
					te->entity.curstate.animtime = cl.time;
					te->entity.curstate.framerate = 1.0f;
					te->entity.curstate.scale = 1.0f;
					te->entity.curstate.frame = 0.0f;
					te->entity.curstate.body = body;
					te->frameMax = 11;
					te->flags |= FTENT_SPRANIMATE;
					te->die = cl.time + 0.4f;
				}
			}
		}
	}
	g_pfnCL_WeaponAnim(iAnim, body);
}

void Key_Event(int key, int down)
{
	/*
	if (key == VK_ESCAPE && down)
	{
		if (g_mgui_candraw)
		{
			BTEPanel_BuyMenu_Close();
			if (IS_ZOMBIE_MODE)
				BuyDefaultWeapon();
			return;
		}
	}
	*/
	return g_pfnKey_Event(key, down);
}

int CL_LookupSound(char *a1)
{
	return g_pfn_CL_LookupSound(a1);
}

void GL_Bind(int texnum)
{
	g_pfn_GL_Bind(texnum);

	//if (gl_texture_needansio)
	//glTexParameterf(GL_TEXTURE_2D, 0x84FE/*GL_TEXTURE_MAX_ANISOTROPY_EXT*/, 0/*gl_ansio->value*/);
}

typedef struct sfxcache_s
{
	int length;
	int loopstart;
	int speed;
	int width;
	int stereo;
	byte data[1];
}
sfxcache_t;

#if 0
#define CACHE_NAME_LEN 64
typedef struct cache_system_s
{
	int						size;		// including this header
	cache_user_t			*user;
	char					name[CACHE_NAME_LEN];
	struct cache_system_s	*prev, *next;
	struct cache_system_s	*lru_prev, *lru_next;	// for LRU flushing	
} cache_system_t;

void Cache_UnlinkLRU(cache_system_t *cs)
{
	/*if (!cs->lru_next || !cs->lru_prev)
	Sys_Error("Cache_UnlinkLRU: NULL link");*/

	cs->lru_next->lru_prev = cs->lru_prev;
	cs->lru_prev->lru_next = cs->lru_next;

	cs->lru_prev = cs->lru_next = NULL;
}

void Cache_MakeLRU(cache_system_t *cs)
{
	/*if (cs->lru_next || cs->lru_prev)
	Sys_Error("Cache_MakeLRU: active link");*/

	cache_system_t *cache_head = (cache_system_t *)0x24F9DB4;

	cache_head->lru_next->lru_prev = cs;
	cs->lru_next = cache_head->lru_next;
	cs->lru_prev = cache_head;
	cache_head->lru_next = cs;
}

void *Cache_Check(cache_user_t *c)
{
	cache_system_t	*cs;

	if (!c->data)
		return NULL;

	cs = ((cache_system_t *)c->data) - 1;

	// move to head of LRU
	//Cache_UnlinkLRU(cs);
	//Cache_MakeLRU(cs);

	return c->data;
}
#endif

typedef struct
{
	qboolean gamealive;
	qboolean soundalive;
	qboolean splitbuffer;
	int channels;
	int samples;
	int submission_chunk;
	int samplepos;
	int samplebits;
	int speed;
	int dmaspeed;
	unsigned char *buffer;
}
dma_t;

void ResampleSfx(sfx_t *sfx, int inrate, int inwidth, byte *data)
{
	int outcount;
	int srcsample;
	float stepscale;
	int i;
	int sample, samplefrac, fracstep;
	sfxcache_t *sc;
	dma_t *shm = *(dma_t **)0x24896D8;

	//if (shm->speed < 20000)
	//shm->speed = 11025;

	sc = (sfxcache_t*)g_pfn_Cache_Check(&sfx->cache);

	if (!sc)
		return;

	if (shm)
		stepscale = (float)inrate / shm->speed;
	else
		stepscale = 1;

	if (stepscale == 2 && 1/*hisound.value != 0*/)
		stepscale = 1;

	outcount = sc->length / stepscale;
	sc->length = outcount;

	if (sc->loopstart != -1)
		sc->loopstart = sc->loopstart / stepscale;

	if (stepscale != 1 && shm)
		sc->speed = shm->speed;

	if (0/*loadas8bit.value*/)
		sc->width = 1;
	else
		sc->width = inwidth;

	sc->stereo = 0;

	if (stepscale == 1 && inwidth == 1 && sc->width == 1)
	{
		for (i = 0; i < outcount; i++)
			((signed char *)sc->data)[i] = (int)((unsigned char)(data[i]) - 128);
	}
	else
	{
		/*if (stepscale != 1 && stepscale != 2)
		Con_DPrintf("WARNING! %s is causing runtime sample conversion!\n", sfx->name);*/

		samplefrac = 0;
		fracstep = stepscale * 512;

		for (i = 0; i < outcount; i++)
		{
			srcsample = samplefrac >> 9;
			samplefrac += fracstep;

			if (inwidth == 2)
				sample = (((short *)data)[srcsample]);
			else
				sample = (int)((unsigned char)(data[srcsample]) - 128) << 8;

			if (sc->width == 2)
				((short *)sc->data)[i] = sample;
			else
				((signed char *)sc->data)[i] = sample >> 8;
		}
	}
}

/*void *Cache_Check(cache_user_t *c)
{
return g_pfn_Cache_Check(c);
}
*/

EXPOSE_SINGLE_INTERFACE(IPlugins, IPlugins, METAHOOK_PLUGIN_API_VERSION);