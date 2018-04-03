#pragma once

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

#include <vgui/VGUI.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <vgui/IScheme.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include <VGUI_controls/Controls.h>
#include <VGUI_controls/Panel.h>
#include <VGUI_controls/Frame.h>

#include "EngineInterface.h"
#include "BaseViewport.h"
#include "ViewPortPanel.h"

#define MAX_SERVERNAME_LENGTH 64

#define PANEL_ALL "all"
#define PANEL_ACTIVE "active"

class CCSBackGroundPanel;
class CCSChatDialog;

#ifdef GetCurrentTime
#undef GetCurrentTime
#endif

class CViewport : public vgui::Panel
{
	DECLARE_CLASS_SIMPLE(CViewport, vgui::Panel);

public:
	CViewport(void);
	~CViewport(void);

public:
	void Start(void);
	void SetParent(vgui::VPANEL parent);
	void SetVisible(bool state);

public:
	virtual int GetViewPortScheme(void);
	virtual vgui::VPANEL GetViewPortPanel(void);
	virtual void CreateBackGround(void);
	virtual void ShowBackGround(bool bShow);

public:
	void Init(void);
	void VidInit(void);
	void Reset(void);
	void Think(void);

public:
	bool IsInLevel(void);
	bool IsInMultiplayer(void);
	bool IsVIPMap(void);
	bool IsBombDefuseMap(void);
	float GetCurrentTime(void) { return m_flCurrentTime; }
	char *GetServerName(void) { return m_szServerName; }

public:
	void HideAllVGUIMenu(void) {}
	bool ShowVGUIMenu(int iMenu) { return false; }
	bool HideVGUIMenu(int iMenu) { return false; }

	CViewPortPanel *AddNewPanel(CViewPortPanel *pPanel, char const *pchDebugName = NULL);
	CViewPortPanel *FindPanelByName(const char *szPanelName);
	void PostMessageToPanel(CViewPortPanel *pPanel, KeyValues *pKeyValues);
	void PostMessageToPanel(const char *pName, KeyValues *pKeyValues);
	CViewPortPanel *GetActivePanel(void);
	void ShowPanel(const char *pName, bool state);
	void ShowPanel(CViewPortPanel *pPanel, bool state);
	void UpdateAllPanels(void);
	void RemoveAllPanels(void);

	void ActivateClientUI(void) {}
	void HideClientUI(void) {}

	bool AllowedToPrintText(void);

private:
	float m_flCurrentTime;
	bool m_bInitialied;
	bool m_bShowBackGround;
	CUtlVector<CViewPortPanel *> m_Panels;
	CViewPortPanel *m_pActivePanel;
	CViewPortPanel *m_pLastActivePanel;
	CCSBackGroundPanel *m_pBackGround;
	CViewPortPanel *m_pPanelShow;
	char m_szServerName[MAX_SERVERNAME_LENGTH];

private:

public:
	friend int MsgFunc_ServerName(const char *pszName, int iSize, void *pbuf); //message.cpp
	
};

extern CViewport *g_pViewPort;