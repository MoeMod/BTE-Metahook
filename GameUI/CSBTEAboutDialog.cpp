
#include "hud.h"
#include "configs.h"
#include "CSBTEAboutDialog.h"
#include "EngineInterface.h"
#include <vgui_controls\PHandle.h>

#include "Client/HUD/hud_saytext.h"

static void(*pfnUnknown)() = (void(*)())0x1960870;

CCSBTEAboutDialog::CCSBTEAboutDialog(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetTitle("", false);

	SetBounds(sw / 2 - 200, sh / 2 - 150, 450, 200);
	SetSizeable(false);
	SetVisible(true);


	m_pLogo = new ImagePanel(this, "CSBTELogo");
	m_pLogo->SetImage(scheme()->GetImage("resource/game_menu_mouseover2", false));

	m_pLogo->SetBounds(130, 25, 300, 40);

	m_pMoreInfoLabel = new Label(this, "MoreInfoLabel", "#CSBTE_About_MoreInformation");
	m_pMoreInfoLabel->SetPaintBackgroundEnabled(false);
	m_pMoreInfoLabel->SetBounds(10, 70, 200, 20);

	m_pCsoldjbURL = new URLLabel(this, "CsoldjbTiebaURL", "#CSBTE_About_DjbBar", "http://tieba.baidu.com/csoldjb");
	m_pCsoldjbURL->SetPaintBackgroundEnabled(false);
	if (Q_stricmp(gConfigs.szLanguage, "english"))
		m_pCsoldjbURL->SetBounds(180, 68, 150, 24);
	else
		m_pCsoldjbURL->SetBounds(200, 68, 150, 24);

	wchar_t *wstrSwitch[2] = { localize()->Find("#CSBTE_SwitchOn"), localize()->Find("#CSBTE_SwitchOff") };

	if (!wstrSwitch[0] || wstrSwitch[0][0] == L'#')
		wstrSwitch[0] = L"On";
	if (!wstrSwitch[1] || wstrSwitch[1][0] == L'#')
		wstrSwitch[1] = L"Off";

	m_pNewHUDSwitchLabel = new Label(this, "NewHUDSwitchLabel", "#CSBTE_NewHUDLabel");
	m_pNewHUDSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pNewHUDSwitchLabel->SetBounds(10, 90, 100, 20);

	m_pNewHUDSwitch = new ComboBox(this, "NewHUDSwitch", 2, false);
	m_pNewHUDSwitch->AddItem(wstrSwitch[0], NULL);
	m_pNewHUDSwitch->AddItem(wstrSwitch[1], NULL);
	m_pNewHUDSwitch->ActivateItem(gConfigs.bEnableNewHud ? 0 : 1);
	m_pNewHUDSwitch->SetBounds(10, 120, 100, 24);

	m_pNewMuzzleflashSwitchLabel = new Label(this, "NewMuzzleFlashSwitchLabel", "#CSBTE_NewMuzzleFlashLabel");
	m_pNewMuzzleflashSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pNewMuzzleflashSwitchLabel->SetBounds(120, 90, 100, 20);

	m_pNewMuzzleflashSwitch = new ComboBox(this, "NewMuzzleFlashSwitch", 2, false);
	m_pNewMuzzleflashSwitch->AddItem(wstrSwitch[0], NULL);
	m_pNewMuzzleflashSwitch->AddItem(wstrSwitch[1], NULL);
	m_pNewMuzzleflashSwitch->ActivateItem(gConfigs.bEnableNewMuzzle ? 0 : 1);
	m_pNewMuzzleflashSwitch->SetBounds(120, 120, 100, 24);

	m_pFireDlightSwitchLabel = new Label(this, "FireDlightSwitchLabel", "#CSBTE_FireDlightLabel");
	m_pFireDlightSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pFireDlightSwitchLabel->SetBounds(230, 90, 100, 20);

	m_pFireDlightSwitch = new ComboBox(this, "FireDlightSwitch", 2, false);
	m_pFireDlightSwitch->AddItem(wstrSwitch[0], NULL);
	m_pFireDlightSwitch->AddItem(wstrSwitch[1], NULL);
	m_pFireDlightSwitch->ActivateItem(gConfigs.bEnableFireDlight ? 0 : 1);
	m_pFireDlightSwitch->SetBounds(230, 120, 100, 24);

	m_pBloodSwitchLabel = new Label(this, "BloodSwitchLabel", "#CSBTE_BloodLabel");
	m_pBloodSwitchLabel->SetPaintBackgroundEnabled(false);
	m_pBloodSwitchLabel->SetBounds(340, 90, 100, 20);

	m_pBloodSwitch = new ComboBox(this, "BloodSwitch", 2, false);
	m_pBloodSwitch->AddItem(wstrSwitch[0], NULL);
	m_pBloodSwitch->AddItem(wstrSwitch[1], NULL);
	m_pBloodSwitch->ActivateItem(gConfigs.bEnableBlood ? 0 : 1);
	m_pBloodSwitch->SetBounds(340, 120, 100, 24);

	m_pLanguageSelectionLabel = new Label(this, "LanguageSelectionLabel", "#CSBTE_LanguageLabel");
	m_pLanguageSelectionLabel->SetPaintBackgroundEnabled(false);
	m_pLanguageSelectionLabel->SetBounds(10, 150, 100, 24);

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
	m_pLanguageSelection->SetBounds(120, 150, 100, 24);

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
	m_pOKButton->SetCommand("SaveConfigs");

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Cancel");
	m_pCancelButton->SetBounds(340, 150, 100, 24);
	m_pCancelButton->SetCommand("vguicancel");
}


void CCSBTEAboutDialog::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "SaveConfigs"))
	{
		if (!m_pNewHUDSwitch->GetActiveItem())
		{
			if (!gConfigs.bEnableNewHud)
				pfnUnknown();
			gConfigs.bEnableNewHud = 1;
		}
		else
		{
			if (gConfigs.bEnableNewHud)
			{
				line *pLine = HudSayText().head;
				while (pLine)
				{
					HudSayText().head = pLine;
					pLine = pLine->next;
					free(HudSayText().head);
					HudSayText().head = NULL;
				}
			}
			gConfigs.bEnableNewHud = 0;
		}

		if (!m_pNewMuzzleflashSwitch->GetActiveItem())
			gConfigs.bEnableNewMuzzle = 1;
		else
			gConfigs.bEnableNewMuzzle = 0;

		if (!m_pFireDlightSwitch->GetActiveItem())
			gConfigs.bEnableFireDlight = 1;
		else
			gConfigs.bEnableFireDlight = 0;

		if (!m_pBloodSwitch->GetActiveItem())
			gConfigs.bEnableBlood = 1;
		else
			gConfigs.bEnableBlood = 0;

		bool bClose = true;

		if (m_pLanguageSelection->GetActiveItem() != 3)
		{
			if (
				(m_pLanguageSelection->GetActiveItem() == 0 && Q_stricmp(gConfigs.szLanguage, "schinese")) ||
				(m_pLanguageSelection->GetActiveItem() == 1 && Q_stricmp(gConfigs.szLanguage, "tchinese")) ||
				(m_pLanguageSelection->GetActiveItem() == 2 && Q_stricmp(gConfigs.szLanguage, "english"))
				)
			{
				bClose = false;

				CCSBTEAboutDialogMessageBox *msgbox;
				msgbox = new CCSBTEAboutDialogMessageBox(L"Warning", L"The language is only changed when you next run the game.\n* Your Steam Language, the game launcher and weapon editor's language will also be changed here\n* The Localized String Table File for languages except Chinese Simplified may not be full.", this);
				msgbox->SetOKButtonText(L"Yes");
				msgbox->SetCancelButtonText(L"Cancel");
				msgbox->SetOKButtonVisible(true);
				msgbox->SetCancelButtonVisible(true);
				msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);

				msgbox->Activate();

				SetKeyBoardInputEnabled(false);
				SetMouseInputEnabled(false);
			}
		}

		Config_Save();
		if (bClose)
			Close();
	}
	else if (!Q_stricmp(command, "vguicancel"))
		Close();
	else if (!Q_stricmp(command, "OnOK"))
	{
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
		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);
		Close();

		// Force restart of entire engine
		engine->pfnClientCmd("fmod stop\n");
		engine->pfnClientCmd("_restart\n");
	}
	else if (!Q_stricmp(command, "OnCancel"))
	{
		SetKeyBoardInputEnabled(true);
		SetMouseInputEnabled(true);
	}
	else
		BaseClass::OnCommand(command);
}