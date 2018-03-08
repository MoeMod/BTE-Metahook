#pragma once

#include <metahook.h>
//#include <vgui_controls\Panel.h>
#include <Interface\vgui\IEngineVGui.h>
#include <vgui_controls\Controls.h>
#include <Interface\IGameUIFuncs.h>
#include <Interface\IGameUI.h>
#include <Interface\IClientVGUI.h>
#include <Interface\IBaseUI.h>
#include <Interface\IGameConsole.h>
#include <Interface\IEngineSurface.h>
#include <VGUI\IPanel.h>
//#include <r_studioint.h>
//#include <custom.h>

using namespace vgui;

extern vgui::IPanel *g_pPanel;
void BaseUI_InstallHook(void);

class CBaseUI : public IBaseUI
{
public:
	virtual void Initialize(CreateInterfaceFn *factories, int count);
	virtual void Start(struct cl_enginefuncs_s *engineFuncs, int interfaceVersion);
	virtual void Shutdown(void);
	virtual int Key_Event(int down, int keynum, const char *pszCurrentBinding);
	virtual void CallEngineSurfaceProc(void *hwnd, unsigned int msg, unsigned int wparam, long lparam);
	virtual void Paint(int x, int y, int right, int bottom);
	virtual void HideGameUI(void);
	virtual void ActivateGameUI(void);
	virtual bool IsGameUIVisible(void);
	virtual void HideConsole(void);
	virtual void ShowConsole(void);
};

class CKeyValuesSystem : public IKeyValuesSystem
{
public:
	virtual void RegisterSizeofKeyValues(int size);
	virtual void *AllocKeyValuesMemory(int size);
	virtual void FreeKeyValuesMemory(void *pMem);
	virtual HKeySymbol GetSymbolForString(const char *name);
	virtual const char *GetStringForSymbol(HKeySymbol symbol);
	virtual void GetLocalizedFromANSI(const char *ansi, wchar_t *outBuf, int unicodeBufferSizeInBytes);
	virtual void GetANSIFromLocalized(const wchar_t *wchar, char *outBuf, int ansiBufferSizeInBytes);
	virtual void AddKeyValuesToMemoryLeakList(void *pMem, HKeySymbol name);
	virtual void RemoveKeyValuesFromMemoryLeakList(void *pMem);
};

extern IBaseUI *baseuifuncs;
//extern IEngineVGui *enginevguifuncs;
//extern IGameConsole *g_pGameConsole;
//extern IFileSystem *g_pFileSystem;


extern IGameUIFuncs *gameuifuncs;
extern cl_enginefunc_t *engine;
//extern IEngineVGui *g_pEngineVGui;
extern IEngineSurface *staticSurface;

extern mh_interface_t *g_pInterface;

extern vgui::ISchemeManager *g_SchemeManager;
/*
extern struct model_s *(*g_pfnCL_GetModelByIndex)(int index);
//extern engine_studio_api_t IEngineStudio;

extern hook_t *g_phCL_GetModelByIndex;
extern void (*g_pfnCL_AddToResourceList)(resource_t *pResource, resource_t *pList);
extern hook_t *g_phCL_AddToResourceList;

struct model_s *CL_GetModelByIndex(int index);
void CL_AddToResourceList(resource_t *pResource, resource_t *pList);

extern char g_szModelPrecache[512][MAX_QPATH];
extern int g_iModelPrecacheNums;
*/
extern bool g_bGameUIActive;