#include "metahook.h"
#include "plugins.h"
#include "bte_const.h"
#include "exportfuncs.h"
#include "hud.h"
#include "imm.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "playaudio.h"
#include "util.h"
#include "Encode.h"

#include "vgui_controls/controls.h"

#include "MGUI/mgui.h"
#include "MGUI/BTEPanel.h"
#include "MGUI/TeamMenu.h"
#include "MGUI/NormalZombieMenu.h"

#include "Client/HUD/hud_saytext.h"
#include "Client/HUD/DrawTGA.h"

MGUI_Panel *pRootPanel = NULL;
MGUI_Image *pImage;
WNDPROC g_OldProc;

int g_mgui_keynum = -1;
int g_mgui_mouseevent, g_mgui_oldmouseevent, g_mgui_candraw;
int g_mgui_tga_imebg;

int g_mouse_w, g_mouse_h;
//int g_mgui_tga_button[3];
float g_mgui_nextclicktime;
int g_mgui_mouse;
CANDIDATELIST *CandidateList;
bool g_bDrawIme = false;
int iCanSetMouse = 0;

void MGUI_Base::Paint(MGUI_Base *pParent)
{
	return;
}
////////////////////// PANEL

MGUI_Panel::MGUI_Panel(void)
{
	iClass = MGUI_PANEL;
	m_iClosing = 0;
	memcpy(m_iTexture, g_UI_Panel, sizeof(g_UI_Panel));
	m_iTitleHeight = 0;
	pNext = NULL;
	m_bHasClose = false;
	m_bCanControl = true;
	m_iTitleSize = 0;
	m_bPerformingMessageBox = false;
	m_bCenterTitle = true;
	memset(m_pTitle, 0, sizeof(m_pTitle));
	return;
}
MGUI_Panel::~MGUI_Panel(void)
{
	delete m_pMsgBox;
	return;
}

int MGUI_Panel::SetTexture(int iTex[3][3])
{
	memcpy(m_iTexture, iTex, sizeof(iTex));
	//Hud().m_TGA.GetTGA(pName);
	return 1;
}

MGUI_Panel *MGUI_CreatePanel(int x, int y, int w, int h, int iTitleHeight, bool CanMove, bool bHasMsgBox)
{
	MGUI_Panel *pPanel = new MGUI_Panel;
	if (!pPanel)
	{
		return NULL;
	}
	pPanel->x = x;
	pPanel->prevx = x;
	pPanel->prevy = y;
	pPanel->y = y;
	pPanel->w = w;
	pPanel->h = h;
	pPanel->m_bCanMove = CanMove;
	pPanel->m_iTitleHeight = iTitleHeight;
	pPanel->pNextPanel = 0x0;
	pPanel->m_pMsgBox = NULL;
	pPanel->m_iHaveBg = 1;
	memcpy(pPanel->m_iTexture, g_UI_Panel_Dark, sizeof(g_UI_Panel_Dark));

	if (bHasMsgBox)
		pPanel->m_pMsgBox = new MGUI_MSGBox;

	if (!pRootPanel)
	{
		pRootPanel = pPanel;
	}
	else
	{
		MGUI_Panel *pPanelNext = pRootPanel;
		while (pPanelNext->pNextPanel)
		{
			pPanelNext = pPanelNext->pNextPanel;
		}
		pPanelNext->pNextPanel = pPanel;
	}
	return pPanel;
}
bool MGUI_Panel::IsInRect(void)
{
	int iMouseX, iMouseY;
	gEngfuncs.GetMousePosition(&iMouseX, &iMouseY);
	if (iMouseX>x && iMouseX< x + w && iMouseY>y && iMouseY<y + m_iTitleHeight) return true;
	return false;
}
bool MGUI_Panel::InsertControl(MGUI_Base *pChild)
{
	MGUI_Base *pTemp = (MGUI_Base *)this;
	while (pTemp->pNext)
	{
		pTemp = pTemp->pNext;
	}
	pTemp->pNext = pChild;
	return true;
}
char *MGUI_Panel::GetTextEntryText(int iClass)
{
	MGUI_Base *pNext = this;
	while (pNext)
	{
		if (pNext->iClass == MGUI_TEXTENTRY)
		{
			MGUI_TextEntry *pEntry = (MGUI_TextEntry *)pNext;
			if (pEntry->m_iClass == iClass)
			{
				return pEntry->m_pText;
			}
		}
		pNext = pNext->pNext;
	}
	return NULL;
}
void MGUI_Panel::MGUIMessageBox(wchar_t szCaption[], wchar_t szText[], int iType, void(*pfnOKClick)(MGUI_Button *), void(*pfnCancelClick)(MGUI_Button *), int iOKKey, int iCancelKey)
{
	MGUI_MSGBox *msgbox = MGUI_CreateMessageBox(this, szCaption, szText, iType, pfnOKClick, pfnCancelClick, iOKKey, iCancelKey);
	m_pMsgBox = msgbox;
}

////////////////////// IMAGE
void MGUI_Image::SetTexture(char *pName)
{
	//m_iTexture = vgui::surface()->CreateNewTextureID();
	m_iTexture = Hud().m_TGA.FindTexture(pName);

	//return m_iTexture;
}

void MGUI_Image::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}

MGUI_Image *MGUI_CreateImage(MGUI_Base *pParent, int x, int y, int w, int h, bool AutoResize, bool ForceSize)
{
	MGUI_Image *pImage = new MGUI_Image;
	if (!pImage)
	{
		return NULL;
	}
	pImage->x = x;
	pImage->y = y;
	pImage->w = w;
	pImage->h = h;
	pImage->m_bAutoResize = AutoResize;
	pImage->m_bForceSize = ForceSize;
	pImage->pParent = pParent;
	pImage->pNext = NULL;
	pImage->iClass = MGUI_IMAGE;
	pImage->m_bVisible = true;
	pImage->m_iTex_Type = MGUI_TEX_TYPE_1;
	//pImage->m_iTexture = 0;
	//pImage->m_iTexture = vgui::surface()->CreateNewTextureID();
	((MGUI_Panel *)pParent)->InsertControl(pImage);
	return pImage;
}
void MGUI_Image::Paint(MGUI_Base *pBaseParent)
{
	if (!pParent) return;
	if (pParent == pBaseParent)
	{
		if (((MGUI_Panel *)pBaseParent)->m_iClosing || !m_bVisible || (m_iTexture <= 0 && m_iTex_Type == MGUI_TEX_TYPE_1))
			return;

		if (m_iTex_Type == MGUI_TEX_TYPE_1)
			GL_DrawTGA2(g_Texture[m_iTexture].iTexture, x + pParent->x, pParent->y + y, w, h, 255);
		if (m_iTex_Type == MGUI_TEX_TYPE_3)
			GL_DrawPanel(m_iTexture3, x + pParent->x, pParent->y + y, w, h, 255);
		if (m_iTex_Type == MGUI_TEX_TYPE_9)
			GL_DrawFullPanel(m_iTexture9, x + pParent->x, pParent->y + y, w, h, 255);
	}
}
/////////// TEXTENTRY
void MGUI_TextEntry::Paint(MGUI_Base *pBaseParent)
{
	if (!pParent) return;
	if (pParent == pBaseParent)
	{
		if (((MGUI_Panel *)pBaseParent)->m_iClosing) return;
		// Draw Bg
		//GL_DrawTGA2(g_mgui_tga_imebg,x+pParent->x,pParent->y+y,w,48,255);
		// Draw Text
		g_Font.SetWidth(20);
		g_Font.SetColor(0, 0, 0, 255);
		wchar_t pWchar[255];
		g_Font.C2W(pWchar, 255, m_pText);
		int iEnd = g_Font.DrawString(pWchar, x + pParent->x + 10, pParent->y + y + 26, w);
		// Draw Line
		if (m_bHasFocus)
		{
			// Draw IME
			if (CandidateList)
			{
				if (g_bDrawIme)
				{
					g_Font.SetWidth(20);
					if (CandidateList->dwCount>1)
					{
						int iLong = 0;
						for (DWORD i = 0; i < CandidateList->dwCount; i++)
						{
							char *pText = (char *)CandidateList + CandidateList->dwOffset[i];
							if (pText)
							{
								int iTemp = strlen(pText);
								if (iTemp>iLong) iLong = iTemp;

							}
						}
						// Draw INSIDE
						g_Font.SetColor(255, 0, 0, 255);
						gEngfuncs.pfnFillRGBA(iEnd, pParent->y + y + 26 - 20, iLong * 10 + 20, 24, 135, 135, 135, 255);
						iEnd = g_Font.DrawString(ANSIToUnicode((char*)CandidateList + CandidateList->dwOffset[0]), iEnd, pParent->y + y + 26, 1000);
						// Draw LIST
						int iTotal = CandidateList->dwCount;
						gEngfuncs.pfnFillRGBA(iEnd - 10, pParent->y + y - 24 * CandidateList->dwCount, iLong * 10 + 40, 24 * CandidateList->dwCount, 40, 40, 40, 255);

						// GetLongest
						for (DWORD i = 0; i < CandidateList->dwCount; i++)
						{
							char *pText = (char*)CandidateList + CandidateList->dwOffset[i];
							if (pText)
							{
								g_Font.SetColor(255, 255, 255, 255);
								char Content[256];
								sprintf(Content, "%d.%s", i + 1, pText);
								g_Font.DrawString(ANSIToUnicode(Content), iEnd, pParent->y + y - (iTotal - i) * 24 + 2 + 18, 1000);
							}
						}
					}
				}
			}
			float fDelta = Hud().m_flTime;
			fDelta -= (int)fDelta;
			if (fDelta > 0.5)
			{
				g_Font.DrawString(L"|", iEnd, pParent->y + y + 26, w);
			}

		}
	}
	return;
}
bool MGUI_TextEntry::IsInRect(void)
{
	int iMouseX, iMouseY;
	gEngfuncs.GetMousePosition(&iMouseX, &iMouseY);
	int iX = pParent->x + x;
	int iY = pParent->y + y;
	if (iMouseX>iX && iMouseX< iX + w && iMouseY>iY && iMouseY<iY + 48) return true;
	return false;
}
MGUI_TextEntry *MGUI_CreateTextEntry(MGUI_Base *pParent, int x, int y, int w)
{
	MGUI_TextEntry *pTextEntry = new MGUI_TextEntry;
	if (!pTextEntry)
	{
		return NULL;
	}
	pTextEntry->x = x;
	pTextEntry->y = y;
	pTextEntry->w = w;
	pTextEntry->pParent = pParent;
	pTextEntry->m_bHasFocus = false;
	pTextEntry->pNext = NULL;
	pTextEntry->iClass = MGUI_TEXTENTRY;
	pTextEntry->m_iClass = 0;
	pTextEntry->SetText("");
	((MGUI_Panel *)pParent)->InsertControl(pTextEntry);
	return pTextEntry;
}
void MGUI_TextEntry::SetText(char *pszText)
{
	sprintf(m_pText, "%s", pszText);
}
////////// MENU
MGUI_Menu *MGUI_CreateMGUI_Menu(MGUI_Base *pParent, int x, int y, int w)
{
	MGUI_Menu *pMenu = new MGUI_Menu;
	if (!pMenu)
	{
		return NULL;
	}
	pMenu->x = x;
	pMenu->y = y;
	pMenu->w = w;
	pMenu->pParent = pParent;
	pMenu->pNext = NULL;
	pMenu->iClass = MGUI_MENU;
	pMenu->m_iNum = 0;
	pMenu->m_bShow = true;
	pMenu->m_iOnFocus = 0;
	((MGUI_Panel *)pParent)->InsertControl(pMenu);
	return pMenu;
}
void MGUI_Menu::AddItem(char *pItemName)
{
	m_iNum++;
	sprintf(pText[m_iNum], "%s", pItemName);
	return;
}
void MGUI_Menu::Paint(MGUI_Base *pBaseParent)
{
	if (!pParent || !m_bShow) return;
	if (pParent == pBaseParent)
	{
		if (((MGUI_Panel *)pBaseParent)->m_iClosing) return;

		// Draw Bg
		int iTotal = m_iNum;
		gEngfuncs.pfnFillRGBA(pParent->x + x, pParent->y + y, w, 24 * iTotal, 255, 255, 0, 100);
		// Draw Border
		// Up
		gEngfuncs.pfnFillRGBA(pParent->x + x, pParent->y + y - 1, w, 1, 255, 255, 255, 255);
		// Down
		gEngfuncs.pfnFillRGBA(pParent->x + x, pParent->y + y + 24 * iTotal, w, 1, 255, 255, 255, 255);
		// Left
		gEngfuncs.pfnFillRGBA(pParent->x + x - 1, pParent->y + y, 1, 24 * iTotal, 255, 255, 255, 255);
		// Right
		gEngfuncs.pfnFillRGBA(pParent->x + x + w, pParent->y + y, 1, 24 * iTotal, 255, 255, 255, 255);
		// Draw Text
		for (int i = 1; i <= m_iNum; i++)
		{
			if (m_iOnFocus == i)
			{
				gEngfuncs.pfnFillRGBA(pParent->x + x, pParent->y + y + 24 * (i - 1), w, 24, 255, 255, 0, 200);
			}
			g_Font.SetWidth(16);
			int iStartX = w - g_Font.GetLen(ANSIToUnicode(pText[i]));
			iStartX /= 2;
			g_Font.SetColor(255, 0, 0, 255);
			g_Font.DrawString(ANSIToUnicode(pText[i]), pParent->x + x + iStartX, pParent->y + y + 24 * i - 6, 100);
			gEngfuncs.pfnFillRGBA(pParent->x + x, pParent->y + y + 24 * i, w, 1, 255, 255, 255, 255);
		}
	}
}
int MGUI_Menu::IsInRect(void)
{
	int iMouseX, iMouseY;
	gEngfuncs.GetMousePosition(&iMouseX, &iMouseY);
	int iX = pParent->x + x;
	int iY = pParent->y + y;
	for (int i = 1; i <= m_iNum; i++)
	{
		if (iMouseX>iX && iMouseX< iX + w && iMouseY>iY && iMouseY<iY + 24 * i)
		{
			m_iOnFocus = i;
			return i;
		}
	}
	m_iOnFocus = 0;
	return 0;
}
////// BUTTON
MGUI_Button *MGUI_CreateButton(MGUI_Base *pParent, int x, int y, int w, int h)
{
	MGUI_Button *pButton = new MGUI_Button;
	if (!pButton)
	{
		return NULL;
	}
	pButton->x = x;
	pButton->y = y;
	pButton->w = w;
	pButton->h = h;
	pButton->pParent = pParent;
	memset(pButton->pszLabel, 0, sizeof(pButton->pszLabel));
	memset(pButton->pszCommand, 0, sizeof(pButton->pszCommand));
	memset(pButton->pszCommand2, 0, sizeof(pButton->pszCommand2));
	memset(pButton->iTga, 0, sizeof(pButton->iTga));
	memset(pButton->iTga3, 0, sizeof(pButton->iTga));
	memset(pButton->iTga9, 0, sizeof(pButton->iTga));
	/*pButton->iTga[0] = vgui::surface()->CreateNewTextureID();
	pButton->iTga[1] = vgui::surface()->CreateNewTextureID();
	pButton->iTga[2] = vgui::surface()->CreateNewTextureID();*/

	pButton->m_iType = MGUI_BUTTON_TYPE_COMMAND;
	pButton->m_iType2 = MGUI_BUTTON_TYPE_COMMAND;
	pButton->pNext = NULL;
	pButton->iClass = MGUI_BUTTON;
	pButton->m_iStat = 0;
	pButton->m_fNextClick = 0;
	pButton->m_Tex_Type = MGUI_TEX_TYPE_1;
	pButton->m_iOffset[0] = pButton->m_iOffset[1] = 0;

	/*pButton->iTga[0] = g_mgui_tga_button[0];
	pButton->iTga[1] = g_mgui_tga_button[1];
	pButton->iTga[2] = g_mgui_tga_button[2];*/
	/*memcpy(pButton->iTga3[0],g_UI_Button[0],sizeof(g_UI_Button[0]));
	memcpy(pButton->iTga3[1],g_UI_Button[1],sizeof(g_UI_Button[1]));
	memcpy(pButton->iTga3[2],g_UI_Button[2],sizeof(g_UI_Button[2]));*/
	pButton->m_iColor32[0] = pButton->m_iColor32[1] = pButton->m_iColor32[2] = pButton->m_iColor32[3] = 255;
	pButton->m_bVisible = true;
	pButton->m_bEnabled = true;
	pButton->m_bEnabled2 = true;
	pButton->m_bClickable = TRUE;
	pButton->m_iSize = h - 16;
	pButton->m_OverPanel = 0x0;
	pButton->m_iKey = -1;

	((MGUI_Panel *)pParent)->InsertControl(pButton);
	return pButton;
}
void MGUI_Button::Paint(MGUI_Base *pBaseParent)
{
	if (!pParent || !m_bVisible) return;
	if (pParent == pBaseParent)
	{
		if (((MGUI_Panel *)pBaseParent)->m_iClosing) return;

		if (iTga[m_iStat] != -1/* && m_bEnabled*/)
		{
			if (m_Tex_Type == MGUI_TEX_TYPE_1)
				GL_DrawTGA2(g_Texture[iTga[m_iStat]].iTexture, x + pParent->x, pParent->y + y, w, h, 255);
			else if (m_Tex_Type == MGUI_TEX_TYPE_3)
				GL_DrawPanel(iTga3[m_iStat], x + pParent->x, pParent->y + y, w, h, 255);
			else
				GL_DrawFullPanel(iTga9[m_iStat], x + pParent->x, pParent->y + y, w, h, 255);
		}
		else
		{
			if (m_Tex_Type == MGUI_TEX_TYPE_1)
				GL_DrawTGA2(g_Texture[iTga[0]].iTexture, x + pParent->x, pParent->y + y, w, h, 255);
			else if (m_Tex_Type == MGUI_TEX_TYPE_3)
				GL_DrawPanel(iTga3[0], x + pParent->x, pParent->y + y, w, h, 255);
			else
				GL_DrawFullPanel(iTga9[0], x + pParent->x, pParent->y + y, w, h, 255);
		}
		// Draw Label
		if (pszLabel[0])
		{
			//int iOffset = w /2 - wcslen(pszLabel) * (m_iSize)/2;
			g_Font.SetWidth(m_iSize);
			int iOffset = w / 2 - g_Font.GetLen(pszLabel) / 2;

			if (m_bEnabled)
				g_Font.SetColor(m_iColor32[0], m_iColor32[1], m_iColor32[2], m_iColor32[3]);
			else
				g_Font.SetColor(75, 75, 75, 200);
			g_Font.DrawString(pszLabel, x + pParent->x + iOffset + m_iOffset[0], pParent->y + y + h / 2 + m_iSize / 2 - 1 + m_iOffset[1], 1000);
		}

	}
}
void MGUI_Button::SetImage(char *n, char *o, char *c)
{
	iTga[0] = Hud().m_TGA.FindTexture(n);
	iTga[1] = Hud().m_TGA.FindTexture(o);
	iTga[2] = Hud().m_TGA.FindTexture(c);
	//vgui::surface()->DrawSetTextureFile(iTga[0],n,true,true);
	//vgui::surface()->DrawSetTextureFile(iTga[1],o,true,true);
	//vgui::surface()->DrawSetTextureFile(iTga[2],c,true,true);
}
void MGUI_Button::SetImage3(int iTex[3][3])
{
	memcpy(iTga3, iTex, sizeof(iTex));
}
void MGUI_Button::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}
void MGUI_Button::SetEnabled(bool fEnabled)
{
	m_bEnabled = fEnabled;
	SetClickable(fEnabled);
}
void MGUI_Button::SetType(int iType)
{
	m_iType = iType;
}
void MGUI_Button::SetCommand(char *pCommand)
{
	sprintf(pszCommand, "%s", pCommand);
}
void MGUI_Button::SetCommand2(char *pCommand)
{
	sprintf(pszCommand2, "%s", pCommand);
}
void MGUI_Button::SetColor(int r, int g, int b, int a)
{
	m_iColor32[0] = r;
	m_iColor32[1] = g;
	m_iColor32[2] = b;
	m_iColor32[3] = a;
}
void MGUI_Button::SetKey(int iKey)
{
	m_iKey = iKey;
}
bool MGUI_Button::IsInRect(void)
{
	int iMouseX, iMouseY;
	gEngfuncs.GetMousePosition(&iMouseX, &iMouseY);
	int iX = pParent->x + x;
	int iY = pParent->y + y;

	if (iMouseX >= iX && iMouseX <= iX + w && iMouseY >= iY && iMouseY <= iY + h)
	{
		if (m_OverPanel)
		{
			m_OverPanel->m_iClosing = 0;
		}
		return true;
	}
	if (m_OverPanel)
		m_OverPanel->m_iClosing = 1;
	return false;
}
void MGUI_Send_Cmd(char *szCmd)
{
	if (strstr(szCmd, "MGUI."))
	{
		if (strstr(szCmd, "BuyMenu_SaveFav"))
		{
			char num[3];
			num[2] = '\0';
			strncpy(num, szCmd + strlen("MGUI.BuyMenu_SaveFav"), 2);
			BTEPanel_BuyMenu_SetFav(atoi(num));
		}
		else if (strstr(szCmd, "ShowBuyMenu"))
		{
			if (g_iMod == MOD_TDM || g_iMod == MOD_DM || g_iMod == MOD_ZB4)
				BTEPanel_TeamSuit_SetSlot(0);

			BTEPanel_BuyMenu_UpdateItems(-1, "");
			BTEPanel_BuyMenu_SetPage(0, 0);
		}/*
		else if (strstr(szCmd, "SaveTeamSuit"))
		{
			BTEPanel_TeamSuit_Save();
		}*/
		else if (strstr(szCmd, "TeamSuitPrevSlot"))
		{
			BTEPanel_TeamSuit_SetSlot(-1);
		}
		else if (strstr(szCmd, "TeamSuitNextSlot"))
		{
			BTEPanel_TeamSuit_SetSlot(1);
		}
		else if (strstr(szCmd, "BuyMenu_Show"))
		{
			char num[3];
			num[2] = 0;
			strncpy(num, szCmd + strlen("MGUI.BuyMenu_Show"), 2);
			//BTEPanel_BuyMenu_SetFav(atoi(num));
			ShowBuyMenu(atoi(num), 0);
		}
		else if (strstr(szCmd, "BuyMenu_SetPage"))
		{
			char num[3], num2[3];
			num[2] = 0;
			num2[2] = 0;
			strncpy(num, szCmd + strlen("MGUI.BuyMenu_SetPage"), 2);
			strncpy(num2, szCmd + strlen("MGUI.BuyMenu_SetPage") + 3, 2);

			ShowBuyMenu(atoi(num), atoi(num2));
		}
		else if (strstr(szCmd, "BuyMenu_BuyWeapon"))
		{
			char sName[256], i[2];
			memset(sName, 0, sizeof(sName));
			memset(i, 0, sizeof(i));
			strncpy(i, szCmd + strlen("MGUI.BuyMenu_BuyWeapon") + 1, 1);
			strncpy(sName, szCmd + strlen("MGUI.BuyMenu_BuyWeapon") + 3, 256);

			BuyWeapon(sName, atoi(i));
		}
		else if (strstr(szCmd, "BuyMenu_AutoBuy"))
		{
			int iLocal = gEngfuncs.GetLocalPlayer()->index;
			if (vPlayer[iLocal].team == 1)
				gEngfuncs.pfnClientCmd("sv_create_psb 10397 m4a1;sv_create_psb 10397 usp;sv_create_psb 10397 hegrenade;sv_create_psb 10397 knife;secammo;primammo;vesthelm;defuser");
			else
				gEngfuncs.pfnClientCmd("sv_create_psb 10397 ak47;sv_create_psb 10397 usp;sv_create_psb 10397 hegrenade;sv_create_psb 10397 knife;secammo;primammo;vesthelm");
		}
		else if (strstr(szCmd, "BuyMenu_BuyFav"))
		{
			int iSlot = atoi(szCmd + strlen("MGUI.BuyMenu_BuyFav "));
			BuyFavWeapon(iSlot);
		}
		else if (strstr(szCmd, "BuyMenu_ClearQuickBuy"))
		{
			ClearQuickBuy();
		}
		else if (strstr(szCmd, "BuyMenu_BuyDefault"))
		{
			BuyDefaultWeapon();
		}
		else if (strstr(szCmd, "TeamMenu"))
		{
			g_TeamMenu.Command(szCmd);
		}
		else if (strstr(szCmd, "NormalZombieMenu"))
		{
			g_NormalZombieMenu.Command(szCmd);
		}
	}
	else
		gEngfuncs.pfnClientCmd(szCmd);
}

void MGUI_Button::Click(void)
{
	if (!m_bVisible || (!Clickable() && !pfnClick)) return;
	g_mgui_keynum = -1;
	g_Next_Key_CanUse = Hud().m_flTime + 0.15;
	if (m_iType == MGUI_BUTTON_TYPE_CHANGE_NAME)
	{
		char *name = ((MGUI_Panel *)pParent)->GetTextEntryText(1);
		if (name)
		{
			char szCmd[64];
			sprintf(szCmd, "name %s", UnicodeToUTF8(ANSIToUnicode(name)));
			MGUI_Send_Cmd(szCmd);
		}
	}
	else if (m_iType == MGUI_BUTTON_TYPE_COMMAND_CLOSEPANEL)
	{
		MGUI_Send_Cmd(pszCommand);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
	}
	else if (m_iType == MGUI_BUTTON_TYPE_CLOSEALL)
	{
		MGUI_Send_Cmd(pszCommand);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
		g_mgui_candraw = 0;
	}
	else if (m_iType == MGUI_BUTTON_TYPE_FUNCTION)
	{
		if (pfnClick)
			pfnClick(this);
	}
	else if (m_iType == MGUI_BUTTON_TYPE_FUNCTION_CLOSEPANEL)
	{
		if (pfnClick)
			pfnClick(this);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
	}
	else if (m_iType == MGUI_BUTTON_TYPE_FUNCTION_CLOSEALL)
	{
		if (pfnClick)
			pfnClick(this);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
		g_mgui_candraw = 0;
	}
	else
	{
		MGUI_Send_Cmd(pszCommand);
	}
}
void MGUI_Button::Click2(void)
{
	if (!m_bVisible || (!m_bEnabled2 && !pfnClick2)) return;
	g_mgui_keynum = -1;
	g_Next_Key_CanUse = Hud().m_flTime + 0.15;
	if (m_iType2 == MGUI_BUTTON_TYPE_COMMAND_CLOSEPANEL)
	{
		MGUI_Send_Cmd(pszCommand2);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
		g_Next_Key_CanUse = Hud().m_flTime + 0.1;
	}
	else if (m_iType2 == MGUI_BUTTON_TYPE_CLOSEALL)
	{
		MGUI_Send_Cmd(pszCommand2);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
		g_mgui_candraw = 0;
		g_Next_Key_CanUse = Hud().m_flTime + 0.1;
	}
	else if (m_iType == MGUI_BUTTON_TYPE_FUNCTION)
	{
		if (pfnClick2)
			pfnClick2(this);
		g_Next_Key_CanUse = Hud().m_flTime + 0.1;
	}
	else if (m_iType == MGUI_BUTTON_TYPE_FUNCTION_CLOSEPANEL)
	{
		if (pfnClick2)
			pfnClick2(this);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
		g_Next_Key_CanUse = Hud().m_flTime + 0.1;
	}
	else if (m_iType == MGUI_BUTTON_TYPE_FUNCTION_CLOSEALL)
	{
		if (pfnClick2)
			pfnClick2(this);
		((MGUI_Panel *)pParent)->m_iClosing = 1;
		g_mgui_candraw = 0;
		g_Next_Key_CanUse = Hud().m_flTime + 0.1;
	}
	else
	{
		MGUI_Send_Cmd(pszCommand2);
	}
}
void MGUI_Button::SetLabel(wchar_t *pLabel)
{
	wcscpy(pszLabel, pLabel);
	for (int i = 0; i < 30; i++)	// not 31 so we can directly check the shortcut
	{
		if (pszLabel[i] == '\0')
			break;
		if (pszLabel[i] == L'&')
		{
			SetKey(tolower(pszLabel[i + 1]));
			pszLabel[i] = '\0';
			wcscat(pszLabel, pszLabel + i + 1);
		}
	}
	g_Font.LoadStr(pLabel);
}
////////////// Label
MGUI_Label *MGUI_CreateLabel(MGUI_Base *pParent, int x, int y, int w, int h)
{
	MGUI_Label *pLabel = new MGUI_Label;
	if (!pLabel)
	{
		return NULL;
	}
	pLabel->x = x;
	pLabel->y = y;
	pLabel->w = w;
	pLabel->h = h;
	pLabel->m_iSize = 12;
	pLabel->pNext = NULL;
	pLabel->m_bVisible = true;
	pLabel->m_iColor32[0] = pLabel->m_iColor32[1] = pLabel->m_iColor32[2] = pLabel->m_iColor32[3] = 255;
	pLabel->pParent = pParent;
	memset(pLabel->pszLabel, 0, sizeof(pLabel->pszLabel));
	pLabel->iClass = MGUI_LABEL;
	pLabel->m_iAlignment = MGUI_Left;
	pLabel->m_iType = pLabel->m_iMaxHeight = 0;
	((MGUI_Panel *)pParent)->InsertControl(pLabel);
	return pLabel;
}
void MGUI_Label::SetSize(int size)
{
	m_iSize = size;
}
void MGUI_Label::SetVisible(bool bVisible)
{
	m_bVisible = bVisible;
}
void MGUI_Label::Paint(MGUI_Base *pBaseParent)
{
	if (!pParent || !m_bVisible) return;
	if (pParent == pBaseParent)
	{
		if (((MGUI_Panel *)pBaseParent)->m_iClosing) return;
		// Draw Label
		if (pszLabel[0])
		{
			g_Font.SetWidth(m_iSize);
			g_Font.SetColor(m_iColor32[0], m_iColor32[1], m_iColor32[2], m_iColor32[3]);

			if (m_iAlignment == MGUI_Left)
				g_Font.DrawString(pszLabel, x + pParent->x, pParent->y + y + m_iSize / 2 + h / 2, w, 3, false);
			else if (m_iAlignment == MGUI_Center)
				g_Font.DrawString(pszLabel, x + pParent->x + w / 2 - g_Font.GetLen(pszLabel) / 2, pParent->y + y + m_iSize / 2 + h / 2, w, 3, false);
			else if (m_iAlignment == MGUI_Right)
				g_Font.DrawString(pszLabel, x + pParent->x + w - g_Font.GetLen(pszLabel), pParent->y + y + m_iSize / 2 + h / 2, w, 3, false);
		}

	}
}
void MGUI_Label::SetColor(int r, int g, int b, int a)
{
	m_iColor32[0] = r;
	m_iColor32[1] = g;
	m_iColor32[2] = b;
	m_iColor32[3] = a;
}
void MGUI_Label::SetLabel(wchar_t *pLabel)
{
	wcscpy(pszLabel, pLabel);
	g_Font.LoadStr(pLabel);
}

MGUI_MSGBox::MGUI_MSGBox()
{
	m_pPanel = NULL;
	m_pButtonOK = m_pButtonCancel = NULL;
	m_pLabelText = NULL;
}

extern int g_UI_Button[3][3];

void DefaultCloseClickCallback(MGUI_Button *pButton)
{
	if (pButton->pSaveData)
	{
		((MGUI_Panel *)pButton->pSaveData)->m_bCanControl = true;
		((MGUI_Panel *)pButton->pSaveData)->m_bPerformingMessageBox = false;
	}
}

MGUI_MSGBox *MGUI_CreateMessageBox(MGUI_Panel *pParent, wchar_t caption[], wchar_t text[], int iType, void(*pfnOKClick)(MGUI_Button *), void(*pfnCancelClick)(MGUI_Button *), int iOKKey, int iCancelKey)
{
	MGUI_MSGBox *msgbox = pParent->m_pMsgBox;

	if (!msgbox)
	{
		return NULL;
	}

	if (!msgbox->m_pPanel)
	{
		msgbox->m_pPanel = MGUI_CreatePanel(0, 0, 0, 0, 0, true);
		msgbox->m_pPanel->m_bCanControl = true;
	}

	g_Font.SetWidth(14);

	if (!msgbox->m_pLabelText)
		msgbox->m_pLabelText = MGUI_CreateLabel(msgbox->m_pPanel, 0, 0, 0, 0);

	wchar_t pText[1024];
	wchar_t *pNewLine;
	int iMaxWidth = 0;

	wcscpy(pText, text);

	pNewLine = wcstok(pText, L"\n");
	if (pNewLine)
	{
		iMaxWidth = g_Font.GetLen(pText);
		while ((pNewLine = wcstok(NULL, L"\n")))
		{
			if (g_Font.GetLen(pText) > iMaxWidth)
				iMaxWidth = g_Font.GetLen(pText);
		}
	}

	pParent->m_bCanControl = false;
	msgbox->m_pPanel->w = 50 + /*g_Font.GetLen(caption)*/iMaxWidth + 60;
	msgbox->m_pPanel->h = 42 + g_Font.GetHeight(text, iMaxWidth, 3) + 48;
	msgbox->m_pPanel->x = (pParent->w - msgbox->m_pPanel->w) / 2;
	msgbox->m_pPanel->prevx = msgbox->m_pPanel->x;
	msgbox->m_pPanel->y = (pParent->h - msgbox->m_pPanel->h) / 2;
	msgbox->m_pPanel->prevy = msgbox->m_pPanel->y;
	msgbox->m_pPanel->m_bCanMove = true;
	msgbox->m_pPanel->m_bHasClose = false;
	msgbox->m_pPanel->m_iClosing = 0;
	msgbox->m_pPanel->m_iTitleSize = 14;
	msgbox->m_pPanel->m_bCenterTitle = false;
	wcscpy(msgbox->m_pPanel->m_pTitle, caption);

	msgbox->m_pLabelText->x = /*msgbox->m_pPanel->x + */50;
	msgbox->m_pLabelText->y = /*msgbox->m_pPanel->y + */52;
	msgbox->m_pLabelText->SetColor(255, 255, 255, 255);
	msgbox->m_pLabelText->SetLabel(text);
	msgbox->m_pLabelText->SetSize(14);
	msgbox->m_pLabelText->w = iMaxWidth;
	msgbox->m_pLabelText->h = 0;

	if (msgbox->m_pButtonOK)
		msgbox->m_pButtonOK->SetVisible(false);
	if (msgbox->m_pButtonCancel)
		msgbox->m_pButtonCancel->SetVisible(false);

	//iType = MB_OK;

	if (iType & MB_OK | MB_OKCANCEL)
	{
		msgbox->m_pPanel->m_bHasClose = false;
		if (!msgbox->m_pButtonOK)
			msgbox->m_pButtonOK = MGUI_CreateButton(msgbox->m_pPanel, 0, 0, 0, 0);
		msgbox->m_pButtonOK->w = 56;
		msgbox->m_pButtonOK->h = 20;
		msgbox->m_pButtonOK->x = msgbox->m_pPanel->w / 2 - msgbox->m_pButtonOK->w / 2;
		msgbox->m_pButtonOK->y = msgbox->m_pPanel->h - 45;
		msgbox->m_pButtonOK->SetLabel(GetLangUni("#CSO_OKl_Btn"));
		msgbox->m_pButtonOK->SetType(MGUI_BUTTON_TYPE_FUNCTION_CLOSEPANEL);
		msgbox->m_pButtonOK->SetEnabled(true);
		msgbox->m_pButtonOK->SetVisible(true);
		memcpy(msgbox->m_pButtonOK->iTga3, g_UI_Button, sizeof(g_UI_Button));
		msgbox->m_pButtonOK->pfnClick = pfnOKClick ? pfnOKClick : DefaultCloseClickCallback;
		msgbox->m_pButtonOK->SetKey(iOKKey);
		msgbox->m_pButtonOK->m_iSize = 14;
		msgbox->m_pButtonOK->m_Tex_Type = MGUI_TEX_TYPE_3;
		msgbox->m_pButtonOK->m_fNextClick = 0.0f;
		msgbox->m_pButtonOK->pSaveData = pParent;

		if (iType & MB_OKCANCEL)
		{
			msgbox->m_pButtonOK->x -= msgbox->m_pButtonOK->w;
			if (!msgbox->m_pButtonCancel)
				msgbox->m_pButtonCancel = MGUI_CreateButton(msgbox->m_pPanel, 0, 0, 0, 0);
			msgbox->m_pButtonCancel->x = msgbox->m_pButtonOK->x + msgbox->m_pButtonOK->w * 2;
			msgbox->m_pButtonCancel->y = msgbox->m_pButtonOK->y;
			msgbox->m_pButtonCancel->w = msgbox->m_pButtonOK->w;
			msgbox->m_pButtonCancel->h = msgbox->m_pButtonOK->h;
			msgbox->m_pButtonCancel->SetLabel(GetLangUni("#CSO_Cancel_Btn"));
			msgbox->m_pButtonCancel->SetType(MGUI_BUTTON_TYPE_FUNCTION_CLOSEPANEL);
			msgbox->m_pButtonCancel->SetEnabled(true);
			msgbox->m_pButtonCancel->SetVisible(true);
			msgbox->m_pButtonCancel->pfnClick = pfnCancelClick ? pfnCancelClick : DefaultCloseClickCallback;
			msgbox->m_pButtonCancel->SetKey(iCancelKey);
			memcpy(msgbox->m_pButtonCancel->iTga3, g_UI_Button, sizeof(g_UI_Button));
			msgbox->m_pButtonCancel->m_iSize = 14;
			msgbox->m_pButtonCancel->m_Tex_Type = MGUI_TEX_TYPE_3;
			msgbox->m_pButtonCancel->m_fNextClick = 0.0f;
			msgbox->m_pButtonCancel->pSaveData = pParent;
		}
	}
	else
		msgbox->m_pPanel->m_bHasClose = true;

	pParent->m_bPerformingMessageBox = true;

	return msgbox;
}

/////////
TCHAR		g_szInsert[0x1000] = { 0 };//输入的字符串
LONG		g_lInsertPos = 0;

LRESULT CALLBACK g_NewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_IME_SETCONTEXT:
		DefWindowProc(hWnd, WM_IME_SETCONTEXT, wParam, lParam&(~ISC_SHOWUIALLCANDIDATEWINDOW)); // 告诉输入法 我将自绘界面
	case WM_IME_STARTCOMPOSITION:
		return 0; // 隐藏候选词
	}

	if (*HudSayText().m_bInputing != 1)
		return CallWindowProc(g_OldProc, hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_IME_NOTIFY://输入法的动作
	{
		switch (wParam)
		{
		case IMN_OPENCANDIDATE://开始选词
		case IMN_SETCANDIDATEPOS://选词窗口移动
		case IMN_CHANGECANDIDATE://改变选词
		case IMN_CLOSECANDIDATE://关闭选词窗口
			//一起处理
		{
			HIMC hIMC;
			DWORD dwBufLen;
			int i;
			LPCANDIDATELIST pList;

			hIMC = ImmGetContext(hWnd);//取得输入法句柄
			dwBufLen = ImmGetCandidateList(hIMC, 0, NULL, 0);//取得候选词列表结构体的大小

			if (!dwBufLen)//成功的话
			{
				ImmReleaseContext(hWnd, hIMC);//释放输入法句柄
				break;
			}

			pList = (LPCANDIDATELIST)malloc(dwBufLen);//建立结构体
			if (!pList)
			{
				ImmReleaseContext(hWnd, hIMC);//释放输入法句柄
				break;
			}

			ImmGetCandidateList(hIMC, 0, pList, dwBufLen);//取得候选词列表
			if (pList->dwStyle != IME_CAND_CODE)//不能是IME_CAND_CODE否则当pList->dwCount为零的时候，
			{
				HudSayText().m_iListCount = pList->dwCount;
				HudSayText().m_iSelection = pList->dwSelection;

				if (HudSayText().m_iListCount > 512)
					HudSayText().m_iListCount = 512;

				for (i = 0; i < HudSayText().m_iListCount; i++)
				{
					TCHAR *pSubStr = (TCHAR*)((BYTE*)pList + pList->dwOffset[i]);//取得候选词

					wchar_t w[128];
					wcscpy(w, ANSIToUnicode(pSubStr));
					swprintf(HudSayText().m_wszList[i], 127, L"%d. %s", i + 1, w);
				}
			}
			free(pList);//释放结构体内存
			ImmReleaseContext(hWnd, hIMC);//释放输入法句柄
		}
		break;
		}
		break;
	}
	case WM_IME_COMPOSITION:
	{
		if (lParam & CS_INSERTCHAR)//当前光标插入一个字符
		{
			lstrcat(g_szInsert, (LPCTSTR)& wParam);
			/*if (CS_NOMOVECARET)
			;//不移动插入符*/
		}
		if (lParam & GCS_CURSORPOS)//取得正在输入的字符串（拼音之类的）的插入符的位置
		{
			HIMC hIMC = ImmGetContext(hWnd);
			g_lInsertPos = ImmGetCompositionString(hIMC, GCS_CURSORPOS, NULL, 0);
			HudSayText().m_iInsertPos = g_lInsertPos;

			ImmReleaseContext(hWnd, hIMC);
		}
		if (lParam & GCS_COMPSTR)//取得正在输入的字符串（拼音之类的）
		{
			HIMC hIMC = ImmGetContext(hWnd);
			UINT uLen, uMem;
			TCHAR*szCompStr;
			/*RECT rcWindow;

			GetClientRect(hWnd, &rcWindow);//把这一块窗体清空
			rcWindow.top = IMERECT_COMPSTR;
			rcWindow.bottom = IMERECT_COMPSTR + IMERECT_STRHEIGHT;
			FillRect(g_hDC, &rcWindow, g_WCEx.hbrBackground);*/

			uLen = ImmGetCompositionString(hIMC, GCS_COMPSTR, NULL, 0);//取得正在输入的字符串大小
			szCompStr = (TCHAR*)malloc(uMem = (uLen + sizeof(TCHAR)));//分配内存

			if (szCompStr)
			{
				szCompStr[uLen] = 0;//结尾的\0
				ImmGetCompositionString(hIMC, GCS_COMPSTR, szCompStr, uMem);//取得正在输入的字符串

				if (uLen > 256)
					szCompStr[255] = 0;

				wcscpy(HudSayText().m_wszComp, ANSIToUnicode(szCompStr));


				//TextOut(g_hDC, 0, IMERECT_COMPSTR, szCompStr, uLen);//显示
				//GetTextExtentPoint(g_hDC, szCompStr, g_lInsertPos, &Size);//准备画插入符
				//MoveToEx(g_hDC, Size.cx, IMERECT_COMPSTR, NULL);
				//LineTo(g_hDC, Size.cx, IMERECT_COMPSTR + Size.cy);//画一根竖线
				free(szCompStr);
			}
			ImmReleaseContext(hWnd, hIMC);
		}
		if (lParam & GCS_RESULTSTR)//取得结果字符串
		{
			HIMC hIMC = ImmGetContext(hWnd);
			UINT uLen, uMem;
			TCHAR*szCompStr;

			uLen = ImmGetCompositionString(hIMC, GCS_RESULTSTR, NULL, 0);//取得结果字符串大小
			szCompStr = (TCHAR*)malloc(uMem = (uLen + sizeof(TCHAR)));//分配内存
			if (szCompStr)
			{
				szCompStr[uLen] = 0;//结尾的\0
				ImmGetCompositionString(hIMC, GCS_RESULTSTR, szCompStr, uMem);//取得结果字符串
				lstrcat(g_szInsert, szCompStr);//把结果字符串添加到结果字符串里面
				free(szCompStr);
			}
			ImmReleaseContext(hWnd, hIMC);
		}
		break;
	}
	case WM_IME_ENDCOMPOSITION://打完字
	{
		HudSayText().m_iListCount = 0;
		HudSayText().m_wszComp[0] = 0;
		HudSayText().m_iInsertPos = 0;

		//HudSayText().m_bInputing = false;
		/*RECT rcWindow;

		GetClientRect(hWnd, &rcWindow);//把这一块窗体清空
		rcWindow.top = IMERECT_RESULT;
		rcWindow.bottom = IMERECT_RESULT + IMERECT_STRHEIGHT;
		FillRect(g_hDC, &rcWindow, g_WCEx.hbrBackground);*/

		//TextOut(g_hDC, 0, IMERECT_RESULT, g_szInsert, lstrlen(g_szInsert));//显示最终字符串
		//SetWindowText(hWnd, TEXT("结束打字"));
	}

	/*case WM_CHAR:
	{
	if (wParam == VK_BACK)
	{
	/*int len = strlen(pText);

	if (pText[len - 1] < 0)
	pText[len - 2] = '\0';
	else
	pText[len - 1] = '\0';****
	}
	else if (wParam != VK_RETURN)
	{
	/*int len = strlen(pText);
	pText[len] = wParam;
	pText[len + 1] = '\0';****
	}
	else
	{
	//MGUI_SetFocusTextEntryDisabled();
	}

	return 1;
	}

	case WM_IME_CHAR:
	{
	char buf[3];
	buf[0] = wParam >> 8;
	buf[1] = (byte)wParam;
	buf[2] = '\0';
	//strcat(pText, buf);
	return 1;


	break;
	}*/

	}
	return CallWindowProc(g_OldProc, hWnd, message, wParam, lParam);

}

/*LRESULT CALLBACK g_NewProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
if(!g_mgui_candraw) return CallWindowProc(g_OldProc, hWnd, message, wParam, lParam);

switch (message)
{
case WM_CHAR:
{
char *pText = MGUI_FindFocusTextEntryText();
if (pText)
{
if (wParam == VK_BACK)
{
int len = strlen(pText);

if (pText[len - 1] < 0)
pText[len - 2] = '\0';
else
pText[len - 1] = '\0';
}
else if(wParam != VK_RETURN)
{
int len = strlen(pText);
pText[len] = wParam;
pText[len + 1] = '\0';
}
else
{
MGUI_SetFocusTextEntryDisabled();
}

return 1;
}

break;
}

case WM_IME_CHAR:
{
char *pText = MGUI_FindFocusTextEntryText();
if (pText)
{
char buf[3];
buf[0] = wParam >> 8;
buf[1] = (byte)wParam;
buf[2] = '\0';
strcat(pText, buf);
return 1;
}

break;
}
case WM_IME_NOTIFY:
{
HIMC hImc = ImmGetContext(hWnd);
if(!hImc) break;
if(wParam == IMN_CHANGECANDIDATE)
{
DWORD dwSize;
dwSize = ImmGetCandidateList(hImc,0,NULL,0);
if(dwSize > 0)
{
CandidateList = (LPCANDIDATELIST)GlobalAlloc( GPTR, dwSize );
g_bDrawIme = true;
ImmGetCandidateList(hImc,0,CandidateList,dwSize);
}
else memset(CandidateList,0,sizeof(CandidateList));
}
else if(wParam == IMN_CLOSECANDIDATE)
{
g_bDrawIme = false;
}
ImmReleaseContext( hWnd, hImc );
}
}

return CallWindowProc(g_OldProc, hWnd, message, wParam, lParam);

}*/
char *MGUI_FindFocusTextEntryText(void)
{
	MGUI_Base *pNextControl = pRootPanel;
	while (pNextControl->pNext)
	{
		if (pNextControl->pNext->iClass == MGUI_TEXTENTRY)
		{
			if (((MGUI_TextEntry*)pNextControl->pNext)->m_bHasFocus)
			{
				return ((MGUI_TextEntry*)pNextControl->pNext)->m_pText;
			}
		}
		pNextControl = pNextControl->pNext;
	}
	return NULL;
}
void MGUI_SetFocusTextEntryDisabled(void)
{
	MGUI_Base *pNextControl = pRootPanel;
	while (pNextControl->pNext)
	{
		if (pNextControl->pNext->iClass == MGUI_TEXTENTRY)
		{
			if (((MGUI_TextEntry*)pNextControl->pNext)->m_bHasFocus)
			{
				((MGUI_TextEntry*)pNextControl->pNext)->m_bHasFocus = false;
			}
		}
		pNextControl = pNextControl->pNext;
	}
	return;
}


void MGUI_Init()
{
	// HOOK
	g_mgui_mouse = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(g_mgui_mouse, "gfx\\ui\\mouse\\mouse", true, true);

	vgui::surface()->DrawGetTextureSize(g_mgui_mouse, g_mouse_w, g_mouse_h);

	g_OldProc = (WNDPROC)GetWindowLong(g_hWnd, GWL_WNDPROC);

	SetWindowLong(g_hWnd, GWL_WNDPROC, (LONG)g_NewProc);
}
bool MGUI_IsInRect(int x, int w, int y, int h)
{
	int iMouseX, iMouseY;
	gEngfuncs.GetMousePosition(&iMouseX, &iMouseY);
	if (iMouseX>x && iMouseX< x + w && iMouseY>y && iMouseY<y + h) return true;
	return false;
}
void MGUI_FreePanel(MGUI_Panel *pPanel)
{
	MGUI_Base *pNext = (MGUI_Base *)pPanel;
	MGUI_Base *pTemp;
	if (pNext)
	{
		pTemp = pNext;
		pNext = pNext->pNext;
		delete pTemp;
	}
}
void MGUI_ProcessMessage(void *pPointer)
{
	if (!pPointer)
		return;
	
	static int iMoving = 0;
	static int iMousePressedPosX = 0;
	static int iMousePressedPosY = 0;
	static int iMousePosX = 0;
	static int iMousePoxY = 0;
	static int iMouseMoveW = 0;
	static int iMouseMoveH = 0;

	gEngfuncs.GetMousePosition(&iMousePosX, &iMousePoxY);
	// Draw Mouse
	//GL_DrawTGA(g_mgui_mouse,255,255,255,255,iMousePosX,iMousePoxY,1.0f);
	//GL_DrawTGA2(g_mgui_mouse,iMousePosX,iMousePoxY,g_MHTga[g_mgui_mouse].width,g_MHTga[g_mgui_mouse].height,255);
	MGUI_Base *pBase = (MGUI_Base *)pPointer;
	int iClass = pBase->iClass;

	if (iClass == MGUI_PANEL)
	{
		int m_iClosing = ((MGUI_Panel *)pPointer)->m_iClosing;
		int m_bHasClose = ((MGUI_Panel *)pPointer)->m_bHasClose;
		if ((g_mgui_mouseevent & ME_LEFTCLICK) && (!(g_mgui_oldmouseevent & ME_LEFTCLICK)) && MGUI_IsInRect(pBase->x + pBase->w - 20, 20, pBase->y, 20) && !m_iClosing && m_bHasClose)
		{
			((MGUI_Panel *)pPointer)->m_iClosing = 1;
			g_mgui_candraw = 0;
		}
		if (m_iClosing == 1) return;

		if (((MGUI_Panel *)pPointer)->m_bCanMove == false) return;
		if ((g_mgui_mouseevent & ME_LEFTCLICK) && (!(g_mgui_oldmouseevent & ME_LEFTCLICK)) && ((MGUI_Panel *)pPointer)->IsInRect())
		{
			iMoving = 1;
		}
		else if (!(g_mgui_mouseevent & ME_LEFTCLICK)) iMoving = 0;

		if (iMoving)
		{

			if (!iMousePressedPosX)
			{
				gEngfuncs.GetMousePosition(&iMousePressedPosX, &iMousePressedPosY);
				pBase->prevx = pBase->x;
				pBase->prevy = pBase->y;
			}
			iMouseMoveW = iMousePosX - iMousePressedPosX;
			iMouseMoveH = iMousePoxY - iMousePressedPosY;
		}
		else
		{
			pBase->prevx = pBase->prevx + iMouseMoveW;
			pBase->prevy = pBase->prevy + iMouseMoveH;

			iMousePressedPosX = 0;
			iMouseMoveW = 0;
			iMouseMoveH = 0;
		}
		pBase->x = pBase->prevx + iMouseMoveW;
		pBase->y = pBase->prevy + iMouseMoveH;

	}
	else if (iClass == MGUI_TEXTENTRY)
	{
		MGUI_TextEntry *pEntry = (MGUI_TextEntry *)pPointer;
		MGUI_Panel *pEntryPanel = (MGUI_Panel *)pEntry->pParent;
		if (pEntryPanel->m_iClosing) return;
		if ((g_mgui_mouseevent & ME_LEFTCLICK) && pEntry->IsInRect())
		{
			pEntry->m_bHasFocus = true;
		}
		else if (g_mgui_mouseevent & ME_LEFTCLICK) pEntry->m_bHasFocus = false;
	}
	else if (iClass == MGUI_MENU)
	{
		MGUI_Menu *pMenu = (MGUI_Menu *)pPointer;
		MGUI_Panel *pMenuPanel = (MGUI_Panel *)pMenu->pParent;
		if (pMenuPanel->m_iClosing) return;
		pMenu->IsInRect();
	}
	else if (iClass == MGUI_BUTTON)
	{
		MGUI_Button *pButton = (MGUI_Button *)pPointer;
		MGUI_Panel *pMenuPanel = (MGUI_Panel *)pButton->pParent;
		if (pMenuPanel->m_iClosing)
			return;

		if (g_mgui_keynum == pButton->m_iKey && g_mgui_keynum >= 0)
			pButton->Click();

		if (pButton->IsInRect())
		{
			pButton->m_iStat = 1;
			if (g_mgui_mouseevent & ME_LEFTCLICK)
				pButton->m_iStat = 2;

			if (g_mgui_mouseevent == 0 && g_mgui_oldmouseevent & ME_LEFTCLICK)
			{
				pButton->m_iStat = 1;
				g_mgui_oldmouseevent = 0;
				pButton->Click();
			}
			if (g_mgui_mouseevent & ME_RIGHTCLICK)
			{
				if ((g_mgui_mouseevent & ME_RIGHTCLICK) && (!(g_mgui_oldmouseevent & ME_RIGHTCLICK)))
				{			
					pButton->Click2();
					g_mgui_mouseevent = 0;
				}
			}
		}
		else
		{
			pButton->m_iStat = 0;
		}
	}
}

void MGUI_Redraw()
{
	//gEngfuncs.pfnFillRGBA(0,0,62.5,10,0,188,240,255);
	if (g_mgui_candraw || g_iMenuActive)
	{
		iCanSetMouse = 1;
		//ShowCursor(false);
		gExportfuncs.IN_DeactivateMouse();

		BTEPanel_BuyMenu_SetTime();
	}
	else
	{
		if (iCanSetMouse)
		{
			SetCursorPos(gEngfuncs.GetWindowCenterX(), gEngfuncs.GetWindowCenterY());
			iCanSetMouse = 0;
		}
		gExportfuncs.IN_ActivateMouse();
		if (g_iMini)
			UI_Draw();
		//ShowCursor(true);
		return;
	}
	if (g_iMenuActive)
	{
		UI_Draw();
		return;
	}

	//MGUI_Base *pNextControl = pRootPanel;
	MGUI_Panel *pSubPanel = pRootPanel;
	// Draw Panel
	while (pSubPanel)
	{
		MGUI_Base *pNextControl = pSubPanel;

		// Draw X
		if (!(pSubPanel->m_iClosing))
		{
			MGUI_ProcessMessage(pSubPanel);
			//GL_DrawTGA2(pSubPanel->m_iTexture,pSubPanel->x,pSubPanel->y,pSubPanel->w,pSubPanel->h,255);
			if (pSubPanel->m_iHaveBg)
				GL_DrawFullPanel(pSubPanel->m_iTexture, pSubPanel->x, pSubPanel->y, pSubPanel->w, pSubPanel->h, 255);

			if (pSubPanel->m_bHasClose)
			{
				g_Font.SetWidth(12);
				g_Font.SetColor(255, 255, 255, 255);
				g_Font.DrawString(L"X", pSubPanel->x + pSubPanel->w - 20, pSubPanel->y + 20, 100);
			}
			// Set Title
			if (pSubPanel->m_pTitle)
			{
				g_FontBold.SetWidth(pSubPanel->m_iTitleSize);
				int iLen = g_FontBold.GetLen(pSubPanel->m_pTitle);
				int iStartX;
				if (pSubPanel->m_bCenterTitle)
					iStartX = pSubPanel->x + (pSubPanel->w - iLen) / 2;
				else
					iStartX = pSubPanel->x + 20;
				g_FontBold.SetColor(255, 255, 255, 255);
				g_FontBold.DrawString(pSubPanel->m_pTitle, iStartX, pSubPanel->y + 30, 1000);
			}
			// Draw Sub Control

			while (pNextControl->pNext)
			{
				if (pSubPanel->m_bCanControl)
					MGUI_ProcessMessage(pNextControl->pNext);
				if (pNextControl->pfnPrePaint)
					pNextControl->pfnPrePaint(pNextControl);
				pNextControl->pNext->Paint(pSubPanel);
				if (pNextControl->pfnPostPaint)
					pNextControl->pfnPostPaint(pNextControl);
				pNextControl = pNextControl->pNext;
			}
		}
		pSubPanel = pSubPanel->pNextPanel;
	}

	int iMousePosX = 0;
	int iMousePoxY = 0;
	gEngfuncs.GetMousePosition(&iMousePosX, &iMousePoxY);

	GL_DrawTGA2(g_mgui_mouse, iMousePosX, iMousePoxY, g_mouse_w, g_mouse_h, 255);

}

void MGUI_ShutDown(void)
{
	return;
}