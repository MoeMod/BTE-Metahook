#pragma once

#include <metahook.h>
#include <Interface\vgui\IEngineVGui.h>
#include <vgui_controls\Controls.h>
#include <Interface\IGameUIFuncs.h>
#include <Interface\IGameUI.h>
#include <Interface\IClientVGUI.h>
#include <Interface\IBaseUI.h>
#include <Interface\IGameConsole.h>
#include <Interface\IEngineSurface.h>
#include <VGUI\IPanel.h>

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

extern IBaseUI *baseuifuncs;
extern IGameUIFuncs *gameuifuncs;
extern cl_enginefunc_t *engine;
extern IEngineSurface *staticSurface;
extern mh_interface_t *g_pInterface;

extern vgui::ISchemeManager *g_SchemeManager;
extern bool g_bGameUIActive;