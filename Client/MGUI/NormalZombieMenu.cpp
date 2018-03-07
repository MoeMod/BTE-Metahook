#include "base.h"
#include "hud.h"
#include "parsemsg.h"
#include "BaseUI.h"
#include "DrawTargaImage.h"
#include "util.h"
#include "common.h"

#include "mgui.h"
#include "BTEPanel.h"
#include "TeamMenu.h"
#include "NormalZombieMenu.h"

#include "Client/HUD/ZombieSelect.h"

#include <list>

CNormalZombieMenu g_NormalZombieMenu;

extern MGUI_Panel *pTeamSuitPanel, *pBuyMenuPanel;

extern int iTotalClass;

wchar_t *GetZombieNameFormat(char *name)
{
	wchar_t *wstrResult = vgui::localize()->Find(va("#CSO_ZombieType_%szb", name));
	
	if (!name[0])
		wstrResult = vgui::localize()->Find(va("#CSO_ZombieType_defaultzb"));
	else if (!strnicmp(name, "z4", 2))
	{
		if (strstr(name, "humpback"))
			wstrResult = vgui::localize()->Find(va("#CSO_ZB4_Hump_Back"));
		else
			wstrResult = vgui::localize()->Find(va("#CSO_ZB4_%s", name + 2));
	}
	

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSO_Item_Name_%sZombie", name));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSO_Zombi%s", name));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSBTE_Zombi%s", name));

	return wstrResult ? wstrResult : ANSIToUnicode(va("#CSO_Zombi%s", name));
}

wchar_t *GetZombieLabelFormat(char *name)
{
	//return L" ";
	wchar_t *wstrResult = vgui::localize()->Find(va("#CSO_%sZombi_Z_Label", name));
	
	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSO_%s_Z_Label", name));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSBTE_%s_Z_Label", name));

	return wstrResult ? wstrResult : ANSIToUnicode(va("#CSO_%s_Z_Label", name));
}

int MsgFunc_ZombieMenu(const char * pszName, int iSize, void * pbuf)
{
	BEGIN_READ(pbuf, iSize);
	int iSelect = READ_BYTE();
	int iCount = READ_BYTE();

	if (!iSelect)
	{
		g_NormalZombieMenu.ClearMenu();
		for (int i = 0; i < iCount; i++)
		{
			g_NormalZombieMenu.AddItem(READ_STRING());
		}
		g_NormalZombieMenu.Show();
	}
	else
	{
		HudZBSelect().SetCount(iCount);
		for (int i = 0; i < iCount; i++)
		{
			HudZBSelect().AddZombie(READ_STRING());
		}
		HudZBSelect().SetDrawTime(cl.time);
	}

	return 1;
}

void CNormalZombieMenu::Init(void)
{
	//gEngfuncs.pfnAddCommand("chooseteam", Cmd_ChooseTeam);

	GL_LoadPanel(m_iColumnTexture[0], "column\\n");
	GL_LoadPanel(m_iColumnTexture[1], "column\\o");
	GL_LoadPanel(m_iColumnTexture[2], "column\\c");

	GL_LoadPanel(m_iButtonTexture[0], "button\\n");
	GL_LoadPanel(m_iButtonTexture[1], "button\\o");
	GL_LoadPanel(m_iButtonTexture[2], "button\\c");

	BuildBaseFrame();

	Close();

	gEngfuncs.pfnHookUserMsg("ZombieMenu", MsgFunc_ZombieMenu);
}

void CNormalZombieMenu::BuildBaseFrame()
{
	//m_pPanel = MGUI_CreatePanel(int(ScreenWidth * 0.029), int(ScreenHeight * 0.062), int(ScreenWidth * 0.95), int(ScreenHeight * 0.86), 20, false, true);
	m_pPanel = MGUI_CreatePanel(0, 0, 0, 0, 22, false, false);
	wcscpy(m_pPanel->m_pTitle, GetLangUni("#CSO_ZombiSelect_Keep_Title"));
	m_pPanel->m_iTitleSize = 18;
	m_pPanel->m_bHasClose = FALSE;
	
	int iX = m_pPanel->x, iY = m_pPanel->y;
	int i;
	iX += 21;
	iY += 54;

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
	m_pButtonPage[0]->SetCommand("MGUI.NormalZombieMenuPrevPage");
	m_pButtonPage[1]->SetCommand("MGUI.NormalZombieMenuNextPage");
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
		m_pLabelInfo[i]->SetSize(14);
	}
}

void CNormalZombieMenu::Reset(void)
{
	float scale = (float)ScreenHeight / 480;
	m_pPanel->x = (int)((ScreenWidth - 606 * scale) / 2);
	m_pPanel->prevx = m_pPanel->x;
	m_pPanel->y = (int)((ScreenHeight - 420 * scale) / 2);
	m_pPanel->prevy = m_pPanel->y;
	m_pPanel->w = 596.875 * scale;	//(int)(606 * scale);
	m_pPanel->h = 413.125 * scale;//(int)(420 * scale);

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

void CNormalZombieMenu::UpdateItems(void)
{
	SwitchToPage(0);
}

void CNormalZombieMenu::Command(char *szCommand)
{
	if (strstr(szCommand, "NormalZombieMenuPrevPage"))
	{
		SwitchToPage(m_iCurrentPage - 1);
	}
	else if (strstr(szCommand, "NormalZombieMenuNextPage"))
	{
		SwitchToPage(m_iCurrentPage + 1);
	}
}

void CNormalZombieMenu::ClearMenu()
{
	m_iElementCount = 0;
}

char *PraseZombieClassName(char *name)
{
	char *pPos = 0;
	if (pPos = strstr(name, "_zb"))
		*pPos = '\0';
	else if (pPos = strstr(name, "zb"))
		*pPos = '\0';
	else if (pPos = strstr(name, "_zombi"))
		*pPos = '\0';
	else if (pPos = strstr(name, "zombi"))
		*pPos = '\0';
	if (!stricmp(name, "heal"))
		strcpy(name, "doctor");
	if (!stricmp(name, "tank"))
		name[0] = '\0';
	if (!stricmp(name, "speed"))
		strcpy(name, "light");
	return name;
}

bool CNormalZombieMenu::AddItem(char *szModel)
{
	if (m_iElementCount >= 128)
		return false;
	int i = m_iElementCount;
	strcpy(m_ZombieClassModel[i], szModel);
	strcpy(m_ZombieClassName[i], szModel);
	PraseZombieClassName(m_ZombieClassName[i]);
	m_iElementCount++;
	return true;
}

void CNormalZombieMenu::SwitchToPage(int iPage)
{
	if (iPage < 0) iPage = 0;
	if (iPage > m_iElementCount / 9) iPage = m_iElementCount / 9;
	m_iCurrentPage = iPage;
	
	wchar_t pKey[3];
	for (int i = 0; i < 10; i++)
	{

		int iClassID = i + m_iCurrentPage * 9;
		m_pButtonSlot[i]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);

		if (i != 9)
		{
			if (iClassID < m_iElementCount)
			{
				m_pButtonSlot[i]->m_OverPanel = m_pPanelInfo[i];
				m_pButtonSlot[i]->SetCommand(va("bte_normal_zombie %s; ", m_ZombieClassModel[iClassID]));
				m_pButtonSlotLabel[i]->SetLabel(GetZombieNameFormat(m_ZombieClassName[iClassID]));
				
				m_pLabelInfo[i]->SetLabel(GetZombieLabelFormat(m_ZombieClassName[iClassID]));

				if (!stricmp(m_ZombieClassName[iClassID], "random"))
					m_pImageInfo[i]->SetTexture(va("resource\\zombi\\%szombie", m_ZombieClassName[iClassID]));
				else if (!strnicmp(m_ZombieClassName[iClassID], "z4", 2))
					m_pImageInfo[i]->SetTexture(va("resource\\zombi\\%s", m_ZombieClassName[iClassID]));
				else
					m_pImageInfo[i]->SetTexture(va("resource\\zombi\\%szombi", m_ZombieClassName[iClassID]));

			}
			else
			{
				m_pButtonSlot[i]->m_OverPanel = NULL;
				m_pButtonSlot[i]->SetCommand("");
			}
		}
		else
		{
			m_pButtonSlot[i]->m_OverPanel = NULL;
			m_pButtonSlot[i]->SetCommand("bte_normal_zombie NULL; ");
			m_pButtonSlotLabel[i]->SetLabel(GetLangUni("#CSO_ZombiSelect_NoKeep"));

			m_pLabelInfo[i]->SetLabel(GetLangUni("#CSO_ZOMBI3_KeepZombiTypeDesc"));
		}

		m_pPanelInfo[i]->m_iClosing = 1;

		m_pButtonSlotKeyLabel[i]->SetColor(255, 255, 255, 255);
		wcscpy(pKey, UTF8ToUnicode(va("%d", (i + 1) % 10)));
		m_pButtonSlotKeyLabel[i]->SetLabel(pKey);

		m_pButtonSlotLabel[i]->SetColor(255, 255, 255, 255);


		if (iClassID < m_iElementCount || (i == 9))
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

	if (iPage == m_iElementCount / 9)
		m_pButtonPage[1]->SetVisible(false);
	else
		m_pButtonPage[1]->SetVisible(true);
}

void CNormalZombieMenu::Show(void)
{
	if (m_iElementCount <= 1)
		return;
	if (m_pPanel->m_iClosing == FALSE)
	{
		Close();
		return;
	}
	if (pTeamSuitPanel)
		pTeamSuitPanel->m_iClosing = TRUE;
	if (pBuyMenuPanel)
		pBuyMenuPanel->m_iClosing = TRUE;
	g_TeamMenu.Close();
	UpdateItems();
	g_mgui_candraw = 1;
	if (m_pPanel)
		m_pPanel->m_iClosing = FALSE;
}

void CNormalZombieMenu::Close(void)
{
	if (m_pPanel)
		m_pPanel->m_iClosing = TRUE;
	for (int i = 0; i < 10; i++)
		if (m_pPanelInfo[i])
			m_pPanelInfo[i]->m_iClosing = TRUE;
}