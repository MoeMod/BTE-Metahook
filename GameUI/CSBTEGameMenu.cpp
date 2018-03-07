#include "base.h"
#include "BaseUI.h"
#include "CSBTEGameMenu.h"

#include "vgui_controls/QueryBox.h"
#include "vgui_controls/TextImage.h"

extern IEngineVGui *g_pEngineVGui;

CCSBTEGameMenuButton::CCSBTEGameMenuButton(Panel *parent, const char *panelName, const char *text) : MenuButton(parent, panelName, text)
{
	
}

CCSBTEGameMenuButton::~CCSBTEGameMenuButton()
{
	
}

void CCSBTEGameMenuButton::Paint(void)
{
	BaseClass::Paint();
	return;

	if (!ShouldPaint())
		return;

	if (GetFont() == INVALID_FONT)
		return;

	int x, y;
	GetPos(x, y);

	g_FontBold.SetWidth(22);
	y -= 22;
	g_FontBold.SetColor(255, 255, 255, 255);
	g_FontBold.DrawString(L"miao", x , y, 1000);
	return;
}

void CCSBTEGameMenuButton::PaintBackground(void)
{
	//Panel::PaintBackground();

	int iState = 1;
	if (IsDepressed()) iState = 0;
	else if (IsArmed()) iState = 2;
	
	if (iState == 0 || iState == 2)
	{
		BaseClass::PaintBackground();
	}
}

CCSBTEGameMenu::CCSBTEGameMenu(Panel *parent, const char *panelName) : Menu(parent, panelName)
{
	/*
	SetCloseButtonVisible(false);
	SetMenuButtonVisible(false);
	SetSizeable(false);
	SetMoveable(false);
	SetTitleBarVisible(false);
	*/
	int swide, stall;
	surface()->GetScreenSize(swide, stall);
	SetBounds(swide / 12, stall / 3, swide / 4, stall / 16 * 4);
	

	int x = 0;
	int y = 0;
	int w = swide / 4;
	int h = stall / 16;

	m_pNewGame = new CCSBTEGameMenuButton(this, "CSBTEGameMenuButton", "#Cstrike_NewMultiplayerGame");
	m_pNewGame->SetBounds(x, y + h * 0, w, h);
	m_pNewGame->SetCommand("OpenCreateMultiplayerGameDialog");
	m_pNewGame->SetMenu(this);
	m_pNewGame->AddActionSignalTarget(this);

	m_pServerBrowser = new CCSBTEGameMenuButton(this, "CSBTEGameMenuButton", "#GameUI_GameMenu_FindServers");
	m_pServerBrowser->SetBounds(x, y + h * 1, w, h);
	m_pServerBrowser->SetCommand("");
	m_pServerBrowser->SetMenu(this);
	m_pServerBrowser->AddActionSignalTarget(this);

	m_pOptions = new CCSBTEGameMenuButton(this, "CSBTEGameMenuButton", "#GameUI_GameMenu_Options");
	m_pOptions->SetBounds(x, y + h * 2, w, h);
	m_pOptions->SetCommand("OpenOptionsDialog");
	m_pOptions->SetMenu(this);
	m_pOptions->AddActionSignalTarget(this);

	m_pExit = new CCSBTEGameMenuButton(this, "CSBTEGameMenuButton", "#GameUI_GameMenu_Quit");
	m_pExit->SetBounds(x, y + h * 3, w, h);
	m_pExit->SetCommand("Quit");
	m_pExit->SetMenu(this);
	m_pExit->AddActionSignalTarget(this);
}

CCSBTEGameMenu::~CCSBTEGameMenu()
{
	delete m_pNewGame;
	delete m_pServerBrowser;
	delete m_pOptions;
	delete m_pExit;
}

void CCSBTEGameMenu::PaintBackground()
{
	//gEngfuncs.pfnFillRGBA(0, 0, ScreenWidth, ScreenHeight, 75, 75, 75, 75);
	//BaseClass::PaintBackground();
}

extern void __fastcall CBasePanel_RunMenuCommand(void *pthis, int, const char *command);

void CCSBTEGameMenu::OnCommand(const char *command)
{
	if (!stricmp(command, "OpenCreateMultiplayerGameDialog"))
	{
		OnOpenCreateMultiplayerGameDialog();
	}
	else if (!stricmp(command, "OpenOptionsDialog"))
	{
		OnOpenOptionsDialog();
	}
	else if (!stricmp(command, "Quit"))
	{
		OnOpenQuitConfirmationDialog();
	}
	else if (!Q_stricmp(command, "QuitNoConfirm"))
	{
		// hide everything while we quit
		SetVisible(false);
		vgui::surface()->RestrictPaintToSinglePanel(GetVPanel());
		engine->pfnClientCmd("quit\n");
		return;
	}
}

void CCSBTEGameMenu::PositionDialog(vgui::PHandle dlg)
{
	if (!dlg.Get())
		return;

	int x, y, ww, wt, wide, tall;
	vgui::surface()->GetWorkspaceBounds(x, y, ww, wt);
	dlg->GetSize(wide, tall);
	dlg->SetPos(x + ((ww - wide) / 2), y + ((wt - tall) / 2));
}

void CCSBTEGameMenu::OnOpenCreateMultiplayerGameDialog(void)
{
	if (!m_hCreateMultiplayerGameDialog.Get())
	{
		VPANEL GameUIPanel = g_pEngineVGui->GetPanel(PANEL_GAMEUIDLL);

		m_hCreateMultiplayerGameDialog = new CCreateMultiplayerGameDialog(NULL);
		m_hCreateMultiplayerGameDialog->SetParent(GameUIPanel);
		PositionDialog(m_hCreateMultiplayerGameDialog);
	}

	m_hCreateMultiplayerGameDialog->Activate();

}

void CCSBTEGameMenu::OnOpenOptionsDialog(void)
{
	if (!m_hOptionsDialog.Get())
	{
		VPANEL GameUIPanel = g_pEngineVGui->GetPanel(PANEL_GAMEUIDLL);

		m_hOptionsDialog = new COptionsDialog(NULL);
		m_hOptionsDialog->SetParent(GameUIPanel);
		PositionDialog(m_hOptionsDialog);
	}

	m_hOptionsDialog->Activate();

}

void CCSBTEGameMenu::OnOpenQuitConfirmationDialog()
{
	VPANEL GameUIPanel = g_pEngineVGui->GetPanel(PANEL_GAMEUIDLL);

	QueryBox *box = new QueryBox("#GameUI_QuitConfirmationTitle", "#GameUI_QuitConfirmationText", NULL);
	box->SetParent(GameUIPanel);
	box->SetOKButtonText("#GameUI_Quit");
	box->SetOKCommand(new KeyValues("Command", "command", "QuitNoConfirm"));
	box->SetCancelCommand(new KeyValues("Command", "command", "ReleaseModalWindow"));
	box->AddActionSignalTarget(this);
	box->DoModal();
	vgui::surface()->RestrictPaintToSinglePanel(box->GetVPanel());
}