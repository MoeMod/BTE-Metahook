#include "base.h"
#include "CSBTEModeSelection.h"
#include <vgui_controls\PHandle.h>
#include "common.h"
#include <io.h>

CCSBTEModeSelection::CCSBTEModeSelection(Panel *parent, const char *panelName, bool showTaskbarIcon) : Frame(parent, panelName, showTaskbarIcon)
{
	int sw, sh;
	surface()->GetScreenSize(sw, sh);

	SetTitle("#CSBTE_ModeSelection", false);

	SetSize(740, 700);
	MoveToCenterOfScreen();
	SetSizeable(false);
	SetVisible(true);

	char szPlugin[128];
	char szTGA[128];
	char szName[128];

	for (int i = 0; i < MAX_GAMEMODES; i++)
	{
		GetPrivateProfileString(va("GameMode%d", i), "Plugin", "", szPlugin, 127, "cstrike\\gamemodes.ini");
		GetPrivateProfileString(va("GameMode%d", i), "Image", "", szTGA, 127, "cstrike\\gamemodes.ini");
		GetPrivateProfileString(va("GameMode%d", i), "Name", "", szName, 127, "cstrike\\gamemodes.ini");

		if (!szPlugin[0])
			continue;

		m_pModeImage[i] = new ImagePanel(this, "CSBTEModeImage");
		int x = 25 + (i % 3) * 235;
		int y = 50 + (i / 3) * 150;
		m_pModeImage[i]->SetImage(scheme()->GetImage(szTGA, false));
		m_pModeImage[i]->SetBounds(x, y, 214, 128);

		char szCommand[12 + MAX_GAMEMODES / 10];
		sprintf(szCommand, "OnSetMode_%d", i);
		m_pModeButton[i] = new Button(this, "CSBTEModeButton", szName);
		m_pModeButton[i]->SetContentAlignment(Label::a_center);
		m_pModeButton[i]->SetBounds(x + 32, y + 92, 150, 24);
		m_pModeButton[i]->SetCommand(szCommand);
	}
	m_pInfoLabel = new Label(this, "InfoLabel", "#CSBTE_ModeSelection_Desc");
	m_pInfoLabel->SetPaintBackgroundEnabled(false);
	m_pInfoLabel->SetBounds(20, 640, 700, 60);

	m_pCancelButton = new Button(this, "CancelButton", "#GameUI_Close");
	m_pCancelButton->SetContentAlignment(Label::a_center);
	m_pCancelButton->SetBounds(260, 520, 200, 50);
	m_pCancelButton->SetCommand("vguicancel");
	m_pCancelButton->SetVisible(false);

	m_pSelectedImage = NULL;
	UpdateCurrentMode();
}

void CCSBTEModeSelection::UpdateCurrentMode()
{
	int iCurMode = -1;
	char szPlugin[128];
	char szFilename[128];
	for (int i = 0; i < MAX_GAMEMODES; i++)
	{
		GetPrivateProfileString(va("GameMode%d", i), "Plugin", "", szPlugin, 127, "cstrike\\gamemodes.ini");
		if (!szPlugin[0])
			continue;
		sprintf(szFilename, "cstrike\\addons\\amxmodx\\configs\\plugins-%s.ini", szPlugin);
		if (access(szFilename, 4) != -1)
		{
			if (iCurMode == -1)
				iCurMode = i;
			else
				iCurMode = -2;
		}
		m_pModeButton[i]->ForceDepressed(false);
	}

	if (m_pSelectedImage == NULL)
	{
		m_pSelectedImage = new ImagePanel(this, "CSBTEModeImageSelected");
		m_pSelectedImage->SetImage(scheme()->GetImage("resource/control/bgglow", false));
		m_pSelectedImage->SetShouldScaleImage(true);
	}
	if (iCurMode >= 0)
	{
		int x = 25 + (iCurMode % 3) * 235;
		int y = 50 + (iCurMode / 3) * 150;
		m_pSelectedImage->SetBounds(x, y, 216, 128);
		m_pModeButton[iCurMode]->ForceDepressed(true);
	}
}


void CCSBTEModeSelection::OnCommand(const char *command)
{
	if (!Q_stricmp(command, "vguicancel"))
	{
		Close();
	}
	else if (!Q_strncmp(command, "OnSetMode_", 10))
	{

		int i;
		sscanf(command, "OnSetMode_%d", &i);

		char szMode[16], szPath[128], szFilename2[128];

		_finddata_t fileDir;
		long lfDir;
		if ((lfDir = _findfirst(".\\cstrike\\addons\\amxmodx\\configs\\plugins-*.ini", &fileDir)) != -1l)
		{
			do
			{
				if (sscanf(fileDir.name, "plugins-%s", szMode) == 1)
				{
					szMode[strlen(szMode) - 4] = '\0';
					sprintf(szPath, "cstrike\\addons\\amxmodx\\configs\\plugins-%s.ini", szMode);
					sprintf(szFilename2, "cstrike\\addons\\amxmodx\\configs\\disabled-%s.ini", szMode);
					rename(szPath, szFilename2);
				}

			} while (_findnext(lfDir, &fileDir) == 0);
		}
		_findclose(lfDir);

		char szPlugin[128];
		GetPrivateProfileString(va("GameMode%d", i), "Plugin", "", szPlugin, 127, "cstrike\\gamemodes.ini");

		sprintf(szPath, "cstrike\\addons\\amxmodx\\configs\\disabled-%s.ini", szPlugin);
		sprintf(szFilename2, "cstrike\\addons\\amxmodx\\configs\\plugins-%s.ini", szPlugin);


		if (!szPlugin[0] || rename(szPath, szFilename2) != 0)
		{
			MessageBox *msgbox;
			msgbox = new MessageBox("", "#CSBTE_ModeSelection_Error", this);
			msgbox->SetOKButtonText("#GameUI_OK");
			msgbox->SetOKButtonVisible(true);
			msgbox->SetBounds(GetWide() / 2 - 150, GetTall() / 2 - 100, 300, 200);

			msgbox->Activate();
		}

		UpdateCurrentMode();



	}
	else
		BaseClass::OnCommand(command);
}