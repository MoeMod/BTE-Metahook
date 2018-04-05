#include <metahook.h>
#include <assert.h>

#include "VPanel.h"
#include "vgui_internal.h"

#include <vgui/IClientPanel.h>
#include <vgui/IPanel.h>
#include <vgui/ISurface.h>

using namespace vgui;

void (__fastcall *g_pfnVPanelWrapper_Init)(void *pthis, int, VPANEL vguiPanel, IClientPanel *panel);
void (__fastcall *g_pfnVPanelWrapper_SetPos)(void *pthis, int, VPANEL vguiPanel, int x, int y);
void (__fastcall *g_pfnVPanelWrapper_GetPos)(void *pthis, int, VPANEL vguiPanel, int &x, int &y);
void (__fastcall *g_pfnVPanelWrapper_SetSize)(void *pthis, int, VPANEL vguiPanel, int wide, int tall);
void (__fastcall *g_pfnVPanelWrapper_GetSize)(void *pthis, int, VPANEL vguiPanel, int &wide, int &tall);
void (__fastcall *g_pfnVPanelWrapper_SetMinimumSize)(void *pthis, int, VPANEL vguiPanel, int wide, int tall);
void (__fastcall *g_pfnVPanelWrapper_GetMinimumSize)(void *pthis, int, VPANEL vguiPanel, int &wide, int &tall);
void (__fastcall *g_pfnVPanelWrapper_SetZPos)(void *pthis, int, VPANEL vguiPanel, int z);
int (__fastcall *g_pfnVPanelWrapper_GetZPos)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_GetAbsPos)(void *pthis, int, VPANEL vguiPanel, int &x, int &y);
void (__fastcall *g_pfnVPanelWrapper_GetClipRect)(void *pthis, int, VPANEL vguiPanel, int &x0, int &y0, int &x1, int &y1);
void (__fastcall *g_pfnVPanelWrapper_SetInset)(void *pthis, int, VPANEL vguiPanel, int left, int top, int right, int bottom);
void (__fastcall *g_pfnVPanelWrapper_GetInset)(void *pthis, int, VPANEL vguiPanel, int &left, int &top, int &right, int &bottom);
void (__fastcall *g_pfnVPanelWrapper_SetVisible)(void *pthis, int, VPANEL vguiPanel, bool state);
bool (__fastcall *g_pfnVPanelWrapper_IsVisible)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_SetParent)(void *pthis, int, VPANEL vguiPanel, VPANEL newParent);
int (__fastcall *g_pfnVPanelWrapper_GetChildCount)(void *pthis, int, VPANEL vguiPanel);
VPANEL (__fastcall *g_pfnVPanelWrapper_GetChild)(void *pthis, int, VPANEL vguiPanel, int index);
VPANEL (__fastcall *g_pfnVPanelWrapper_GetParent)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_MoveToFront)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_MoveToBack)(void *pthis, int, VPANEL vguiPanel);
bool (__fastcall *g_pfnVPanelWrapper_HasParent)(void *pthis, int, VPANEL vguiPanel, VPANEL potentialParent);
bool (__fastcall *g_pfnVPanelWrapper_IsPopup)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_SetPopup)(void *pthis, int, VPANEL vguiPanel, bool state);
bool (__fastcall *g_pfnVPanelWrapper_Render_GetPopupVisible)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_Render_SetPopupVisible)(void *pthis, int, VPANEL vguiPanel, bool state);
HScheme (__fastcall *g_pfnVPanelWrapper_GetScheme)(void *pthis, int, VPANEL vguiPanel);
bool (__fastcall *g_pfnVPanelWrapper_IsProportional)(void *pthis, int, VPANEL vguiPanel);
bool (__fastcall *g_pfnVPanelWrapper_IsAutoDeleteSet)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_DeletePanel)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_SetKeyBoardInputEnabled)(void *pthis, int, VPANEL vguiPanel, bool state);
void (__fastcall *g_pfnVPanelWrapper_SetMouseInputEnabled)(void *pthis, int, VPANEL vguiPanel, bool state);
bool (__fastcall *g_pfnVPanelWrapper_IsKeyBoardInputEnabled)(void *pthis, int, VPANEL vguiPanel);
bool (__fastcall *g_pfnVPanelWrapper_IsMouseInputEnabled)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_Solve)(void *pthis, int, VPANEL vguiPanel);
const char *(__fastcall *g_pfnVPanelWrapper_GetName)(void *pthis, int, VPANEL vguiPanel);
const char *(__fastcall *g_pfnVPanelWrapper_GetClassName)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_SendMessage)(void *pthis, int, VPANEL vguiPanel, KeyValues *params, VPANEL ifromPanel);
void (__fastcall *g_pfnVPanelWrapper_Think)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_PerformApplySchemeSettings)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_PaintTraverse)(void *pthis, int, VPANEL vguiPanel, bool forceRepaint, bool allowForce);
void (__fastcall *g_pfnVPanelWrapper_Repaint)(void *pthis, int, VPANEL vguiPanel);
VPANEL (__fastcall *g_pfnVPanelWrapper_IsWithinTraverse)(void *pthis, int, VPANEL vguiPanel, int x, int y, bool traversePopups);
void (__fastcall *g_pfnVPanelWrapper_OnChildAdded)(void *pthis, int, VPANEL vguiPanel, VPANEL child);
void (__fastcall *g_pfnVPanelWrapper_OnSizeChanged)(void *pthis, int, VPANEL vguiPanel, int newWide, int newTall);
void (__fastcall *g_pfnVPanelWrapper_InternalFocusChanged)(void *pthis, int, VPANEL vguiPanel, bool lost);
bool (__fastcall *g_pfnVPanelWrapper_RequestInfo)(void *pthis, int, VPANEL vguiPanel, KeyValues *outputData);
void (__fastcall *g_pfnVPanelWrapper_RequestFocus)(void *pthis, int, VPANEL vguiPanel, int direction);
bool (__fastcall *g_pfnVPanelWrapper_RequestFocusPrev)(void *pthis, int, VPANEL vguiPanel, VPANEL existingPanel);
bool (__fastcall *g_pfnVPanelWrapper_RequestFocusNext)(void *pthis, int, VPANEL vguiPanel, VPANEL existingPanel);
VPANEL (__fastcall *g_pfnVPanelWrapper_GetCurrentKeyFocus)(void *pthis, int, VPANEL vguiPanel);
int (__fastcall *g_pfnVPanelWrapper_GetTabPosition)(void *pthis, int, VPANEL vguiPanel);
SurfacePlat *(__fastcall *g_pfnVPanelWrapper_Plat)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_SetPlat)(void *pthis, int, VPANEL vguiPanel, SurfacePlat *Plat);
Panel *(__fastcall *g_pfnVPanelWrapper_GetPanel)(void *pthis, int, VPANEL vguiPanel, const char *destinationModule);
bool (__fastcall *g_pfnVPanelWrapper_IsEnabled)(void *pthis, int, VPANEL vguiPanel);
void (__fastcall *g_pfnVPanelWrapper_SetEnabled)(void *pthis, int, VPANEL vguiPanel, bool state);
IClientPanel *(__fastcall *g_pfnVPanelWrapper_Client)(void *pthis, int, VPANEL vguiPanel);
const char *(__fastcall *g_pfnVPanelWrapper_GetModuleName)(void *pthis, int, VPANEL vguiPanel);

class VPanelWrapper : public vgui::IPanel
{
public:
	virtual void Init(VPANEL vguiPanel, IClientPanel *panel)
	{
		((VPanel *)vguiPanel)->Init(panel);
	}

	virtual void SetPos(VPANEL vguiPanel, int x, int y)
	{
		((VPanel *)vguiPanel)->SetPos(x, y);
	}

	virtual void GetPos(VPANEL vguiPanel, int &x, int &y)
	{
		((VPanel *)vguiPanel)->GetPos(x, y);
	}

	virtual void SetSize(VPANEL vguiPanel, int wide, int tall)
	{
		((VPanel *)vguiPanel)->SetSize(wide, tall);
	}

	virtual void GetSize(VPANEL vguiPanel, int &wide, int &tall)
	{
		((VPanel *)vguiPanel)->GetSize(wide, tall);
	}

	virtual void SetMinimumSize(VPANEL vguiPanel, int wide, int tall)
	{
		((VPanel *)vguiPanel)->SetMinimumSize(wide, tall);
	}

	virtual void GetMinimumSize(VPANEL vguiPanel, int &wide, int &tall)
	{
		((VPanel *)vguiPanel)->GetMinimumSize(wide, tall);
	}

	virtual void SetZPos(VPANEL vguiPanel, int z)
	{
		((VPanel *)vguiPanel)->SetZPos(z);
	}

	virtual int GetZPos(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->GetZPos();
	}

	virtual void GetAbsPos(VPANEL vguiPanel, int &x, int &y)
	{
		((VPanel *)vguiPanel)->GetAbsPos(x, y);
	}

	virtual void GetClipRect(VPANEL vguiPanel, int &x0, int &y0, int &x1, int &y1)
	{
		((VPanel *)vguiPanel)->GetClipRect(x0, y0, x1, y1);
	}

	virtual void SetInset(VPANEL vguiPanel, int left, int top, int right, int bottom)
	{
		((VPanel *)vguiPanel)->SetInset(left, top, right, bottom);
	}

	virtual void GetInset(VPANEL vguiPanel, int &left, int &top, int &right, int &bottom)
	{
		((VPanel *)vguiPanel)->GetInset(left, top, right, bottom);
	}

	virtual void SetVisible(VPANEL vguiPanel, bool state)
	{
		((VPanel *)vguiPanel)->SetVisible(state);
	}

	virtual bool IsVisible(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->IsVisible();
	}

	virtual void SetParent(VPANEL vguiPanel, VPANEL newParent)
	{
		((VPanel *)vguiPanel)->SetParent((VPanel *)newParent);
	}

	virtual int GetChildCount(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->GetChildCount();
	}

	virtual VPANEL GetChild(VPANEL vguiPanel, int index)
	{
		return (VPANEL)((VPanel *)vguiPanel)->GetChild(index);
	}

	virtual VPANEL GetParent(VPANEL vguiPanel)
	{
		return (VPANEL)((VPanel *)vguiPanel)->GetParent();
	}

	virtual void MoveToFront(VPANEL vguiPanel)
	{
		((VPanel *)vguiPanel)->MoveToFront();
	}

	virtual void MoveToBack(VPANEL vguiPanel)
	{
		((VPanel *)vguiPanel)->MoveToBack();
	}

	virtual bool HasParent(VPANEL vguiPanel, VPANEL potentialParent)
	{
		if (!vguiPanel)
			return false;

		return ((VPanel *)vguiPanel)->HasParent((VPanel *)potentialParent);
	}

	virtual bool IsPopup(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->IsPopup();
	}

	virtual void SetPopup(VPANEL vguiPanel, bool state)
	{
		((VPanel *)vguiPanel)->SetPopup(state);
	}

	virtual bool Render_GetPopupVisible(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->Render_IsPopupPanelVisible();
	}

	virtual void Render_SetPopupVisible(VPANEL vguiPanel, bool state)
	{
		return ((VPanel *)vguiPanel)->Render_SetPopupVisible(state);
	}

	virtual HScheme GetScheme(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->GetScheme();
	}

	virtual bool IsProportional(VPANEL vguiPanel)
	{
		return Client(vguiPanel)->IsProportional();
	}

	virtual bool IsAutoDeleteSet(VPANEL vguiPanel)
	{
		return Client(vguiPanel)->IsAutoDeleteSet();
	}

	virtual void DeletePanel(VPANEL vguiPanel)
	{
		Client(vguiPanel)->DeletePanel();
	}

	virtual void SetKeyBoardInputEnabled(VPANEL vguiPanel, bool state)
	{
		((VPanel *)vguiPanel)->SetKeyBoardInputEnabled(state);
	}

	virtual void SetMouseInputEnabled(VPANEL vguiPanel, bool state)
	{
		((VPanel *)vguiPanel)->SetMouseInputEnabled(state);
	}

	virtual bool IsMouseInputEnabled(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->IsMouseInputEnabled();
	}

	virtual bool IsKeyBoardInputEnabled(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->IsKeyBoardInputEnabled();
	}

	virtual void Solve(VPANEL vguiPanel)
	{
		((VPanel *)vguiPanel)->Solve();
	}

	virtual const char *GetName(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->GetName();
	}

	virtual const char *GetClassName(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->GetClassName();
	}

	virtual void SendMessage(VPANEL vguiPanel, KeyValues *params, VPANEL ifrompanel)
	{
		((VPanel *)vguiPanel)->SendMessage(params, ifrompanel);
	}

	virtual void Think(VPANEL vguiPanel)
	{
		Client(vguiPanel)->Think();
	}

	virtual void PerformApplySchemeSettings(VPANEL vguiPanel)
	{
		Client(vguiPanel)->PerformApplySchemeSettings();
	}

	virtual void PaintTraverse(VPANEL vguiPanel, bool forceRepaint, bool allowForce)
	{
		Client(vguiPanel)->PaintTraverse(forceRepaint, allowForce);
	}

	virtual void Repaint(VPANEL vguiPanel)
	{
		Client(vguiPanel)->Repaint();
	}

	virtual VPANEL IsWithinTraverse(VPANEL vguiPanel, int x, int y, bool traversePopups)
	{
		return Client(vguiPanel)->IsWithinTraverse(x, y, traversePopups);
	}

	virtual void OnChildAdded(VPANEL vguiPanel, VPANEL child)
	{
		Client(vguiPanel)->OnChildAdded(child);
	}

	virtual void OnSizeChanged(VPANEL vguiPanel, int newWide, int newTall)
	{
		Client(vguiPanel)->OnSizeChanged(newWide, newTall);
	}

	virtual void InternalFocusChanged(VPANEL vguiPanel, bool lost)
	{
		Client(vguiPanel)->InternalFocusChanged(lost);
	}

	virtual bool RequestInfo(VPANEL vguiPanel, KeyValues *outputData)
	{
		return Client(vguiPanel)->RequestInfo(outputData);
	}

	virtual void RequestFocus(VPANEL vguiPanel, int direction = 0)
	{
		Client(vguiPanel)->RequestFocus(direction);
	}

	virtual bool RequestFocusPrev(VPANEL vguiPanel, VPANEL existingPanel)
	{
		return Client(vguiPanel)->RequestFocusPrev(existingPanel);
	}

	virtual bool RequestFocusNext(VPANEL vguiPanel, VPANEL existingPanel)
	{
		return Client(vguiPanel)->RequestFocusNext(existingPanel);
	}

	virtual VPANEL GetCurrentKeyFocus(VPANEL vguiPanel)
	{
		return Client(vguiPanel)->GetCurrentKeyFocus();
	}

	virtual int GetTabPosition(VPANEL vguiPanel)
	{
		return Client(vguiPanel)->GetTabPosition();
	}

	virtual SurfacePlat *Plat(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->Plat();
	}

	virtual void SetPlat(VPANEL vguiPanel, SurfacePlat *Plat)
	{
		((VPanel *)vguiPanel)->SetPlat(Plat);
	}

	virtual Panel *GetPanel(VPANEL vguiPanel, const char *moduleName)
	{
		if (vguiPanel == g_pSurface->GetEmbeddedPanel())
			return NULL;

		if (stricmp(GetModuleName(vguiPanel), moduleName))
			return NULL;

		return Client(vguiPanel)->GetPanel();
	}

	virtual bool IsEnabled(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->IsEnabled();
	}

	virtual void SetEnabled(VPANEL vguiPanel, bool state)
	{
		((VPanel *)vguiPanel)->SetEnabled(state);
	}

	virtual IClientPanel *Client(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->Client();
	}

	virtual const char *GetModuleName(VPANEL vguiPanel)
	{
		return Client(vguiPanel)->GetModuleName();
	}

public:
	bool IsFullyVisible(VPANEL vguiPanel)
	{
		return ((VPanel *)vguiPanel)->IsFullyVisible();
	}
};

VPanelWrapper g_Panel;

bool IPanel::IsFullyVisible(VPANEL vguiPanel)
{
	return g_Panel.IsFullyVisible(vguiPanel);
}

void IPanel::SetTopmostPopup(VPANEL vguiPanel, bool state)
{
	return;
}

void Panel_InstallHook(vgui::IPanel *pPanel)
{
	DWORD *pVFTable = *(DWORD **)&g_Panel;

	g_pIPanel = pPanel;
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 1, (void *)pVFTable[1], (void *&)g_pfnVPanelWrapper_Init);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 2, (void *)pVFTable[2], (void *&)g_pfnVPanelWrapper_SetPos);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 3, (void *)pVFTable[3], (void *&)g_pfnVPanelWrapper_GetPos);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 4, (void *)pVFTable[4], (void *&)g_pfnVPanelWrapper_SetSize);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 5, (void *)pVFTable[5], (void *&)g_pfnVPanelWrapper_GetSize);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 6, (void *)pVFTable[6], (void *&)g_pfnVPanelWrapper_SetMinimumSize);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 7, (void *)pVFTable[7], (void *&)g_pfnVPanelWrapper_GetMinimumSize);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 8, (void *)pVFTable[8], (void *&)g_pfnVPanelWrapper_SetZPos);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 9, (void *)pVFTable[9], (void *&)g_pfnVPanelWrapper_GetZPos);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 10, (void *)pVFTable[10], (void *&)g_pfnVPanelWrapper_GetClipRect);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 11, (void *)pVFTable[11], (void *&)g_pfnVPanelWrapper_SetInset);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 12, (void *)pVFTable[12], (void *&)g_pfnVPanelWrapper_GetInset);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 13, (void *)pVFTable[13], (void *&)g_pfnVPanelWrapper_SetVisible);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 14, (void *)pVFTable[14], (void *&)g_pfnVPanelWrapper_IsVisible);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 15, (void *)pVFTable[15], (void *&)g_pfnVPanelWrapper_SetParent);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 16, (void *)pVFTable[16], (void *&)g_pfnVPanelWrapper_GetChildCount);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 17, (void *)pVFTable[17], (void *&)g_pfnVPanelWrapper_GetChild);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 18, (void *)pVFTable[18], (void *&)g_pfnVPanelWrapper_GetParent);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 19, (void *)pVFTable[19], (void *&)g_pfnVPanelWrapper_MoveToFront);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 20, (void *)pVFTable[20], (void *&)g_pfnVPanelWrapper_MoveToBack);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 21, (void *)pVFTable[21], (void *&)g_pfnVPanelWrapper_HasParent);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 22, (void *)pVFTable[22], (void *&)g_pfnVPanelWrapper_IsPopup);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 23, (void *)pVFTable[23], (void *&)g_pfnVPanelWrapper_SetPopup);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 24, (void *)pVFTable[24], (void *&)g_pfnVPanelWrapper_Render_GetPopupVisible);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 25, (void *)pVFTable[25], (void *&)g_pfnVPanelWrapper_Render_SetPopupVisible);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 26, (void *)pVFTable[26], (void *&)g_pfnVPanelWrapper_GetScheme);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 27, (void *)pVFTable[27], (void *&)g_pfnVPanelWrapper_IsProportional);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 28, (void *)pVFTable[28], (void *&)g_pfnVPanelWrapper_IsAutoDeleteSet);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 29, (void *)pVFTable[29], (void *&)g_pfnVPanelWrapper_DeletePanel);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 30, (void *)pVFTable[30], (void *&)g_pfnVPanelWrapper_SetKeyBoardInputEnabled);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 31, (void *)pVFTable[31], (void *&)g_pfnVPanelWrapper_SetMouseInputEnabled);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 32, (void *)pVFTable[32], (void *&)g_pfnVPanelWrapper_IsKeyBoardInputEnabled);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 33, (void *)pVFTable[33], (void *&)g_pfnVPanelWrapper_Solve);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 34, (void *)pVFTable[34], (void *&)g_pfnVPanelWrapper_GetName);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 35, (void *)pVFTable[35], (void *&)g_pfnVPanelWrapper_GetClassName);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 36, (void *)pVFTable[36], (void *&)g_pfnVPanelWrapper_SendMessage);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 37, (void *)pVFTable[37], (void *&)g_pfnVPanelWrapper_Think);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 38, (void *)pVFTable[38], (void *&)g_pfnVPanelWrapper_PerformApplySchemeSettings);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 39, (void *)pVFTable[39], (void *&)g_pfnVPanelWrapper_Repaint);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 40, (void *)pVFTable[40], (void *&)g_pfnVPanelWrapper_IsWithinTraverse);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 41, (void *)pVFTable[41], (void *&)g_pfnVPanelWrapper_OnChildAdded);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 42, (void *)pVFTable[42], (void *&)g_pfnVPanelWrapper_InternalFocusChanged);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 43, (void *)pVFTable[43], (void *&)g_pfnVPanelWrapper_RequestInfo);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 44, (void *)pVFTable[44], (void *&)g_pfnVPanelWrapper_RequestFocus);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 45, (void *)pVFTable[45], (void *&)g_pfnVPanelWrapper_RequestFocusPrev);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 46, (void *)pVFTable[46], (void *&)g_pfnVPanelWrapper_RequestFocusNext);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 47, (void *)pVFTable[47], (void *&)g_pfnVPanelWrapper_GetCurrentKeyFocus);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 48, (void *)pVFTable[48], (void *&)g_pfnVPanelWrapper_GetTabPosition);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 49, (void *)pVFTable[49], (void *&)g_pfnVPanelWrapper_Plat);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 50, (void *)pVFTable[50], (void *&)g_pfnVPanelWrapper_SetPlat);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 51, (void *)pVFTable[51], (void *&)g_pfnVPanelWrapper_GetPanel);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 52, (void *)pVFTable[52], (void *&)g_pfnVPanelWrapper_IsEnabled);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 53, (void *)pVFTable[53], (void *&)g_pfnVPanelWrapper_SetEnabled);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 54, (void *)pVFTable[54], (void *&)g_pfnVPanelWrapper_Client);
	g_pMetaHookAPI->VFTHook(g_pIPanel, 0, 55, (void *)pVFTable[55], (void *&)g_pfnVPanelWrapper_GetModuleName);
}

EXPOSE_SINGLE_INTERFACE(VPanelWrapper, IPanel, VGUI_PANEL_INTERFACE_VERSION);