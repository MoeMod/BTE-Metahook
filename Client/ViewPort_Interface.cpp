#include <metahook.h>
#include <vgui_controls/Controls.h>
using namespace vgui;

#include <Interface\vgui\IEngineVGui.h>
#include <Interface\IClientVGUI.h>
#include <VGUI\IPanel.h>
#include <Interface\IEngineSurface.h>
#include <VGUI\IPanel.h>
#include <vgui_controls/Panel.h>
#include <VGUI/ISurface.h>

#include <EngineInterface.h>
#include "ViewPort.h"
#include "ViewPort_Interface.h"

vgui::IEngineVGui *enginevgui;
IClientVGUI *g_pClientVGUI;

EXPOSE_SINGLE_INTERFACE(CClientVGUI, IClientVGUI, CLIENTVGUI_INTERFACE_VERSION);

void(__fastcall *g_pfnCClientVGUI_Initialize)(IClientVGUI *, int, CreateInterfaceFn *, int) = NULL;
void(__fastcall *g_pfnCClientVGUI_Start)(IClientVGUI *, int) = NULL;
void(__fastcall *g_pfnCClientVGUI_SetParent)(IClientVGUI *, int, vgui::VPANEL) = NULL;
bool(__fastcall *g_pfnCClientVGUI_UseVGUI1)(IClientVGUI *, int) = NULL;
void(__fastcall *g_pfnCClientVGUI_HideScoreBoard)(IClientVGUI *, int) = NULL;
void(__fastcall *g_pfnCClientVGUI_HideAllVGUIMenu)(IClientVGUI *, int) = NULL;
void(__fastcall *g_pfnCClientVGUI_ActivateClientUI)(IClientVGUI *, int) = NULL;
void(__fastcall *g_pfnCClientVGUI_HideClientUI)(IClientVGUI *, int) = NULL;

void CClientVGUI::Initialize(CreateInterfaceFn *factories, int count)
{
	return g_pfnCClientVGUI_Initialize(this, 0, factories, count);
}

void CClientVGUI::Start(void)
{
	g_pfnCClientVGUI_Start(this, 0);
	g_pViewPort = new CViewport();
	g_pViewPort->Start();
}

void CClientVGUI::SetParent(vgui::VPANEL parent)
{
	g_pfnCClientVGUI_SetParent(this, 0, parent);
	g_pViewPort->SetParent(parent);
}

bool CClientVGUI::UseVGUI1(void)
{
	return g_pfnCClientVGUI_UseVGUI1(this, 0);
}

void CClientVGUI::HideScoreBoard(void)
{
	return g_pfnCClientVGUI_HideScoreBoard(this, 0);
}

void CClientVGUI::HideAllVGUIMenu(void)
{
	g_pViewPort->HideAllVGUIMenu();
	return g_pfnCClientVGUI_HideAllVGUIMenu(this, 0);
}

void CClientVGUI::ActivateClientUI(void)
{
	g_pViewPort->ActivateClientUI();
	return g_pfnCClientVGUI_ActivateClientUI(this, 0);
}

void CClientVGUI::HideClientUI(void)
{
	g_pViewPort->HideClientUI();
	return g_pfnCClientVGUI_HideClientUI(this, 0);
}

void CClientVGUI::HideAllPanel(void)
{
	vgui::VPANEL rootpanel = enginevguifuncs->GetPanel(PANEL_CLIENTDLL);
	int count = vgui::ipanel()->GetChildCount(rootpanel);

	for (int i = 0; i < count; i++)
	{
		vgui::VPANEL p = vgui::ipanel()->GetChild(rootpanel, i);

		if (!p)
			continue;

		vgui::ipanel()->SetParent(p, NULL);
	}
}

void CClientVGUI::HidePanel(const char *name)
{
	vgui::VPANEL rootpanel = enginevguifuncs->GetPanel(PANEL_CLIENTDLL);
	int count = vgui::ipanel()->GetChildCount(rootpanel);

	for (int i = 0; i < count; i++)
	{
		vgui::VPANEL p = vgui::ipanel()->GetChild(rootpanel, i);

		if (!p || strcmp(vgui::ipanel()->GetName(p), name))
			continue;

		vgui::ipanel()->SetParent(p, NULL);
	}
}

void ViewPort_InstallHook(cl_exportfuncs_t *pExportfuncs)
{
	CreateInterfaceFn fnCreateInterfaceClient = (CreateInterfaceFn)pExportfuncs->ClientFactory();
	g_pClientVGUI = (IClientVGUI *)fnCreateInterfaceClient(CLIENTVGUI_INTERFACE_VERSION, NULL);

	CClientVGUI ClientVGUI;

	DWORD *pTable = *(DWORD **)&ClientVGUI;

	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 1, (void *)pTable[1], (void *&)g_pfnCClientVGUI_Initialize);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 2, (void *)pTable[2], (void *&)g_pfnCClientVGUI_Start);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 3, (void *)pTable[3], (void *&)g_pfnCClientVGUI_SetParent);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 4, (void *)pTable[4], (void *&)g_pfnCClientVGUI_UseVGUI1);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 5, (void *)pTable[5], (void *&)g_pfnCClientVGUI_HideScoreBoard);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 6, (void *)pTable[6], (void *&)g_pfnCClientVGUI_HideAllVGUIMenu);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 7, (void *)pTable[7], (void *&)g_pfnCClientVGUI_ActivateClientUI);
	g_pMetaHookAPI->VFTHook(g_pClientVGUI, 0, 8, (void *)pTable[8], (void *&)g_pfnCClientVGUI_HideClientUI);
}