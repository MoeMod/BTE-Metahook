#include "bte_const.h"
#include "metahook.h"
#include "exportfuncs.h"
#include "hud.h"
#include "BaseUI.h"
#include "DrawTargaImage.h"
#include "weapons.h"
#include "util.h"
#include "Encode.h"
#include "common.h"

#include "mgui.h"
#include "BTEPanel.h"
#include "TeamMenu.h"
#include "NormalZombieMenu.h"

#include "Client/PlayerClassManager.h"

#include <list>

CCSTeamMenu g_TeamMenu;

extern MGUI_Panel *pTeamSuitPanel, *pBuyMenuPanel;

void CCSTeamMenu::Init(void)
{
	//gEngfuncs.pfnAddCommand("chooseteam", Cmd_ChooseTeam);

	GL_LoadPanel(m_iColumnTexture[0], "column\\n");
	GL_LoadPanel(m_iColumnTexture[1], "column\\o");
	GL_LoadPanel(m_iColumnTexture[2], "column\\c");

	GL_LoadPanel(m_iButtonTexture[0], "button\\n");
	GL_LoadPanel(m_iButtonTexture[1], "button\\o");
	GL_LoadPanel(m_iButtonTexture[2], "button\\c");
	
	m_iCurrentTeam = 2;

	BuildBaseFrame();

	Close();
}
/*
void Cmd_ChooseTeam()
{
	g_mgui_candraw = 1;
	g_TeamMenu.Show();
}
*/
char TeamSelections[4][128] = 
{
	"#Cstrike_TitlesTXT_Unassigned",
	"#Cstrike_TitlesTXT_Terrorist_Forces",
	"#Cstrike_TitlesTXT_CT_Forces",
	"#Cstrike_TitlesTXT_Spectators"
};

void CCSTeamMenu::BuildBaseFrame()
{
	//m_pPanel = MGUI_CreatePanel(int(ScreenWidth * 0.029), int(ScreenHeight * 0.062), int(ScreenWidth * 0.95), int(ScreenHeight * 0.86), 20, false, true);
	m_pPanel = MGUI_CreatePanel(0, 0, 0, 0, 22, false, false);
	wcscpy(m_pPanel->m_pTitle, GetLangUni("#Cstrike_Join_Class"));
	m_pPanel->m_iTitleSize = 18;
	m_pPanel->m_bHasClose = FALSE;
	
	int iX = m_pPanel->x, iY = m_pPanel->y;
	int i;
	iX += 21;
	iY += 54;
	for (i = 0; i < 2; i++)
	{
		m_pButtonTeamSlot[i] = MGUI_CreateButton(m_pPanel, 0, 0, 0, 0);
		m_pButtonTeamSlot[i]->SetEnabled(true);
		m_pButtonTeamSlot[i]->SetLabel(GetLangUni(TeamSelections[i+1]));
		m_pButtonTeamSlot[i]->SetCommand(va("MGUI.TeamMenuSwitchTeam%d", i + 1));
		m_pButtonTeamSlot[i]->m_iSize = 10;
		m_pButtonTeamSlot[i]->m_Tex_Type = MGUI_TEX_TYPE_3;
		memcpy(m_pButtonTeamSlot[i]->iTga3, m_iColumnTexture, sizeof(m_iColumnTexture));
	}

	// Page
	for (int i = 0; i <= 1; i++)
	{
		m_pButtonPage[i] = MGUI_CreateButton(m_pPanel, 0, 0, 0, 0);
		m_pButtonPage[i]->SetType(MGUI_BUTTON_TYPE_COMMAND);
		m_pButtonPage[i]->SetVisible(true);
		//pBuyMenuButtonPage[i]->SetImage3(g_UI_Button);
		memcpy(m_pButtonPage[i]->iTga3, m_iButtonTexture, sizeof(m_iButtonTexture));
		m_pButtonPage[i]->m_iSize = 14;
		m_pButtonPage[i]->m_Tex_Type = MGUI_TEX_TYPE_3;
	}
	m_pButtonPage[0]->SetLabel(GetLangUni("#CSO_PrevBuy"));
	m_pButtonPage[1]->SetLabel(GetLangUni("#CSO_NextBuy"));
	m_pButtonPage[0]->SetCommand("MGUI.TeamMenuPrevSlot");
	m_pButtonPage[1]->SetCommand("MGUI.TeamMenuNextSlot");
	m_pButtonPage[0]->SetKey('-');
	m_pButtonPage[1]->SetKey('=');
	
	for (i = 0; i < 10; i++)
	{
		m_pButtonSlot[i] = MGUI_CreateButton(m_pPanel, 0, 0, 0, 0);
		m_pButtonSlot[i]->SetImage("gfx\\ui\\panel\\blank_slot", "gfx\\ui\\panel\\select", "gfx\\ui\\panel\\select");
		m_pButtonSlot[i]->SetKey('0' + (i+1) % 10);

		m_pButtonSlotLabel[i] = MGUI_CreateLabel(m_pPanel, 0, 0, 0, 0);
		m_pButtonSlotLabel[i]->SetSize(14);
		m_pButtonSlotLabel[i]->SetColor(255, 255, 255, 255);

		m_pButtonSlotKeyImage[i] = MGUI_CreateImage(m_pPanel, 0, 0, 0, 0, true, false);
		m_pButtonSlotKeyImage[i]->SetTexture("gfx\\ui\\panel\\key");

		m_pButtonSlotKeyLabel[i] = MGUI_CreateLabel(m_pPanel, 0, 0, 0, 0);
		m_pButtonSlotKeyLabel[i]->SetSize(14);
		m_pButtonSlotKeyLabel[i]->SetColor(255, 255, 255, 255);

		m_pPanelInfo[i] = MGUI_CreatePanel(0, 0, 0, 0, 0, false);
		m_pPanelInfo[i]->m_pTitle[0] = '\0';
		m_pPanelInfo[i]->m_iTitleSize = 18;
		m_pPanelInfo[i]->m_bHasClose = false;
		m_pPanelInfo[i]->m_iHaveBg = 0;

		m_pImageInfo[i] = MGUI_CreateImage(m_pPanelInfo[i], 0, 0, 0, 0, true, false);
		m_pLabelInfo[i] = MGUI_CreateLabel(m_pPanelInfo[i], 0, 0, 0, 0);
		m_pLabelInfo[i]->SetColor(150, 100, 20, 255);
	}
}

void CCSTeamMenu::Reset(void)
{
	float scale = (float)ScreenHeight / 480;
	m_pPanel->x = (int)((ScreenWidth - 606 * scale) / 2);
	m_pPanel->prevx = m_pPanel->x;
	m_pPanel->y = (int)((ScreenHeight - 420 * scale) / 2);
	m_pPanel->prevy = m_pPanel->y;
	m_pPanel->w = 596.875 * scale;	//(int)(606 * scale);
	m_pPanel->h = 413.125 * scale;//(int)(420 * scale);

	for (int i = 0; i < 2; i++)
	{
		if (!i)
			m_pButtonTeamSlot[i]->prevx = m_pButtonTeamSlot[i]->x = int(15.625 * scale);
		else
			m_pButtonTeamSlot[i]->prevx = m_pButtonTeamSlot[i]->x = int(15.625 * scale) + 12 * wcslen(m_pButtonTeamSlot[i - 1]->pszLabel) *i;
		m_pButtonTeamSlot[i]->prevy = m_pButtonTeamSlot[i]->y = int(50 * scale) - 20;
		m_pButtonTeamSlot[i]->w = 12 * wcslen(m_pButtonTeamSlot[i]->pszLabel);
		m_pButtonTeamSlot[i]->h = 20;
		m_pButtonTeamSlot[i]->SetColor(255, 255, 255, 255);
	}

	for (int i = 0; i < 10; i++)
	{
		m_pButtonSlot[i]->prevx = m_pButtonSlot[i]->x = int(15.625 * scale);
		m_pButtonSlot[i]->prevy = m_pButtonSlot[i]->y = int((50 + 25.625 * i) * scale);
		m_pButtonSlot[i]->w = int(200 * scale);
		m_pButtonSlot[i]->h = int(25 * scale);

		m_pButtonSlotLabel[i]->x = int(37.5 * scale);
		m_pButtonSlotLabel[i]->y = int((50.625 + 25.625 * i + 6.25) * scale);
		m_pButtonSlotLabel[i]->w = int(140 * scale);
		m_pButtonSlotLabel[i]->h = int(8.75 * scale);
		m_pButtonSlotLabel[i]->SetSize(14);

		m_pButtonSlotKeyImage[i]->x = int(17.5 * scale);
		m_pButtonSlotKeyImage[i]->y = int((55 + 25.625 * i) * scale);
		m_pButtonSlotKeyImage[i]->w = m_pButtonSlotKeyImage[i]->h = int(17.5 * scale);

		m_pButtonSlotKeyLabel[i]->x = int(23.75 * scale);
		m_pButtonSlotKeyLabel[i]->y = int((48 + 25.625 * i + 3.75) * scale);
		m_pButtonSlotKeyLabel[i]->w = int(203 * scale);
		m_pButtonSlotKeyLabel[i]->h = int(24.375 * scale);
		m_pButtonSlotKeyLabel[i]->SetSize(14);

		m_pPanelInfo[i]->x = m_pPanel->x + int(260 * scale);
		m_pPanelInfo[i]->y = m_pPanel->y + int(50 * scale);
		m_pPanelInfo[i]->w = int(256 * scale);
		m_pPanelInfo[i]->h = int(256 * scale);

		m_pImageInfo[i]->x = m_pImageInfo[i]->y = 0;
		m_pImageInfo[i]->w = int(256 * scale);
		m_pImageInfo[i]->h = int(196 * scale);

		m_pLabelInfo[i]->x = 0;
		m_pLabelInfo[i]->y = int(200 * scale);
		m_pLabelInfo[i]->w = int(256 * scale);
		m_pLabelInfo[i]->h = int(46 * scale);
		m_pLabelInfo[i]->SetSize(15);

	}

	m_pButtonPage[0]->w = m_pButtonPage[1]->w = 97;
	m_pButtonPage[0]->h = m_pButtonPage[1]->h = 25;

	m_pButtonPage[0]->x = m_pButtonSlot[0]->x + 36;
	m_pButtonPage[1]->x = m_pButtonSlot[0]->x + m_pButtonSlot[0]->w - 141;
	m_pButtonPage[0]->y = m_pButtonPage[1]->y = m_pButtonSlot[9]->y + m_pButtonSlot[0]->h + 8;
}

void CCSTeamMenu::UpdateItems(void)
{
	m_Classes.clear();
	for (auto &x : PlayerClassManager())
	{
		if (x.team != m_iCurrentTeam)
			continue;
		m_Classes.push_back(&x);
	}
	/*
	if (g_iMod == MOD_NONE || g_iMod == MOD_TDM || g_iMod == MOD_ZBU || g_iMod == MOD_GHOST || g_iMod == MOD_GD)
	{
		for (int i = 0; i < 2; i++)
		{
			m_pButtonTeamSlot[i]->SetVisible(false);
		}
	}
	else
	{*/
		for (int i = 0; i < 2; i++)
		{
			m_pButtonTeamSlot[i]->SetVisible(true);
		}
	//}

	float scale = (float)ScreenHeight / 480;
	for (int i = 0; i < 2; i++)
	{
		m_pButtonTeamSlot[i]->prevy = m_pButtonTeamSlot[i]->y = int(50 * scale) - 20;
		m_pButtonTeamSlot[i]->SetColor(255, 255, 255, 255);
		if (i + 1 == m_iCurrentTeam)
		{
			if (m_iCurrentTeam == 2)
				m_pButtonTeamSlot[i]->SetColor(192, 205, 224, 255);
			else if (m_iCurrentTeam == 1)
				m_pButtonTeamSlot[i]->SetColor(216, 182, 183, 255);

			m_pButtonTeamSlot[i]->y -= 3;
		}
	}
	SwitchToPage(0);
	
}

void CCSTeamMenu::Command(char *szCommand)
{
	if (strstr(szCommand, "TeamMenuPrevSlot"))
	{
		SwitchToPage(m_iCurrentPage - 1);
	}
	else if (strstr(szCommand, "TeamMenuNextSlot"))
	{
		SwitchToPage(m_iCurrentPage + 1);
	}
	else if (strstr(szCommand, "TeamMenuSwitchTeam"))
	{
		int iTeam = 0;
		sscanf(szCommand, "MGUI.TeamMenuSwitchTeam%d", &iTeam);
		SwitchTeam(iTeam);
	}
}

void CCSTeamMenu::SwitchTeam(int iTeam)
{
	if (iTeam < 0 || iTeam > 3)
		return;
	m_iCurrentTeam = iTeam;
	UpdateItems();
}

void CCSTeamMenu::SwitchToPage(unsigned iPage)
{
	if (iPage < 0) iPage = 0;
	if (iPage > m_Classes.size() / 9) iPage = m_Classes.size() / 9;
	m_iCurrentPage = iPage;
	
	wchar_t pKey[3];
	for (int i = 0; i < 10; i++)
	{
		unsigned iClassID = i + m_iCurrentPage * 9;
		m_pButtonSlot[i]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);

		if (i != 9)
		{
			if (iClassID < m_Classes.size())
			{
				auto &x = *(m_Classes[iClassID]);
				m_pButtonSlot[i]->m_OverPanel = m_pPanelInfo[i];
				m_pButtonSlot[i]->SetCommand(va("jointeam %d; joinclass %d; bte_choose_player %s; ", m_iCurrentTeam,1, x.model));
				m_pButtonSlotLabel[i]->SetLabel(GetClassNameFormat(x.model));
				
				m_pLabelInfo[i]->SetLabel(GetClassInfoFormat(x.model));
				m_pImageInfo[i]->SetTexture(va("gfx\\vgui\\%s", x.model));

			}
			else
			{
				m_pButtonSlot[i]->m_OverPanel = NULL;
				m_pButtonSlot[i]->SetCommand("");
			}
		}
		else
		{
			m_pButtonSlot[i]->m_OverPanel = m_pPanelInfo[i];
			m_pButtonSlot[i]->SetCommand(va("jointeam %d;joinclass %d, bte_choose_player %s", m_iCurrentTeam,1, m_Classes[RANDOM_LONG(0, m_Classes.size() - 1)]->model));
			m_pButtonSlotLabel[i]->SetLabel(GetLangUni("#Cstrike_Auto_Select"));

			m_pLabelInfo[i]->SetLabel(GetLangUni("#Cstrike_AutoSelect_Label"));
			m_pImageInfo[i]->SetTexture("gfx\\vgui\\t_random");
		}

		m_pPanelInfo[i]->m_iClosing = 1;

		m_pButtonSlotKeyLabel[i]->SetColor(255, 255, 255, 255);
		wcscpy(pKey, UTF8ToUnicode(va("%d", (i + 1) % 10)));
		m_pButtonSlotKeyLabel[i]->SetLabel(pKey);

		if (m_iCurrentTeam == 2)
			m_pButtonSlotLabel[i]->SetColor(192, 205, 224, 255);
		else if (m_iCurrentTeam == 1)
			m_pButtonSlotLabel[i]->SetColor(216, 182, 183, 255);
		else
			m_pButtonSlotLabel[i]->SetColor(255, 255, 255, 255);


		if (iClassID < m_Classes.size() || (i == 9))
		{
			m_pButtonSlot[i]->SetVisible(true);
			m_pButtonSlotLabel[i]->SetVisible(true);
			m_pButtonSlotKeyImage[i]->SetVisible(true);
			m_pButtonSlotKeyLabel[i]->SetVisible(true);
		}
		else
		{
			m_pButtonSlot[i]->SetVisible(false);
			m_pButtonSlotLabel[i]->SetVisible(false);
			m_pButtonSlotKeyImage[i]->SetVisible(false);
			m_pButtonSlotKeyLabel[i]->SetVisible(false);
		}

	}

	if (!iPage)
		m_pButtonPage[0]->SetVisible(false);
	else
		m_pButtonPage[0]->SetVisible(true);

	if (iPage == m_Classes.size() / 9)
		m_pButtonPage[1]->SetVisible(false);
	else
		m_pButtonPage[1]->SetVisible(true);
}

void CCSTeamMenu::Show(void)
{
	if (pTeamSuitPanel)
		pTeamSuitPanel->m_iClosing = TRUE;
	if (pBuyMenuPanel)
		pBuyMenuPanel->m_iClosing = TRUE;
	
	g_NormalZombieMenu.Close();
	//UpdateItems();
	g_mgui_candraw = 1;
	if (m_pPanel)
		m_pPanel->m_iClosing = FALSE;
}

void CCSTeamMenu::Close(void)
{
	if (m_pPanel)
		m_pPanel->m_iClosing = TRUE;
	for (int i = 0; i < 10; i++)
		if (m_pPanelInfo[i])
			m_pPanelInfo[i]->m_iClosing = TRUE;
}