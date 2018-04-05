
#include "configs.h"
#include "CSBTEWelcomeDialog.h"
#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

CCSBTEWelcomeDialog::CCSBTEWelcomeDialog(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	SetTitle("", false);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetDragEnabled(false);
	SetMoveable(false);
	
	int swide, stall;
	vgui::surface()->GetScreenSize(swide, stall);
	SetBounds((swide - 450) / 2, (stall - 200) * 4 / 5, 450, 200);
	SetSizeable(false);
	SetVisible(true);

	m_pLogo = new ImagePanel(this, "CSBTELogo");
	m_pLogo->SetImage(scheme()->GetImage("resource/game_menu_mouseover2", false));
	
	m_pLogo->SetBounds(130, 25, 300, 40);


	m_pInfoLabel = new Label(this, "MoreInfoLabel", "#CSBTE_WelcomeMessage");
	m_pInfoLabel->SetPaintBackgroundEnabled(false);
	m_pInfoLabel->SetBounds(100, 70, 200, 30);
	m_pInfoLabel->SetContentAlignment(Label::a_center);

	m_pLanguageSelectionLabel = new Label(this, "LanguageSelectionLabel", "#CSBTE_LanguageLabel");
	m_pLanguageSelectionLabel->SetPaintBackgroundEnabled(false);
	m_pLanguageSelectionLabel->SetBounds(100, 110, 100, 24);

	m_pLanguageSelection = new ComboBox(this, "LanguageSelection", 3, false);
	if (Q_stricmp(gConfigs.szLanguage, "tchinese"))
	{
		m_pLanguageSelection->AddItem(L"简体中文", NULL);
		m_pLanguageSelection->AddItem(L"繁体中文", NULL);
	}
	else
	{
		m_pLanguageSelection->AddItem(L"簡體中文", NULL);
		m_pLanguageSelection->AddItem(L"繁體中文", NULL);
	}

	m_pLanguageSelection->AddItem(L"English", NULL);
	m_pLanguageSelection->SetBounds(230, 110, 100, 24);

	if (!Q_stricmp(gConfigs.szLanguage, "schinese"))
		m_pLanguageSelection->ActivateItem(0);
	else if (!Q_stricmp(gConfigs.szLanguage, "tchinese"))
		m_pLanguageSelection->ActivateItem(1);
	else if (!Q_stricmp(gConfigs.szLanguage, "english"))
		m_pLanguageSelection->ActivateItem(2);
	else
	{
		m_pLanguageSelection->SetNumberOfEditLines(4);
		m_pLanguageSelection->AddItem(gConfigs.szLanguage, NULL);
		m_pLanguageSelection->ActivateItem(3);
	}

	m_pOKButton = new Button(this, "OKButton", "#GameUI_OK");
	m_pOKButton->SetBounds(230, 150, 100, 24);
	m_pOKButton->SetCommand("OnOK");

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Quit");
	m_pCancelButton->SetBounds(340, 150, 100, 24);
	m_pCancelButton->SetCommand("Close");
}


void CCSBTEWelcomeDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "OnOK"))
	{
		//Config_Save();

		switch (m_pLanguageSelection->GetActiveItem())
		{
		case 0:
			//Q_strcpy(gConfigs.szLanguage, "schinese");
			system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", "schinese");
			break;
		case 1:
			//Q_strcpy(gConfigs.szLanguage, "tchinese");
			system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", "tchinese");
			break;
		case 2:
			//Q_strcpy(gConfigs.szLanguage, "english");
			system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Steam\\Language", "english");
			break;
		}

		// Set Key
		system()->SetRegistryString("HKEY_CURRENT_USER\\Software\\Valve\\Half-Life\\Settings\\ValveKey", "5RP2E-EPH3K-BR3LG-KMGTE-FN8PY");

		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);
		Close();

		// Force restart of entire engine
		engine->pfnClientCmd("fmod stop\n");
		engine->pfnClientCmd("_restart\n");
	}
	else if (!stricmp(command, "Cancel") || !stricmp(command, "Close"))
	{
		BaseClass::OnCommand("Close");
	}
	else
		BaseClass::OnCommand(command);
}

void CCSBTEWelcomeDialog::OnClose()
{
	engine->pfnClientCmd("quit\n");
	

	BaseClass::OnClose();
	MarkForDeletion();
}