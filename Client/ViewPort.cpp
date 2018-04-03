#include "metahook.h"
#include "exportfuncs.h"
#include "plugins.h"
#include "demo_api.h"

#include "ViewPort.h"
#include "links.h"
#include "CounterStrikeViewPort.h"
#include "VGUI/CSBackGroundPanel.h"

using namespace vgui;

CViewport *g_pViewPort = NULL;

CViewport::CViewport()
{
	m_bInitialied = false;

	int swide, stall;
	surface()->GetScreenSize(swide, stall);

	MakePopup(false, true);
	SetScheme("ClientScheme");
	SetBounds(0, 0, swide, stall);
	SetPaintBorderEnabled(false);
	SetPaintBackgroundEnabled(false);
	SetMouseInputEnabled(false);
	SetKeyBoardInputEnabled(false);

	m_bInitialied = false;
	m_bShowBackGround = false;

	m_pActivePanel = NULL;
	m_pLastActivePanel = NULL;
	m_pPanelShow = NULL;

	m_Panels.RemoveAll();

}

CViewport::~CViewport()
{
	RemoveAllPanels();
}

void CViewport::Start(void)
{
	CreateBackGround();


	m_bInitialied = true;

	SetVisible(false);
}

void CViewport::SetParent(VPANEL parent)
{
	Panel::SetParent(parent);

	for (int i = 0; i < m_Panels.Count(); i++)
		m_Panels[i]->SetParent(this);

}

void CViewport::SetVisible(bool state)
{
	Panel::SetVisible(state);

	if (state)
	{
		if (m_pActivePanel)
		{
			if (m_pActivePanel->HasInputElements() && m_pActivePanel->IsMouseInputEnabled())
			{
				m_pActivePanel->SetMouseInputEnabled(false);
				m_pActivePanel->SetMouseInputEnabled(true);
			}
		}
	}
}

int CViewport::GetViewPortScheme(void)
{
	return m_pBackGround->GetScheme();
}

VPANEL CViewport::GetViewPortPanel(void)
{
	return m_pBackGround->GetVParent();
}

void CViewport::CreateBackGround(void)
{
	m_pBackGround = new CCSBackGroundPanel(this);
	m_pBackGround->SetZPos(-20);
	m_pBackGround->SetVisible(false);
}

void CViewport::ShowBackGround(bool bShow)
{
	m_bShowBackGround = bShow;
	m_pBackGround->SetVisible(bShow);
}

void CViewport::Init(void)
{
	for (int i = 0; i < m_Panels.Count(); i++)
		m_Panels[i]->Init();

	HideAllVGUIMenu();
}

void CViewport::VidInit(void)
{
	for (int i = 0; i < m_Panels.Count(); i++)
		m_Panels[i]->VidInit();
	HideAllVGUIMenu();
}

void CViewport::Reset(void)
{
	for (int i = 0; i < m_Panels.Count(); i++)
		m_Panels[i]->Reset();
}

void CViewport::Think(void)
{
	m_flCurrentTime = gEngfuncs.GetAbsoluteTime();
}

bool CViewport::IsInLevel(void)
{
	const char *levelName = gEngfuncs.pfnGetLevelName();

	if (levelName && levelName[0])
		return true;

	return false;
}

bool CViewport::IsInMultiplayer(void)
{
	return (IsInLevel() && gEngfuncs.GetMaxClients() > 1);
}

bool CViewport::IsVIPMap(void)
{
	const char *mapName = engine->pfnGetLevelName();

	if (!mapName)
		return false;

	if (!Q_strncmp(mapName, "maps/as_", 8))
		return true;

	return false;
}

bool CViewport::IsBombDefuseMap(void)
{
	const char *mapName = engine->pfnGetLevelName();

	if (!mapName)
		return false;

	if (!Q_strncmp(mapName, "maps/de_", 8))
		return true;

	return false;
}

CViewPortPanel *CViewport::AddNewPanel(CViewPortPanel *pPanel, char const *pchDebugName)
{
	if (!pPanel)
	{
		if (pchDebugName)
			DevMsg("CViewport::AddNewPanel(%s): NULL panel.\n", pchDebugName);
		else
			Assert(0);

		delete pPanel;
		return NULL;
	}

	if (FindPanelByName(pPanel->GetName()) != NULL)
	{
		DevMsg("CViewport::AddNewPanel: panel with name '%s' already exists.\n", pPanel->GetName());

		delete pPanel;
		return NULL;
	}

	m_Panels.AddToTail(pPanel);

	ipanel()->SetVisible(pPanel->GetVPanel(), false);
	pPanel->SetParent(this);
	return pPanel;
}

CViewPortPanel *CViewport::FindPanelByName(const char *szPanelName)
{
	int count = m_Panels.Count();

	for (int i = 0; i < count; i++)
	{
		if (Q_strcmp(m_Panels[i]->GetName(), szPanelName) == 0)
			return m_Panels[i];
	}

	return NULL;
}

#ifdef PostMessage
#undef PostMessage
#endif
void CViewport::PostMessageToPanel(CViewPortPanel *pPanel, KeyValues *pKeyValues)
{
	PostMessage(pPanel->GetVPanel(), pKeyValues);
}

void CViewport::PostMessageToPanel(const char *pName, KeyValues *pKeyValues)
{
	if (Q_strcmp(pName, PANEL_ALL) == 0)
	{
		for (int i = 0; i < m_Panels.Count(); i++)
		{
			if (m_Panels[i]->IsDynamic())
				PostMessageToPanel(m_Panels[i], pKeyValues);
		}

		return;
	}

	CViewPortPanel *panel = NULL;

	if (Q_strcmp(pName, PANEL_ACTIVE) == 0)
		panel = m_pActivePanel;
	else
		panel = FindPanelByName(pName);

	if (!panel)
		return;

	PostMessageToPanel(panel, pKeyValues);
}

CViewPortPanel *CViewport::GetActivePanel(void)
{
	return m_pActivePanel;
}

void CViewport::ShowPanel(const char *pName, bool state)
{
	if (Q_strcmp(pName, PANEL_ALL) == 0)
	{
		if (!state)
			m_pPanelShow = NULL;

		for (int i = 0; i < m_Panels.Count(); i++)
		{
			if (m_Panels[i]->IsDynamic())
				ShowPanel(m_Panels[i], state);
		}

		return;
	}

	CViewPortPanel *panel = NULL;

	if (Q_strcmp(pName, PANEL_ACTIVE) == 0)
		panel = m_pActivePanel;
	else
		panel = FindPanelByName(pName);

	if (!panel)
		return;

	ShowPanel(panel, state);
}

void CViewport::ShowPanel(CViewPortPanel *pPanel, bool state)
{
	/*if (m_pTextWindow->IsVisible())
	{
		if (pPanel == m_pScoreBoard && state)
		{
			m_pTextWindow->ShowPanel(false);
		}

		if (pPanel != m_pTextWindow && pPanel != m_pScoreBoard)
		{
			if (m_pPanelShow && m_pPanelShow != pPanel)
				ShowPanel(m_pPanelShow, true);

			if (state)
				m_pPanelShow = pPanel;
			else if (m_pPanelShow == pPanel)
				m_pPanelShow = NULL;

			return;
		}
	}
	else
	{
		if (pPanel == m_pScoreBoard && !state)
		{
			if (!m_pTextWindow->IsExited())
				m_pTextWindow->ShowPanel(true);
		}
	}*/

	if (state)
	{
		if (pPanel->HasInputElements())
		{
			if (gEngfuncs.IsSpectateOnly())
				return;

			if (gEngfuncs.pDemoAPI->IsPlayingback())
				return;

			if ((m_pActivePanel != NULL) && (m_pActivePanel != pPanel) && (m_pActivePanel->IsVisible()))
			{
				m_pLastActivePanel = m_pActivePanel;
				m_pActivePanel->ShowPanel(false);
			}

			m_pActivePanel = pPanel;
		}
	}
	else
	{
		if (m_pActivePanel == pPanel)
			m_pActivePanel = NULL;

		if (m_pLastActivePanel)
		{
			m_pActivePanel = m_pLastActivePanel;
			m_pLastActivePanel = NULL;
			m_pActivePanel->ShowPanel(true);
		}
	}

	if (state && pPanel->NeedsUpdate())
		pPanel->Update();

	pPanel->ShowPanel(state);
}

void CViewport::UpdateAllPanels(void)
{
	int count = m_Panels.Count();

	for (int i = 0; i < count; i++)
	{
		CViewPortPanel *p = m_Panels[i];

		if (p->IsVisible() && p->NeedsUpdate())
			p->Update();
	}
}

void CViewport::RemoveAllPanels(void)
{
	for (int i = 0; i < m_Panels.Count(); i++)
	{
		VPANEL vPanel = m_Panels[i]->GetVPanel();
		ipanel()->DeletePanel(vPanel);
	}


	m_Panels.Purge();
	m_pActivePanel = NULL;
	m_pLastActivePanel = NULL;
}

bool CViewport::AllowedToPrintText(void)
{
	if (!gViewPortInterface->AllowedToPrintText())
		return false;

	//if (m_pTextWindow->IsVisible())
	//	return false;

	if (m_pActivePanel)
	{
		if (m_pActivePanel->IsVisible() && m_pActivePanel->HasInputElements())
			return false;
	}

	return true;
}