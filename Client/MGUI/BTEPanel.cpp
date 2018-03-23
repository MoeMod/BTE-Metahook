#include "base.h"
#include "hud.h"
#include "exportfuncs.h"
#include "BaseUI.h"
#include "DrawTargaImage.h"
#include "Fonts.h"
#include "message.h"
#include "util.h"
#include "common.h"

#include <vector>

#include "mgui.h"
#include "BTEPanel.h"
#include "TeamMenu.h"
#include "NormalZombieMenu.h"

#include "weapons.h"
#include "Client/PlayerClassManager.h"
#include "Client/WeaponManager.h"

#include "Client/HUD/DrawTGA.h"

//#define BUTTON_STATIC_DEFINE

// TDM UI
MGUI_Panel *pTeamSuitPanel;
// -- 记录可以改变的标签指针 共12个
MGUI_Label *pTeamSuitLabel[24];
// -- 记录可以改变的图片指针 共12个
MGUI_Image *pTeamSuitImage[12];
MGUI_Image *pTeamSuitImage2[12];
// -- 记录按钮 共4个
MGUI_Button *pTeamSuitButton[4];

// BUY MENU
char MENU[10][32] = { "#CSO_Pistols", "#CSO_Shotguns", "#CSO_SubMachineGuns", "#CSO_Rifles",
"#CSO_MachineGuns", "#CSO_Prim_Ammo", "#CSO_Sec_Ammo", "#CSO_Equipment",
"#CSO_ProximityWpn", "#CSO_EndWpnBuy" };

char CMD[10][32] = { "MGUI.BuyMenu_Show 0", "MGUI.BuyMenu_Show 1", "MGUI.BuyMenu_Show 2", "MGUI.BuyMenu_Show 3",
"MGUI.BuyMenu_Show 4", "primammo", "secammo", "MGUI.BuyMenu_Show 5",
"MGUI.BuyMenu_Show 6", "MGUI.BuyMenu_BuyDefault" };
char *WEAPON_INFO_FORMAT[5] = { "#CStrike_%sPrice", "#CStrike_%sCalibre", "#CStrike_%sClipCapacity", "#CStrike_%sRateOfFire", "#CStrike_%sWeightLoaded" };
char *WEAPON_INFO_LABEL[5] = { "#CStrike_PriceLabel", "#CStrike_CalibreLabel", "#CStrike_ClipCapacityLabel", "#CStrike_RateOfFireLabel", "#CStrike_WeightLoadedLabel" };
char QUICKBUY_INI[4][16] = { "Primary", "Secondary", "Knife", "Grenade" };

MGUI_Panel *pBuyMenuPanel;
MGUI_Label *pBuyMenuLabel[10];
MGUI_Label *pBuyMenuLabelKey[10];
MGUI_Button *pBuyMenuButton[10];
MGUI_Button *pBuyMenuButton2[2];
MGUI_Button *pBuyMenuButtonPage[2];
MGUI_Image *pBuyMenuImage[10];
MGUI_Image *pBuyMenuImageWpn[10];

//MGUI_Image *pBuyMenuImageQuickBuy[10];
MGUI_Image *pBuyMenuImageQuickBuy[2][5];
//MGUI_Image *pBuyMenuImageFav[5];
//MGUI_Image *pBuyMenuImageFavBg[5];
MGUI_Image *pBuyMenuImageFavWpn[5][3];
MGUI_Image *pBuyMenuImageFavKey[5];
MGUI_Label *pBuyMenuLabelFavKey[10];
MGUI_Button *pBuyMenuButtonFav[5];
MGUI_Button *pBuyMenuButtonSaveFav[5];

MGUI_Button *pBuyMenuButtonSlot[2];
MGUI_Label *pBuyMenuLabelSlot;
MGUI_Image *pBuyMenuImageSlot;

MGUI_Image *pBuyMenuPanelImageTimer[2];
MGUI_Label *pBuyMenuPanelLabelTimer[2];
MGUI_Label *pBuyMenuPanelLabelTimer2[2];

MGUI_Panel *pBuyMenuPanel_WpnInfo[10];
MGUI_Image *pBuyMenuPanelImage_WpnInfo[10][2];
MGUI_Label *pBuyMenuPanelLabel_WpnInfo[10][2][5];

MGUI_Button *pBuyMenuButtonClear;
MGUI_Button *pBuyMenuButtonBuy;
MGUI_Button *pBuyMenuButtonClose;

int iPage = 0;
int iType = 0;

int iTDMSlot = 0;

char configPath[128];
char configPathFavWpn[128];
char configPathTeamSuit[128];
bool bFirst = true;
bool bTypeOpen = false;

char szFavWpn[6][4][32];
char szTeamSuitWpn[3][4][32];
char szDefaultWpn[4][32];
char szMonthWpn[32];

int g_UI_Panel[3][3];
int g_UI_Panel_Dark[3][3];
int g_UI_Output[3][3];
int g_UI_Button[3][3];
int g_UI_Button_Glow[3][3];
int g_UI_Panel_Gray[3][3];
int g_UI_Panel_TeamSuit[3][3];
int g_UI_Panel2[3][3];
int g_UI_TextEntry[3][3];
int g_iScrollBarButton[2][3], g_iScrollBar[3];

//int iWeaponId[7][256];
//int iWeaponIdInList[9];
std::vector<CWeaponManager::CustomWeapon *> g_WeaponList[7];

int bFromEquipMenu;

void BTEPanel_BuyMenu_First();
int	BTEPanel_BuyMenu_Read(int iSolt);
void BTEPanel_TeamSuit_Read(int iSlot);
void Cmd_BuyEquip();
void Cmd_BuyEquip_ZB();
void Cmd_BuyEquip_Advanced(void);
void BuyWeapon(char *sName, int iSlot);
void MakeWeaponList();

int iMonthWeaponBG, iMonthWeaponBG2;
Tga iMonthWeaponBGLabel;

wchar_t *GetWeaponInfoFormat(int iSlot, char *pItem)
{
	char *pTemp = AliasWeaponName(pItem);
	auto &Item = WeaponManager()[pItem];
	if (!iSlot)
	{
		if (!stricmp(pTemp, "kevlar_helmet"))
			pTemp = "kevlarhelmet";
		char *pPrice = va(": $%d", Item.iPrice);
		if (GetLangUtf(va(WEAPON_INFO_FORMAT[iSlot], pTemp))[0] == '#')
			return UTF8ToUnicode(pPrice);
	}
	return GetLangUni(va(WEAPON_INFO_FORMAT[iSlot], pTemp));
}


void BuyMenuButton_PrePaint(MGUI_Base *control)
{
	if (((MGUI_Button *)control)->m_bVisible)
		GL_DrawTGA2(iMonthWeaponBG, control->x + control->pParent->x, control->y + control->pParent->y, control->w, control->h, 255);
}

void BuyMenuQuickBuyButton_PrePaint(MGUI_Base *control)
{
	if (((MGUI_Image *)control)->m_bVisible)
	{
		GL_DrawTGA2(iMonthWeaponBG2, control->x + control->pParent->x, control->y + control->pParent->y, control->w, control->h, 255);
		GL_DrawTGA(iMonthWeaponBGLabel.iTexture, 255, 255, 255, 255, control->x + control->pParent->x, control->y + control->pParent->y + control->h - iMonthWeaponBGLabel.iHeight, 1.0);
	}
}


void BTEPanel_TeamSuit_Reset()
{
	bFirst = true;
	pTeamSuitButton[0]->m_fNextClick = 0.0f;
	pTeamSuitButton[1]->m_fNextClick = 0.0f;
	pTeamSuitButton[2]->m_fNextClick = 0.0f;
	pTeamSuitButton[3]->m_fNextClick = 0.0f;
}
void BTEPanel_TeamSuit_Save(void)
{
	char szText[16];
	sprintf(szText, "TeamSuit%d", iTDMSlot + 1);
	for (int i = 0; i <= 3; i++)
	{
		WritePrivateProfileString(szText, QUICKBUY_INI[i], szFavWpn[5][i], configPathTeamSuit);
	}
	char szCmd[256];
	sprintf(szCmd, "bte_dm_set_weapon %s %s %s %s;bte_dm_buy", szFavWpn[5][0], szFavWpn[5][1], szFavWpn[5][2], szFavWpn[5][3]);

	BTEPanel_TeamSuit_UpdateItems(iTDMSlot, szFavWpn[5][0], szFavWpn[5][1], szFavWpn[5][2], szFavWpn[5][3]);
	pTeamSuitButton[iTDMSlot + 1]->SetCommand(szCmd);

	BTEPanel_TeamSuit_Show();
}
void BTEPanel_TeamSuit_Show(void)
{
	pTeamSuitPanel->m_iClosing = 0;
	pBuyMenuPanel->m_iClosing = 1;
	g_TeamMenu.Close();
	g_NormalZombieMenu.Close();
	pTeamSuitPanel->m_bCanControl = true;
	for (int i = 0; i <= 9; i++)
		pBuyMenuPanel_WpnInfo[i]->m_iClosing = 1;
	g_mgui_candraw = 1;

	for (int i = 0; i <= 2; i++)
	{
		if (g_iWeaponLimit == 0 || g_iWeaponLimit == 1)
			pTeamSuitImage2[i]->SetVisible(false);
		else
			pTeamSuitImage2[i]->SetVisible(true);

		if (g_iWeaponLimit == 0 || g_iWeaponLimit == 2)
			pTeamSuitImage2[i + 3]->SetVisible(false);
		else
			pTeamSuitImage2[i + 3]->SetVisible(true);

		if (g_iWeaponLimit == 0 || g_iWeaponLimit == 3)
			pTeamSuitImage2[i + 9]->SetVisible(false);
		else
			pTeamSuitImage2[i + 9]->SetVisible(true);

		pTeamSuitImage2[i + 6]->SetVisible(false);
	}
}

void BTEPanel_TeamSuit_SetSlot(int iSlotDelta)
{
	iTDMSlot += iSlotDelta;
	if (iTDMSlot >= 3) iTDMSlot = 0;
	if (iTDMSlot <= -1) iTDMSlot = 2;
	char szText[32];
	sprintf(szText, "#CSO_%cSet", iTDMSlot + 'A');
	pBuyMenuLabelSlot->SetLabel(GetLangUni(szText));

	for (int i = 0; i <= 3; i++)
	{
		BTEPanel_BuyMenu_SetQuickBuy(i + 1, szTeamSuitWpn[iTDMSlot][i]);
	}
	BTEPanel_BuyMenu_UpdateItems(-1, "");
	BTEPanel_BuyMenu_SetPage(0, 0);

}
void BTEPanel_TeamSuit_UpdateItems(int iSlot, char *pItems1, char *pItems2, char *pItems3, char *pItems4)
{
	char szTga[64];

	sprintf(szTga, "gfx\\vgui\\%s", pItems1);
	pTeamSuitImage[iSlot]->SetTexture(szTga);
	pTeamSuitLabel[iSlot]->SetLabel(GetWeaponNameFormat(pItems1));

	sprintf(szTga, "gfx\\vgui\\%s", pItems2);
	pTeamSuitImage[iSlot + 3]->SetTexture(szTga);
	pTeamSuitLabel[iSlot + 3]->SetLabel(GetWeaponNameFormat(pItems2));

	sprintf(szTga, "gfx\\vgui\\%s", pItems3);
	pTeamSuitImage[iSlot + 6]->SetTexture(szTga);
	pTeamSuitLabel[iSlot + 6]->SetLabel(GetWeaponNameFormat(pItems3));

	sprintf(szTga, "gfx\\vgui\\%s", pItems4);
	pTeamSuitImage[iSlot + 9]->SetTexture(szTga);
	pTeamSuitLabel[iSlot + 9]->SetLabel(GetWeaponNameFormat(pItems4));

}
void BTEPanel_TeamSuit_Read(int iSlot)
{
	char name[16];
	sprintf(name, "TeamSuit%d", iSlot + 1);
	for (int i = 0; i <= 3; i++)
	{
		char value[1024];
		GetPrivateProfileStringA(name, QUICKBUY_INI[i], "", value, sizeof(value), configPathTeamSuit);
		if (strlen(value))
		{
			strcpy(szTeamSuitWpn[iSlot][i], value);
			pTeamSuitImage[iSlot + i * 3]->SetTexture(va("gfx\\vgui\\%s", value));
			pTeamSuitLabel[iSlot + i * 3]->SetLabel(GetWeaponNameFormat(value));
		}
	}
	char szCmd[256];
	sprintf(szCmd, "bte_dm_set_weapon %s %s %s %s;bte_dm_buy", szTeamSuitWpn[iSlot][0], szTeamSuitWpn[iSlot][1], szTeamSuitWpn[iSlot][2], szTeamSuitWpn[iSlot][3]);
	pTeamSuitButton[iSlot + 1]->SetCommand(szCmd);

}
void BTEPanel_Init(void)
{
	strcpy(configPath, "cstrike/buymenu.ini");
	strcpy(configPathFavWpn, "cstrike/quickbuy.ini");
	strcpy(configPathTeamSuit, "cstrike/teamsuit.ini");

	//GL_LoadFullPanel(g_UI_Panel2,"igframe");
	GL_LoadFullPanel(g_UI_Panel, "window");
	GL_LoadFullPanel(g_UI_Panel_Dark, "window_dark");
	GL_LoadFullPanel(g_UI_Output, "output");
	GL_LoadFullPanel(g_UI_Panel_Gray, "gray");
	GL_LoadFullPanel(g_UI_Panel_TeamSuit, "teamsuit");
	GL_LoadFullPanel(g_UI_TextEntry, "input");

	GL_LoadPanel(g_UI_Button[0], "button\\n");
	GL_LoadPanel(g_UI_Button[1], "button\\o");
	GL_LoadPanel(g_UI_Button[2], "button\\c");

	GL_LoadPanel(g_UI_Button_Glow[0], "button_glow\\n");
	GL_LoadPanel(g_UI_Button_Glow[1], "button_glow\\o");
	GL_LoadPanel(g_UI_Button_Glow[2], "button_glow\\c");

	for (int i = 0; i < 3; i++)
	{
		g_iScrollBarButton[0][i] = vgui::surface()->CreateNewTextureID();
		g_iScrollBarButton[1][i] = vgui::surface()->CreateNewTextureID();
		g_iScrollBar[i] = vgui::surface()->CreateNewTextureID();
	}

	vgui::surface()->DrawSetTextureFile(g_iScrollBarButton[0][0], "resource\\control\\scrollbar\\btn_default_scroll_up@n", true, false);
	vgui::surface()->DrawSetTextureFile(g_iScrollBarButton[0][1], "resource\\control\\scrollbar\\btn_default_scroll_up@o", true, false);
	vgui::surface()->DrawSetTextureFile(g_iScrollBarButton[0][2], "resource\\control\\scrollbar\\btn_default_scroll_up@c", true, false);

	vgui::surface()->DrawSetTextureFile(g_iScrollBarButton[1][0], "resource\\control\\scrollbar\\btn_default_scroll_down@n", true, false);
	vgui::surface()->DrawSetTextureFile(g_iScrollBarButton[1][1], "resource\\control\\scrollbar\\btn_default_scroll_down@o", true, false);
	vgui::surface()->DrawSetTextureFile(g_iScrollBarButton[1][2], "resource\\control\\scrollbar\\btn_default_scroll_down@c", true, false);

	vgui::surface()->DrawSetTextureFile(g_iScrollBar[0], "resource\\control\\scrollbar\\btn_default_scroll_bar_top@n", true, false);
	vgui::surface()->DrawSetTextureFile(g_iScrollBar[1], "resource\\control\\scrollbar\\btn_default_scroll_bar_center@n", true, false);
	vgui::surface()->DrawSetTextureFile(g_iScrollBar[2], "resource\\control\\scrollbar\\btn_default_scroll_bar_bottom@n", true, false);


	// 初始化团队套装
	pTeamSuitPanel = MGUI_CreatePanel(50, 100, 602, 550, 20, true/*是否可以移动*/);
	//pTeamSuitPanel->SetTexture("");
	wcscpy(pTeamSuitPanel->m_pTitle, GetLangUni("#CSO_WeaponSelect_Label"));
	pTeamSuitPanel->m_iTitleSize = 16;
	pTeamSuitPanel->m_bHasClose = false; // 是否提供X关闭面板
	//pTeamSuitPanel->pNextPanel = 0x0;

	//
	pTeamSuitButton[0] = MGUI_CreateButton(pTeamSuitPanel, 230, 450, 150, 32);
	pTeamSuitButton[0]->SetLabel(GetLangUni("#CSO_SetWeaponEdit"));
	pTeamSuitButton[0]->SetCommand("MGUI.ShowBuyMenu");
	pTeamSuitButton[0]->SetType(MGUI_BUTTON_TYPE_COMMAND_CLOSEPANEL);
	pTeamSuitButton[0]->m_iSize = 14;
#ifdef BUTTON_STATIC_DEFINE
	pTeamSuitButton[0]->SetKey('e');
#endif
	//pTeamSuitButton[0]->SetImage3(g_UI_Button);	
	memcpy(pTeamSuitButton[0]->iTga3, g_UI_Button, sizeof(g_UI_Button));
	pTeamSuitButton[0]->m_Tex_Type = MGUI_TEX_TYPE_3;

	// 标题 第一行
	int iX = 60;
	int iY = 60;

	MGUI_Label *pLabel = MGUI_CreateLabel(pTeamSuitPanel, iX + 20, iY, 100, 20);
	pLabel->SetColor(255, 251, 219, 255);
	pLabel->SetSize(14);
	pLabel->SetLabel(GetLangUni("#CSO_ASet"));
	// 背景图	
	MGUI_Image *pImage = MGUI_CreateImage(pTeamSuitPanel, iX - 35, iY - 10, 180, 382, true, false);
	pImage->m_iTex_Type = MGUI_TEX_TYPE_9;
	memcpy(pImage->m_iTexture9, g_UI_Panel_Gray, sizeof(g_UI_Panel_Gray));

	pTeamSuitButton[1] = MGUI_CreateButton(pTeamSuitPanel, iX - 35, iY - 10, 180, 382);
	pTeamSuitButton[1]->SetCommand("");
	pTeamSuitButton[1]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
	//pTeamSuitButton[1]->SetImage("","other\\ui\\panel\\equipbg","other\\ui\\panel\\equipbg");
	pTeamSuitButton[1]->m_Tex_Type = MGUI_TEX_TYPE_9;
	memcpy(pTeamSuitButton[1]->iTga9[1], g_UI_Panel_TeamSuit, sizeof(g_UI_Panel_TeamSuit));
	memcpy(pTeamSuitButton[1]->iTga9[2], g_UI_Panel_TeamSuit, sizeof(g_UI_Panel_TeamSuit));
	//pTeamSuitButton[1]->iTga[1] = pTeamSuitButton[1]->iTga[2] = Hud().m_TGA.GetTGA("other\\ui\\panel\\equipbg");
	pTeamSuitButton[1]->SetKey('1');

	iX += 185;
	pLabel = MGUI_CreateLabel(pTeamSuitPanel, iX + 20, iY, 100, 20);
	pLabel->SetColor(255, 251, 219, 255);
	pLabel->SetSize(14);
	pLabel->SetLabel(GetLangUni("#CSO_BSet"));
	pImage = MGUI_CreateImage(pTeamSuitPanel, iX - 35, iY - 10, 180, 382, true, false);
	pImage->m_iTex_Type = MGUI_TEX_TYPE_9;
	memcpy(pImage->m_iTexture9, g_UI_Panel_Gray, sizeof(g_UI_Panel_Gray));
	pTeamSuitButton[2] = MGUI_CreateButton(pTeamSuitPanel, iX - 35, iY - 10, 180, 382);
	pTeamSuitButton[2]->SetCommand("");
	pTeamSuitButton[2]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
	//pTeamSuitButton[2]->SetImage("","other\\ui\\panel\\equipbg","other\\ui\\panel\\equipbg");
	pTeamSuitButton[2]->m_Tex_Type = MGUI_TEX_TYPE_9;
	memcpy(pTeamSuitButton[2]->iTga9[1], g_UI_Panel_TeamSuit, sizeof(g_UI_Panel_TeamSuit));
	memcpy(pTeamSuitButton[2]->iTga9[2], g_UI_Panel_TeamSuit, sizeof(g_UI_Panel_TeamSuit));
	//pTeamSuitButton[2]->iTga[1] = pTeamSuitButton[2]->iTga[2] = Hud().m_TGA.GetTGA("other\\ui\\panel\\equipbg");
	pTeamSuitButton[2]->SetKey('2');

	iX += 185;
	pLabel = MGUI_CreateLabel(pTeamSuitPanel, iX + 20, iY, 100, 20);
	pLabel->SetColor(255, 251, 219, 255);
	pLabel->SetSize(14);
	pLabel->SetLabel(GetLangUni("#CSO_CSet"));
	pImage = MGUI_CreateImage(pTeamSuitPanel, iX - 35, iY - 10, 180, 382, true, false);
	pImage->m_iTex_Type = MGUI_TEX_TYPE_9;
	memcpy(pImage->m_iTexture9, g_UI_Panel_Gray, sizeof(g_UI_Panel_Gray));
	pTeamSuitButton[3] = MGUI_CreateButton(pTeamSuitPanel, iX - 35, iY - 10, 180, 381);
	pTeamSuitButton[3]->SetCommand("");
	pTeamSuitButton[3]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
	//pTeamSuitButton[3]->SetImage("","other\\ui\\panel\\equipbg","other\\ui\\panel\\equipbg");
	pTeamSuitButton[3]->m_Tex_Type = MGUI_TEX_TYPE_9;
	memcpy(pTeamSuitButton[3]->iTga9[1], g_UI_Panel_TeamSuit, sizeof(g_UI_Panel_TeamSuit));
	memcpy(pTeamSuitButton[3]->iTga9[2], g_UI_Panel_TeamSuit, sizeof(g_UI_Panel_TeamSuit));
	//pTeamSuitButton[3]->iTga[1] = pTeamSuitButton[3]->iTga[2] = Hud().m_TGA.GetTGA("other\\ui\\panel\\equipbg");
	pTeamSuitButton[3]->SetKey('3');

	// 图片背景 及内容
	int iDefineX = 34;
	int iDefineY = iY + 23; // 45
	// 共12 个

	for (int iLine = 0; iLine < 4; iLine++)
	{
		for (int iRow = 0; iRow < 3; iRow++)
		{
			iX = iRow * 185 + iDefineX;
			iY = iDefineY + 85 * iLine; // iY = iDefineY + 100* iLine;


			// -- 下面的武器名字
			pTeamSuitLabel[iLine * 3 + iRow] = MGUI_CreateLabel(pTeamSuitPanel, iX + 5, iY + 63, 164, 20);
			pTeamSuitLabel[iLine * 3 + iRow]->SetColor(255, 255, 255, 255);
			pTeamSuitLabel[iLine * 3 + iRow]->SetSize(14);
			pTeamSuitLabel[iLine * 3 + iRow]->SetLabel(L"");

			pTeamSuitLabel[iLine * 3 + iRow + 12] = MGUI_CreateLabel(pTeamSuitPanel, iX + 8, iY + 40, 164, 20);
			pTeamSuitLabel[iLine * 3 + iRow + 12]->SetColor(128, 128, 128, 255);
			pTeamSuitLabel[iLine * 3 + iRow + 12]->SetSize(13);

			pImage = MGUI_CreateImage(pTeamSuitPanel, iX, iY, 164, 63, true, false);
			pImage->SetTexture("gfx\\ui\\panel\\equipslot");
			pTeamSuitImage[iLine * 3 + iRow] = MGUI_CreateImage(pTeamSuitPanel, iX + 7, iY + 4, 150, 53, true, false);// 武器tga
			pTeamSuitImage[iLine * 3 + iRow]->m_iTexture = 0;

			pTeamSuitImage2[iLine * 3 + iRow] = MGUI_CreateImage(pTeamSuitPanel, iX + 7, iY + 4, 150, 53, true, false);// 武器tga
			pTeamSuitImage2[iLine * 3 + iRow]->SetTexture("gfx\\vgui\\cannotuse");
			pTeamSuitImage2[iLine * 3 + iRow]->SetVisible(true);
			//pTeamSuitImage[iLine*3 + iRow]->SetTexture("gfx\\vgui\\cannotuse");
		}
	}

	for (int i = 0; i <= 2; i++)
	{

		pTeamSuitLabel[i + 12]->SetLabel(GetLangUni("#CSO_PrimaryWeapon_Holder"));
		pTeamSuitLabel[i + 3 + 12]->SetLabel(GetLangUni("#CSO_SecondaryWeapon_Holder"));
		pTeamSuitLabel[i + 6 + 12]->SetLabel(GetLangUni("#CSO_Knife_Holder"));
		pTeamSuitLabel[i + 9 + 12]->SetLabel(GetLangUni("#CSO_HEGrenade_Holder"));

		BTEPanel_TeamSuit_Read(i);
	}

	/*for(int iLine = 0; iLine <4;iLine++)
	{
	for(int iRow = 0 ; iRow <3 ; iRow++)
	{
	iX = iRow * 185 + iDefineX;
	iY = iDefineY + 85* iLine;
	pTeamSuitLabel[iLine*3 + iRow + 12] = MGUI_CreateLabel(pTeamSuitPanel,iX+8,iY+40,164,20);
	pTeamSuitLabel[iLine*3 + iRow + 12]->SetColor(128,128,128,255);
	pTeamSuitLabel[iLine*3 + iRow + 12]->SetSize(13);


	}
	for(int iRow = 0 ; iRow <3 ; iRow++)
	{
	iX = iRow * 175 + iDefineX;
	iY = iDefineY + 85* iLine; // iY = iDefineY + 100* iLine;
	// -- 下面的武器名字
	pTeamSuitLabel[iLine*3 + iRow] = MGUI_CreateLabel(pTeamSuitPanel,iX+5,iY+63,100,20);
	pTeamSuitLabel[iLine*3 + iRow]->SetColor(255,255,255,255);
	pTeamSuitLabel[iLine*3 + iRow]->SetSize(16);
	pTeamSuitLabel[iLine*3 + iRow]->SetLabel(L"");
	}
	}*/
	// 为什么不提供一个输入框呢 SB
	// MGUI_TextEntry *pTextEntry = MGUI_CreateTextEntry(pTeamSuitPanel,100,100,300);

	iMonthWeaponBG = g_Texture[Hud().m_TGA.FindTexture("resource\\month_weapon_longbg")].iTexture;
	iMonthWeaponBG2 = g_Texture[Hud().m_TGA.FindTexture("resource\\month_weapon_bg")].iTexture;
	iMonthWeaponBGLabel = g_Texture[Hud().m_TGA.FindTexture("resource\\month_weapon")];
}

void ShowBuyMenu(int type, int page)
{
	int bHavePrevPage = FALSE;

	if (type == 5 && page <= 0)
	{
		if (g_iMod == MOD_TDM || g_iMod == MOD_DM || g_iMod == MOD_ZB4)
		{
			bFromEquipMenu = FALSE;

			BTEPanel_BuyMenu_UpdateItems(9, "");
			BTEPanel_BuyMenu_SetPage(0, 5);

			char szName[9][32];
			memset(szName, 0, sizeof(szName));

			strcpy(szName[0], "vest");
			strcpy(szName[1], "vesthelm");
			strcpy(szName[2], "flash");
			strcpy(szName[3], "hegrenade");
			strcpy(szName[4], "sgren");
			strcpy(szName[5], "nvgs");

			for (int i = 0; i <= 8; i++)
				BTEPanel_BuyMenu_UpdateItems(i, szName[i]);

			int bHaveNextPage = FALSE;

			if (!g_WeaponList[5].empty() && g_WeaponList[5][0] && (g_WeaponList[5][0]->iTeam == g_iTeam || !g_WeaponList[5][0]->iTeam || g_iMod != MOD_NONE))
				bHaveNextPage = TRUE;

			BTEPanel_BuyMenu_SetHaveNextPage(0, bHaveNextPage);
		}
		else
		{
			page = 0;
			if (!IS_ZOMBIE_MODE)
				Cmd_BuyEquip();
			else
				Cmd_BuyEquip_Advanced();
			bFromEquipMenu = FALSE;
			BTEPanel_BuyMenu_UpdateItems(9, "");
		}

		return;
	}

	if (type == 5 && page > 0)
	{
		bHavePrevPage = TRUE;
		page = page - 1;
	}

	//int result[9];
	CWeaponManager::CustomWeapon *WeaponsInPage[9];
	memset(WeaponsInPage, 0, sizeof(WeaponsInPage));
	//decltype(g_WeaponInPage) result;
	int total = 0;
	int id = 0;

	for (auto x : g_WeaponList[type])
	{
		if (!x || !*x)
			break;

		if ((x->iTeam == g_iTeam || !x->iTeam || g_iMod != MOD_NONE))
		{
			if (total >= page * 9)
			{
				if (id < 9)
				{
					WeaponsInPage[id] = x;
					id++;
				}
			}
			total++;
		}
	}

	for (int i = 0; i <= 8; i++)
	{
		//iWeaponIdInList[i] = result[i];

		if (WeaponsInPage[i])
			BTEPanel_BuyMenu_UpdateItems(i, WeaponsInPage[i]->szName);
		else
			BTEPanel_BuyMenu_UpdateItems(i, "");
	}
	BTEPanel_BuyMenu_UpdateItems(9, "");
	BTEPanel_BuyMenu_SetPage(page, type);
	BTEPanel_BuyMenu_SetHaveNextPage(page > 0 || bHavePrevPage, total > id + page * 9);

}

#include "keydefs.h"

void BuyWeapon(char *sName, int iSlot)
{
	if (!sName || !strlen(sName))
		return;

	auto &Weapon = WeaponManager()[sName];
	//int i = FindWeaponData(sName);

	if (Weapon.iModeLimit && (!(Weapon.iModeLimit & (1 << 4)) && IS_ZOMBIE_MODE))
	{
		pBuyMenuPanel->MGUIMessageBox(GetLangUni("#CSO_MODE_LOCK_H"), GetLangUni("CSO_MODE_LOCK_B"), MB_OK, NULL, NULL, 13, -1);
		return;
	}

	if (!stricmp(sName, "defuser"))
	{
		if (strncmp(gEngfuncs.pfnGetLevelName(), "de_", 3))
		{
			pBuyMenuPanel->MGUIMessageBox(GetLangUni("#CSO_MAP_LOCK_H"), GetLangUni("CSO_MAP_LOCK_B"), MB_OK, NULL, NULL, 13, -1);
			return;
		}
		if (g_iMod != MOD_NONE && g_iMod != MOD_GHOST)
		{
			pBuyMenuPanel->MGUIMessageBox(GetLangUni("#CSO_MODE_LOCK_H"), GetLangUni("CSO_MODE_LOCK_B"), MB_OK, NULL, NULL, 13, -1);
			return;
		}
	}

	int id = gEngfuncs.GetLocalPlayer()->index;

	if ((!Q_stricmp(sName, "lugers") && Q_stricmp(PlayerClassManager()[id].model, "blair")) ||
		(!Q_stricmp(sName, "holybomb") && Q_stricmp(PlayerClassManager()[id].model, "fernando")))
	{
		pBuyMenuPanel->MGUIMessageBox(GetLangUni("#CSO_CLASS_LOCK_H"), GetLangUni("CSO_CLASS_LOCK_B"), MB_OK, NULL, NULL, K_SPACE, -1);
		return;
	}

	if (g_iMod == MOD_NONE || g_iMod == MOD_ZBS || g_iMod == MOD_GHOST)
	{
		char szCmd[256];
		sprintf(szCmd, "sv_create_psb 10397 %s", sName);
		gEngfuncs.pfnClientCmd(szCmd);
	}
	else
	{
		if (IS_ZOMBIE_MODE)
		{
			if (!stricmp(sName, "sgren") || !stricmp(sName, "flash") || !stricmp(sName, "nvgs"))
			{
				pBuyMenuPanel->MGUIMessageBox(GetLangUni("#CSO_MODE_LOCK_H"), GetLangUni("CSO_MODE_LOCK_B"), MB_OK, NULL, NULL, K_SPACE, -1);
				return;
			}
		}
		if (strstr(sName, "vest"))
			return;

		BTEPanel_BuyMenu_SetQuickBuy(iSlot, sName);
		if (g_iMod == MOD_DM || g_iMod == MOD_TDM || g_iMod == MOD_ZB4)
		{
			strcpy(szTeamSuitWpn[iTDMSlot][iSlot - 1], sName);
		}
		BTEPanel_BuyMenu_UpdateItems(-1, "");
		BTEPanel_BuyMenu_SetPage(0, 0);
	}
}

void BuyFavWeapon(int iSlot)
{
	if (g_iMod != MOD_NONE && g_iMod != MOD_ZBS && g_iMod != MOD_GHOST)
	{
		for (int i = 1; i <= 4; i++)
			BTEPanel_BuyMenu_SetQuickBuy(i, szFavWpn[iSlot][i - 1]);
	}
	else
	{
		char szCmd[512];
		sprintf(szCmd, "sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;secammo;primammo;vesthelm;defuser", szFavWpn[iSlot][0], szFavWpn[iSlot][1], szFavWpn[iSlot][2], szFavWpn[iSlot][3]);
		gEngfuncs.pfnClientCmd(szCmd);
	}

}

void BuyDefaultWeapon(void)
{
	gEngfuncs.pfnClientCmd(va("sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;primammo;secammo;vesthelm;defuser", szDefaultWpn[0], szDefaultWpn[1], szDefaultWpn[2], szDefaultWpn[3]));
}

void ClearQuickBuy(void)
{
	if (g_iMod == MOD_DM || g_iMod == MOD_TDM || g_iMod == MOD_ZB4)
	{
		for (int i = 1; i <= 4; i++)
			BTEPanel_BuyMenu_SetQuickBuy(i, szDefaultWpn[i - 1]);
		return;
	}
	for (int i = 1; i <= 4; i++)
		BTEPanel_BuyMenu_SetQuickBuy(i, "");
	pBuyMenuImageQuickBuy[1][3]->m_iTexture = -1;
}

void Cmd_Buy_Advanced(void)
{
	bTypeOpen = true;

	pBuyMenuPanel->m_bCanControl = true;

	if (g_iMod == MOD_ZB4)
	{
		BTEPanel_TeamSuit_Show();
	}
	else if (g_iMod == MOD_GHOST && g_iTeam == 2)
	{
		ShowBuyMenu(6, 0);
		BTEPanel_BuyMenu_SetPage(0, 0);
		bFromEquipMenu = TRUE;
	}
	else
	{
		BTEPanel_BuyMenu_UpdateItems(-1, "");
		BTEPanel_BuyMenu_SetPage(0, 0);
		bFromEquipMenu = FALSE;
	}

	g_mgui_candraw = 1;

	bTypeOpen = false;
}

void Cmd_Buy()
{
	//if (IS_ZOMBIE_MODE)
	//	return;

	bTypeOpen = true;

	pBuyMenuPanel->m_bCanControl = true;

	MakeWeaponList();

	//if (!g_bAlive)
	//	return;

	//if (g_bInBuyzone == FALSE)
	//	return;

	if (g_iMod == MOD_GHOST && g_iTeam == 2)
	{
		ShowBuyMenu(6, 0);
		bFromEquipMenu = TRUE;
	}
	else
	{
		BTEPanel_BuyMenu_UpdateItems(-1, "");
		bFromEquipMenu = FALSE;
	}

	g_mgui_candraw = 1;
	BTEPanel_BuyMenu_SetPage(0, 0);

	bTypeOpen = false;
}

void MakeWeaponList()
{
	//memset(iWeaponId, 0, sizeof(iWeaponId));
	for (int type = 0; type <= 6; type++)
	{
		/*
		int total = 0;

		for (int i = 1; i <= 255; i++)
		{
		if (!g_WeaponData[i].szName)
		break;

		if (g_WeaponData[i].bCanBuy && g_WeaponData[i].iMenu == type && g_WeaponData[i].iMenu >= 0 && strcmp(g_WeaponData[i].szName, "hegrenade"))
		{
		iWeaponId[type][total] = i;
		total ++;
		}
		}*/
		g_WeaponList[type].clear();
		for (auto &x : WeaponManager())
		{
			if (!x.szName)
				break;
			if (x.bCanBuy && x.iMenu == type && x.iMenu >= 0 && strcmp(x.szName, "hegrenade"))
			{
				g_WeaponList[type].push_back(&x);
			}
		}
	}
}

void Cmd_BuyEquip_Advanced(void)
{
	g_mgui_candraw = 1;
	BTEPanel_BuyMenu_UpdateItems(-1, "");
	BTEPanel_BuyMenu_SetPage(0, 5);

	char szName[9][32];
	memset(szName, 0, sizeof(szName));

	if (g_iMod == MOD_GHOST && g_iTeam == 2)
	{
		strcpy(szName[0], "ghost_vest");
	}
	else
	{
		if (g_iTeam == 1 || IS_ZOMBIE_MODE)
		{
			strcpy(szName[0], "vest");
			strcpy(szName[1], "vesthelm");
			strcpy(szName[2], "flash");
			strcpy(szName[3], "hegrenade");
			strcpy(szName[4], "sgren");
			strcpy(szName[5], "defuser");
			strcpy(szName[6], "nvgs");
		}
		else
		{
			strcpy(szName[0], "vest");
			strcpy(szName[1], "vesthelm");
			strcpy(szName[2], "flash");
			strcpy(szName[3], "hegrenade");
			strcpy(szName[4], "sgren");
			strcpy(szName[5], "nvgs");
		}
	}
	for (int i = 0; i <= 8; i++)
	{
		BTEPanel_BuyMenu_UpdateItems(i, szName[i]);
	}

	int bHaveNextPage = FALSE;

	if (!g_WeaponList[5].empty() && g_WeaponList[5][0] && (g_WeaponList[5][0]->iTeam == g_iTeam || !g_WeaponList[5][0]->iTeam || g_iMod != MOD_NONE))
		bHaveNextPage = TRUE;

	BTEPanel_BuyMenu_SetHaveNextPage(0, bHaveNextPage);

	bFromEquipMenu = TRUE;
}

void Cmd_BuyEquip()
{
	if (IS_ZOMBIE_MODE)
		return;

	if (!g_bAlive)
		return;

	if (g_bInBuyzone == FALSE)
		return;

	Cmd_BuyEquip_Advanced();
}

void Cmd_BuyEquip_ZB()
{
	Cmd_BuyEquip_Advanced();
}

void Cmd_TeamSuit()
{
	BTEPanel_TeamSuit_Show();
	g_mgui_candraw = 1;
}

void BTEBuyMenu_Init(void)
{
	gEngfuncs.pfnAddCommand("buy", Cmd_Buy);
	gEngfuncs.pfnAddCommand("buyequip", Cmd_BuyEquip);
	gEngfuncs.pfnAddCommand("teamsuit", Cmd_TeamSuit);

	memset(szFavWpn, 0, sizeof(szFavWpn));

	sprintf(szFavWpn[5][0], "mp5");
	sprintf(szFavWpn[5][1], "usp");
	sprintf(szFavWpn[5][2], "knife");
	sprintf(szFavWpn[5][3], "hegrenade");

	char name[16];
	sprintf(name, "QuickBuy%d", 0);
	for (int i = 0; i <= 4; i++)
	{
		GetPrivateProfileStringA(name, QUICKBUY_INI[i], "", szFavWpn[5][i], sizeof(szFavWpn[5][i]), configPathFavWpn);
		if (strlen(szFavWpn[5][i]))
		{
			char wpnname[64];
			sprintf(wpnname, "gfx\\vgui\\%s", szFavWpn[5][i]);
		}
	}

	GetPrivateProfileString("BUYMENUCONFIG", "MonthWeapon", "", szMonthWpn, sizeof(szMonthWpn), configPath);
	GetPrivateProfileString("BUYMENUCONFIG", "DefaultPrimaryWeapon", strlen(szMonthWpn) ? szMonthWpn : "ak47", szDefaultWpn[0], sizeof(szDefaultWpn[0]), configPath);
	GetPrivateProfileString("BUYMENUCONFIG", "DefaultPistol", "usp", szDefaultWpn[1], sizeof(szDefaultWpn[1]), configPath);
	GetPrivateProfileString("BUYMENUCONFIG", "DefaultMelee", "knife", szDefaultWpn[2], sizeof(szDefaultWpn[2]), configPath);
	strcpy(szDefaultWpn[3], "hegrenade");

	pBuyMenuPanel = MGUI_CreatePanel(0, 0, 0, 0, 22, false, true);
	wcscpy(pBuyMenuPanel->m_pTitle, GetLangUni("#CSO_WeaponSelectMenu"));
	pBuyMenuPanel->m_iTitleSize = 18;
	pBuyMenuPanel->m_bHasClose = false;

	for (int i = 0; i <= 9; i++)
	{

		pBuyMenuPanel_WpnInfo[i] = MGUI_CreatePanel(100, 100, 200, 200, 22, false);
		pBuyMenuPanel_WpnInfo[i]->m_pTitle[0] = '\0';
		pBuyMenuPanel_WpnInfo[i]->m_iTitleSize = 18;
		pBuyMenuPanel_WpnInfo[i]->m_bHasClose = false;
		pBuyMenuPanel_WpnInfo[i]->m_iHaveBg = 0;

		pBuyMenuPanelImage_WpnInfo[i][0] = MGUI_CreateImage(pBuyMenuPanel_WpnInfo[i], 0, 0, 0, 0, true, false);
		pBuyMenuPanelImage_WpnInfo[i][0]->m_iTex_Type = MGUI_TEX_TYPE_9;
		memcpy(pBuyMenuPanelImage_WpnInfo[i][0]->m_iTexture9, g_UI_Panel_Gray, sizeof(g_UI_Panel_Gray));
		pBuyMenuPanelImage_WpnInfo[i][1] = MGUI_CreateImage(pBuyMenuPanel_WpnInfo[i], 0, 0, 0, 0, false, false);
		pBuyMenuPanelImage_WpnInfo[i][1]->m_iTexture = 0;

		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				pBuyMenuPanelLabel_WpnInfo[i][k][j] = MGUI_CreateLabel(pBuyMenuPanel_WpnInfo[i], 0, 0, 0, 0);
				pBuyMenuPanelLabel_WpnInfo[i][k][j]->SetSize(15);
				if (j)
					pBuyMenuPanelLabel_WpnInfo[i][k][j]->SetColor(170, 120, 0, 255);
				else
					pBuyMenuPanelLabel_WpnInfo[i][k][j]->SetColor(250, 240, 140, 255);
				pBuyMenuPanelLabel_WpnInfo[i][k][j]->m_iAlignment = MGUI_Left;
			}

			pBuyMenuPanelLabel_WpnInfo[i][0][j]->SetLabel(GetLangUni(WEAPON_INFO_LABEL[j]));
		}

		pBuyMenuButton[i] = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuButton[i]->SetImage("gfx\\ui\\panel\\blank_slot", "gfx\\ui\\panel\\select", "gfx\\ui\\panel\\select");

		pBuyMenuImage[i] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
		pBuyMenuImage[i]->SetTexture("gfx\\ui\\panel\\key");

		pBuyMenuImageWpn[i] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
		pBuyMenuImageWpn[i]->SetVisible(false);

		pBuyMenuLabelKey[i] = MGUI_CreateLabel(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuLabelKey[i]->SetSize(14);
		pBuyMenuLabelKey[i]->SetColor(255, 255, 255, 255);
		char num[2];
		int n;
		n = i + 1;
		if (n == 10) n = 0;
		sprintf(num, "%d", n);
		wchar_t pWchar[2];
		g_Font.C2W(pWchar, 2, num);
		pBuyMenuLabelKey[i]->SetLabel(pWchar);
		pBuyMenuButton[i]->SetKey(num[0]);

		pBuyMenuLabel[i] = MGUI_CreateLabel(pBuyMenuPanel, 0, 0, 286, 43);
		pBuyMenuLabel[i]->SetSize(14);

	}


	for (int i = 0; i <= 1; i++)
	{
		pBuyMenuButtonPage[i] = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuButtonPage[i]->SetType(MGUI_BUTTON_TYPE_COMMAND);
		pBuyMenuButtonPage[i]->SetVisible(false);
		//pBuyMenuButtonPage[i]->SetImage3(g_UI_Button);
		memcpy(pBuyMenuButtonPage[i]->iTga3, g_UI_Button, sizeof(g_UI_Button));
		pBuyMenuButtonPage[i]->m_iSize = 14;
		pBuyMenuButtonPage[i]->m_Tex_Type = MGUI_TEX_TYPE_3;

		pBuyMenuButton2[i] = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuButton2[i]->SetType(MGUI_BUTTON_TYPE_COMMAND);
		pBuyMenuButton2[i]->SetColor(195, 221, 255, 255);
		//pBuyMenuButton2[i]->SetImage3(g_UI_Button_Glow);
		memcpy(pBuyMenuButton2[i]->iTga3, g_UI_Button_Glow, sizeof(g_UI_Button_Glow));
		pBuyMenuButton2[i]->m_iSize = 14;
		pBuyMenuButton2[i]->m_Tex_Type = MGUI_TEX_TYPE_3;
		pBuyMenuButton2[i]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
		pBuyMenuButton2[i]->m_iOffset[0] = 2;

		pBuyMenuPanelImageTimer[i] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
		pBuyMenuPanelImageTimer[i]->m_iTex_Type = MGUI_TEX_TYPE_9;
		memcpy(pBuyMenuPanelImageTimer[i]->m_iTexture9, g_UI_Output, sizeof(g_UI_Output));

		pBuyMenuPanelLabelTimer[i] = MGUI_CreateLabel(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuPanelLabelTimer[i]->SetColor(200, 200, 200, 255);
		pBuyMenuPanelLabelTimer[i]->SetSize(14);

		pBuyMenuPanelLabelTimer2[i] = MGUI_CreateLabel(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuPanelLabelTimer2[i]->SetColor(200, 200, 200, 255);
		pBuyMenuPanelLabelTimer2[i]->SetSize(14);
		pBuyMenuPanelLabelTimer2[i]->m_iAlignment = MGUI_Right;
	}
	pBuyMenuPanelLabelTimer[0]->SetLabel(GetLangUni("#Cstrike_Current_Money"));
	pBuyMenuPanelLabelTimer[1]->SetLabel(GetLangUni("#CSO_BuyTime"));

	pBuyMenuButtonPage[0]->SetLabel(GetLangUni("#CSO_PrevBuy"));
	pBuyMenuButtonPage[1]->SetLabel(GetLangUni("#CSO_NextBuy"));

	pBuyMenuButton2[0]->SetLabel(GetLangUni("#CSO_SetWeaponSelect"));
	pBuyMenuButton2[1]->SetLabel(GetLangUni("#CSO_AutoBuy2"));
	pBuyMenuButton2[0]->SetCommand("bte_wpn_rebuy;rebuy;secammo;primammo");

#ifdef BUTTON_STATIC_DEFINE
	pBuyMenuButton2[0]->SetKey('r');
	pBuyMenuButton2[1]->SetKey('a');
#endif

	pBuyMenuButtonPage[0]->SetKey('-');
	pBuyMenuButtonPage[1]->SetKey('=');


	for (int i = 0; i <= 4; i++)
	{
		/*pBuyMenuImageFavBg[i] = MGUI_CreateImage(pBuyMenuPanel,0 ,0, 0, 0, true, false);
		pBuyMenuImageFavBg[i]->SetTexture("other\\ui\\panel\\blank_slot");*/

		/*pBuyMenuImageFav[i] = MGUI_CreateImage(pBuyMenuPanel,0 ,0, 0, 0, true, false);
		pBuyMenuImageFav[i]->SetTexture("other\\ui\\panel\\blank_slot");*/
		//pBuyMenuImageFav[i]->SetVisible(false);

		pBuyMenuButtonFav[i] = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuButtonSaveFav[i] = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);

		pBuyMenuImageFavKey[i] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
		pBuyMenuImageFavKey[i]->SetTexture("gfx\\ui\\panel\\key");

		pBuyMenuLabelFavKey[i] = MGUI_CreateLabel(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuLabelFavKey[i]->SetSize(14);

		pBuyMenuImageQuickBuy[0][i] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
		pBuyMenuImageQuickBuy[1][i] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
		//pBuyMenuImageQuickBuy[1][i]->SetTexture("");

		for (int j = 0; j <= 2; j++)
		{
			pBuyMenuImageFavWpn[i][j] = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
			pBuyMenuImageFavWpn[i][j]->m_iTexture = -1;
		}


		pBuyMenuButtonFav[i]->SetType(0);
		pBuyMenuButtonFav[i]->SetEnabled(false);
		pBuyMenuButtonFav[i]->SetImage("gfx\\ui\\panel\\blank_slot", "gfx\\ui\\panel\\blank_slot", "gfx\\ui\\panel\\blank_slot");
		//char szCmd[64];
		//sprintf(szCmd,"MGUI.BuyMenu_SaveFav %d",i);
		//pBuyMenuButtonFav[i]->SetCommand2(szCmd);		

		if (BTEPanel_BuyMenu_Read(i))
		{
			pBuyMenuButtonFav[i]->SetImage("gfx\\ui\\panel\\favoritebg", "gfx\\ui\\panel\\select", "gfx\\ui\\panel\\select");
			pBuyMenuButtonFav[i]->SetEnabled(true);
		}

		pBuyMenuButtonSaveFav[i]->SetType(MGUI_BUTTON_TYPE_COMMAND);
		pBuyMenuButtonSaveFav[i]->SetEnabled(true);
		pBuyMenuButtonSaveFav[i]->m_Tex_Type = MGUI_TEX_TYPE_3;
		pBuyMenuButtonSaveFav[i]->m_iSize = 14;
		memcpy(pBuyMenuButtonSaveFav[i]->iTga3, g_UI_Button, sizeof(g_UI_Button));
		pBuyMenuButtonSaveFav[i]->SetCommand(va("MGUI.BuyMenu_SaveFav %d", i));
		pBuyMenuButtonSaveFav[i]->SetLabel(GetLangUni("#CSO_FavSave"));
	}
	pBuyMenuImageQuickBuy[0][0]->SetTexture("gfx\\ui\\panel\\blank_slot_priweapons");
	pBuyMenuImageQuickBuy[0][1]->SetTexture("gfx\\ui\\panel\\blank_slot_flashbangs");
	pBuyMenuImageQuickBuy[0][2]->SetTexture("gfx\\ui\\panel\\blank_slot_sndweapons");
	pBuyMenuImageQuickBuy[0][3]->SetTexture("gfx\\ui\\panel\\blank_slot_equipment");
	pBuyMenuImageQuickBuy[0][4]->SetTexture("gfx\\ui\\panel\\basket_blank_slot");

	pBuyMenuLabelFavKey[0]->SetLabel(L"S");
	pBuyMenuLabelFavKey[1]->SetLabel(L"D");
	pBuyMenuLabelFavKey[2]->SetLabel(L"F");
	pBuyMenuLabelFavKey[3]->SetLabel(L"G");
	pBuyMenuLabelFavKey[4]->SetLabel(L"H");

	pBuyMenuButtonFav[0]->SetKey('s');
	pBuyMenuButtonFav[1]->SetKey('d');
	pBuyMenuButtonFav[2]->SetKey('f');
	pBuyMenuButtonFav[3]->SetKey('g');
	pBuyMenuButtonFav[4]->SetKey('h');

	char szImg[64];
	sprintf(szImg, "gfx\\vgui\\%s", szFavWpn[5][0]);
	pBuyMenuImageQuickBuy[1][0]->SetTexture(szImg);
	if (!strcmp(szFavWpn[5][0], szMonthWpn))
		pBuyMenuImageQuickBuy[1][0]->pfnPrePaint = BuyMenuQuickBuyButton_PrePaint;
	else
		pBuyMenuImageQuickBuy[1][0]->pfnPrePaint = NULL;
	sprintf(szImg, "gfx\\vgui\\%s", szFavWpn[5][1]);
	pBuyMenuImageQuickBuy[1][1]->SetTexture(szImg);
	if (!strcmp(szFavWpn[5][1], szMonthWpn))
		pBuyMenuImageQuickBuy[1][1]->pfnPrePaint = BuyMenuQuickBuyButton_PrePaint;
	else
		pBuyMenuImageQuickBuy[1][1]->pfnPrePaint = NULL;
	sprintf(szImg, "gfx\\vgui\\%s", szFavWpn[5][3]);
	pBuyMenuImageQuickBuy[1][2]->SetTexture(szImg);
	pBuyMenuImageQuickBuy[1][3]->SetTexture("gfx\\vgui\\vesthelm");
	sprintf(szImg, "gfx\\vgui\\%s", szFavWpn[5][2]);
	pBuyMenuImageQuickBuy[1][4]->SetTexture(szImg);
	if (!strcmp(szFavWpn[5][2], szMonthWpn))
		pBuyMenuImageQuickBuy[1][4]->pfnPrePaint = BuyMenuQuickBuyButton_PrePaint;
	else
		pBuyMenuImageQuickBuy[1][4]->pfnPrePaint = NULL;

	pBuyMenuButtonClear = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
	pBuyMenuButtonClear->SetType(MGUI_BUTTON_TYPE_COMMAND);
	pBuyMenuButtonClear->SetCommand("MGUI.BuyMenu_ClearQuickBuy");
	pBuyMenuButtonClear->SetLabel(GetLangUni("#CSO_BasketClear"));
	pBuyMenuButtonClear->m_iSize = 14;
	memcpy(pBuyMenuButtonClear->iTga3, g_UI_Button, sizeof(g_UI_Button));
	pBuyMenuButtonClear->m_Tex_Type = MGUI_TEX_TYPE_3;

	pBuyMenuButtonBuy = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
	pBuyMenuButtonBuy->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
	pBuyMenuButtonBuy->SetColor(195, 221, 255, 255);
	//pBuyMenuButtonBuy->SetCommand("sv_create_psb 10397 mp5;sv_create_psb 10397 usp;sv_create_psb 10397 hegrenade;sv_create_psb 10397 knife;secammo;primammo");

	char szCmd[256];
	sprintf(szCmd, "sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;secammo;primammo;vesthelm;defuser", szFavWpn[5][0], szFavWpn[5][1], szFavWpn[5][2], szFavWpn[5][3]);
	pBuyMenuButtonBuy->SetCommand(szCmd);

	pBuyMenuButtonBuy->SetLabel(GetLangUni("#CSO_BasketBuy"));
#ifdef BUTTON_STATIC_DEFINE
	pBuyMenuButtonBuy->SetKey('b');
#endif
	memcpy(pBuyMenuButtonBuy->iTga3, g_UI_Button_Glow, sizeof(g_UI_Button_Glow));
	pBuyMenuButtonBuy->m_iSize = 14;
	pBuyMenuButtonBuy->m_Tex_Type = MGUI_TEX_TYPE_3;
	pBuyMenuButtonBuy->m_iOffset[0] = 2;

	pBuyMenuButtonClose = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
	pBuyMenuButtonClose->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
	//pBuyMenuButtonClose->SetCommand("MGUI.BuyMenu_BuyDefault");
	pBuyMenuButtonClose->SetCommand("");
	pBuyMenuButtonClose->SetLabel(GetLangUni("#CSO_BuyQuit"));
	memcpy(pBuyMenuButtonClose->iTga3, g_UI_Button, sizeof(g_UI_Button));
	pBuyMenuButtonClose->m_iSize = 14;
	pBuyMenuButtonClose->m_Tex_Type = MGUI_TEX_TYPE_3;


	pBuyMenuImageSlot = MGUI_CreateImage(pBuyMenuPanel, 0, 0, 0, 0, true, false);
	pBuyMenuImageSlot->SetTexture("gfx\\ui\\panel\\blank_slot");
	for (int i = 0; i <= 1; i++)
	{
		pBuyMenuButtonSlot[i] = MGUI_CreateButton(pBuyMenuPanel, 0, 0, 0, 0);
		pBuyMenuButtonSlot[i]->SetType(MGUI_BUTTON_TYPE_COMMAND);
	}
	pBuyMenuButtonSlot[0]->SetCommand("MGUI.TeamSuitPrevSlot");
	pBuyMenuButtonSlot[1]->SetCommand("MGUI.TeamSuitNextSlot");
	pBuyMenuButtonSlot[0]->SetImage("gfx\\ui\\page\\btn_page_prev@n", "gfx\\ui\\page\\btn_page_prev@o", "gfx\\ui\\page\\btn_page_prev@c");
	pBuyMenuButtonSlot[1]->SetImage("gfx\\ui\\page\\btn_page_next@n", "gfx\\ui\\page\\btn_page_next@o", "gfx\\ui\\page\\btn_page_next@c");

	pBuyMenuLabelSlot = MGUI_CreateLabel(pBuyMenuPanel, 0, 0, 0, 0);
	pBuyMenuLabelSlot->SetSize(14);
	pBuyMenuLabelSlot->SetColor(255, 251, 219, 255);
	pBuyMenuLabelSlot->SetLabel(GetLangUni("#CSO_ASet"));
	pBuyMenuLabelSlot->m_iAlignment = 2;

}
void BTEPanel_BuyMenu_Reset()
{
	MakeWeaponList();

	bFirst = true;
	iTDMSlot = 0;
	pBuyMenuLabelSlot->SetLabel(GetLangUni("#CSO_ASet"));
	for (int i = 0; i <= 9; i++)
	{
		pBuyMenuButton[i]->m_fNextClick = 0.0f;
	}
	for (int i = 0; i <= 1; i++)
	{
		pBuyMenuButtonPage[i]->m_fNextClick = 0.0f;
		pBuyMenuButton2[i]->m_fNextClick = 0.0f;
		pBuyMenuButtonSlot[i]->m_fNextClick = 0.0f;
	}
	for (int i = 0; i <= 4; i++)
	{
		pBuyMenuButtonFav[i]->m_fNextClick = 0.0f;
		pBuyMenuButtonSaveFav[i]->m_fNextClick = 0.0f;
	}
	pBuyMenuButtonClear->m_fNextClick = 0.0f;
	pBuyMenuButtonBuy->m_fNextClick = 0.0f;
	pBuyMenuButtonClose->m_fNextClick = 0.0f;

}
void BTEPanel_BuyMenu_UpdateItems(int iSlot, char *pItems)
{
	if (bFirst)
		BTEPanel_BuyMenu_First();

	pBuyMenuPanel->m_iClosing = 0;
	pTeamSuitPanel->m_iClosing = 1;
	g_TeamMenu.Close();
	g_NormalZombieMenu.Close();
	for (int i = 0; i <= 9; i++)
		pBuyMenuPanel_WpnInfo[i]->m_iClosing = 1;
	int iLocal = gEngfuncs.GetLocalPlayer()->index;

	float scale = (float)ScreenHeight / 480;
	pBuyMenuButton2[1]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
	if (iSlot == -1)
	{
		if (bTypeOpen)
		{
			if (!strlen(szFavWpn[5][3]))
				BTEPanel_BuyMenu_SetQuickBuy(4, "hegrenade");
			if (pBuyMenuImageQuickBuy[1][3]->m_iTexture == -1)
				pBuyMenuImageQuickBuy[1][3]->SetTexture("gfx\\vgui\\vesthelm");
		}

		pBuyMenuButton2[1]->SetCommand("MGUI.BuyMenu_AutoBuy");
		for (int i = 0; i <= 1; i++)
		{
			pBuyMenuButtonPage[i]->SetVisible(false);
		}

		for (int i = 0; i <= 9; i++)
		{
			pBuyMenuLabel[i]->SetLabel(GetLangUni(MENU[i]));
			pBuyMenuLabel[i]->SetVisible(true);

			pBuyMenuButton[i]->SetCommand(CMD[i]);
			pBuyMenuButton[i]->SetType(MGUI_BUTTON_TYPE_COMMAND);
			pBuyMenuButton[i]->SetEnabled(true);
			pBuyMenuButton[i]->SetVisible(true);
			pBuyMenuButton[i]->m_OverPanel = 0;
			pBuyMenuButton[i]->pfnPrePaint = NULL;

			pBuyMenuLabelKey[i]->SetVisible(true);
			pBuyMenuLabelKey[i]->SetColor(255, 255, 255, 255);
			pBuyMenuImage[i]->SetVisible(true);
			pBuyMenuImageWpn[i]->SetVisible(false);

			if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
				pBuyMenuLabel[i]->SetColor(192, 205, 224, 255);
			else
				pBuyMenuLabel[i]->SetColor(216, 182, 183, 255);
		}

		if (!IS_ZOMBIE_MODE)
			pBuyMenuButton[9]->SetCommand("");

		switch (g_iWeaponLimit)
		{
		case 1:
		{
			for (int i = 0; i <= 9; i++)
			{
				pBuyMenuButton[i]->SetEnabled(false);
				pBuyMenuLabel[i]->SetColor(90, 90, 90, 255);
				pBuyMenuLabelKey[i]->SetColor(150, 150, 150, 255);
			}

			if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
			{
				pBuyMenuLabel[3]->SetColor(192, 205, 224, 255);
				pBuyMenuLabel[8]->SetColor(192, 205, 224, 255);
			}
			else
			{
				pBuyMenuLabel[3]->SetColor(216, 182, 183, 255);
				pBuyMenuLabel[8]->SetColor(216, 182, 183, 255);
			}

			pBuyMenuLabelKey[3]->SetColor(255, 255, 255, 255);
			pBuyMenuLabelKey[8]->SetColor(255, 255, 255, 255);
			pBuyMenuButton[3]->SetEnabled(true);
			pBuyMenuButton[8]->SetEnabled(true);
			break;
		}
		case 2:
		{
			for (int i = 0; i <= 9; i++)
			{
				pBuyMenuButton[i]->SetEnabled(false);
				pBuyMenuLabel[i]->SetColor(90, 90, 90, 255);
				pBuyMenuLabelKey[i]->SetColor(150, 150, 150, 255);
			}

			if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
			{
				pBuyMenuLabel[0]->SetColor(192, 205, 224, 255);
				pBuyMenuLabel[8]->SetColor(192, 205, 224, 255);
			}
			else
			{
				pBuyMenuLabel[0]->SetColor(216, 182, 183, 255);
				pBuyMenuLabel[8]->SetColor(216, 182, 183, 255);
			}

			pBuyMenuLabelKey[0]->SetColor(255, 255, 255, 255);
			pBuyMenuLabelKey[8]->SetColor(255, 255, 255, 255);
			pBuyMenuButton[0]->SetEnabled(true);
			pBuyMenuButton[8]->SetEnabled(true);
			break;
		}
		case 3:
		{
			for (int i = 0; i <= 9; i++)
			{
				pBuyMenuButton[i]->SetEnabled(false);
				pBuyMenuLabel[i]->SetColor(90, 90, 90, 255);
				pBuyMenuLabelKey[i]->SetColor(150, 150, 150, 255);
			}

			if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
			{
				pBuyMenuLabel[7]->SetColor(192, 205, 224, 255);
				pBuyMenuLabel[8]->SetColor(192, 205, 224, 255);
			}
			else
			{
				pBuyMenuLabel[7]->SetColor(216, 182, 183, 255);
				pBuyMenuLabel[8]->SetColor(216, 182, 183, 255);
			}

			pBuyMenuLabelKey[7]->SetColor(255, 255, 255, 255);
			pBuyMenuLabelKey[8]->SetColor(255, 255, 255, 255);
			pBuyMenuButton[7]->SetEnabled(true);
			pBuyMenuButton[8]->SetEnabled(true);
			break;
		}
		case 4:
		{
			for (int i = 0; i <= 9; i++)
			{
				pBuyMenuButton[i]->SetEnabled(false);
				pBuyMenuLabel[i]->SetColor(90, 90, 90, 255);
				pBuyMenuLabelKey[i]->SetColor(150, 150, 150, 255);
			}

			if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
			{
				pBuyMenuLabel[8]->SetColor(192, 205, 224, 255);
			}
			else
			{
				pBuyMenuLabel[8]->SetColor(216, 182, 183, 255);
			}

			pBuyMenuLabelKey[8]->SetColor(255, 255, 255, 255);
			pBuyMenuButton[8]->SetEnabled(true);
			break;
		}
		}

		pBuyMenuButton[5]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
		pBuyMenuButton[6]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
		pBuyMenuButton[9]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);

		auto &MonthWeapon = WeaponManager()[szMonthWpn];
		if (MonthWeapon)
		{
			int iMenu = MonthWeapon.iMenu;
			if (iMenu != -1)
				pBuyMenuButton[iMenu]->pfnPrePaint = BuyMenuButton_PrePaint;
		}

		if (g_iMod != MOD_NONE && g_iMod != MOD_ZBS)
		{
			pBuyMenuButton[5]->SetEnabled(false);
			pBuyMenuButton[6]->SetEnabled(false);
			pBuyMenuLabel[5]->SetVisible(false);
			pBuyMenuLabel[6]->SetVisible(false);
		}
		else
		{
			pBuyMenuButton[5]->SetEnabled(true);
			pBuyMenuButton[6]->SetEnabled(true);
			pBuyMenuButton[7]->SetEnabled(true);
			pBuyMenuButton[9]->SetEnabled(true);
			pBuyMenuButtonClose->SetEnabled(true);
		}

		if (g_iMod == MOD_TDM || g_iMod == MOD_DM || g_iMod == MOD_ZB4)
		{
			pBuyMenuButton[9]->SetEnabled(false);
			pBuyMenuButtonClose->SetEnabled(false);
		}
	}
	else
	{
		if (!strcmp(pItems, szMonthWpn))
			pBuyMenuButton[iSlot]->pfnPrePaint = BuyMenuButton_PrePaint;
		else
			pBuyMenuButton[iSlot]->pfnPrePaint = NULL;
		if (iSlot != 9)
		{
			if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
				pBuyMenuLabel[iSlot]->SetColor(192, 205, 224, 255);
			else
				pBuyMenuLabel[iSlot]->SetColor(216, 182, 183, 255);

			if (pItems[0])
			{
				char szCmd[64];
				if (g_iMod == MOD_NONE || g_iMod == MOD_ZBS || g_iMod == MOD_GHOST)
				{
					pBuyMenuButton[iSlot]->SetType(MGUI_BUTTON_TYPE_CLOSEALL);
					if (!strcmp(pItems, "vest") || !strcmp(pItems, "vesthelm") || !strcmp(pItems, "flash") || !strcmp(pItems, "sgren") || !strcmp(pItems, "defuser") || !strcmp(pItems, "nvgs") || strstr(pItems, "ghost_"))
						sprintf(szCmd, "%s", strlwr(pItems));
					else
						sprintf(szCmd, "MGUI.BuyMenu_BuyWeapon %d %s", WeaponManager()[pItems].iSlot, strlwr(pItems));
				}
				else
				{
					pBuyMenuButton[iSlot]->SetType(MGUI_BUTTON_TYPE_COMMAND);
					pBuyMenuButton[iSlot]->SetEnabled(false);
					pBuyMenuButton[iSlot]->SetClickable(TRUE);
					//if (!strcmp(pItems, "vest") || !strcmp(pItems, "vesthelm") || !strcmp(pItems, "flash") || !strcmp(pItems, "sgren") || !strcmp(pItems, "defuser") || !strcmp(pItems, "nvgs"))
					//sprintf(szCmd, "");
					//else
					{
						if (!strcmp(pItems, "hegrenade") || !strcmp(pItems, "sgren") || !strcmp(pItems, "flash"))
							sprintf(szCmd, "MGUI.BuyMenu_BuyWeapon %d %s", 4, pItems);
						else
							sprintf(szCmd, "MGUI.BuyMenu_BuyWeapon %d %s", WeaponManager()[pItems].iSlot, strlwr(pItems));
					}

				}
				char szTga[64];
				sprintf(szTga, "gfx\\vgui\\%s", pItems);
				//pBuyMenuButton[iSlot]->pSaveData = &g_WeaponData[FindWeaponData(pItems)];
				pBuyMenuImageWpn[iSlot]->SetTexture(szTga);
				pBuyMenuImageWpn[iSlot]->SetVisible(true);
				pBuyMenuLabel[iSlot]->SetLabel(GetWeaponNameFormat(pItems));

				pBuyMenuButton[iSlot]->SetCommand(szCmd);
				pBuyMenuButton[iSlot]->SetEnabled(true);
				//if (!stricmp(pItems, "defuser") && (strncmp(gEngfuncs.pfnGetLevelName(), "de_", 3) || IS_ZOMBIE_MODE))
				//pBuyMenuButton[iSlot]->SetEnabled(false);
				pBuyMenuButton[iSlot]->SetVisible(true);
				pBuyMenuLabelKey[iSlot]->SetVisible(true);
				pBuyMenuLabel[iSlot]->SetVisible(true);
				pBuyMenuPanelImage_WpnInfo[iSlot][1]->SetTexture(szTga);
				pBuyMenuButton[iSlot]->m_OverPanel = pBuyMenuPanel_WpnInfo[iSlot];
				pBuyMenuImage[iSlot]->SetVisible(true);

				//int iWeapon = FindWeaponData(pItems);
				//CUSTOMWEAPON Item = g_WeaponData[iWeapon];
				auto &Weapon = WeaponManager()[pItems];
				if (Weapon.iSlot == 3)
				{
					pBuyMenuPanelLabel_WpnInfo[iSlot][1][0]->SetLabel(L": $0");
					pBuyMenuPanelLabel_WpnInfo[iSlot][1][0]->x = 33.75 * scale;
					for (int i = 1; i < 5; i++)
					{
						pBuyMenuPanelLabel_WpnInfo[iSlot][0][i]->SetVisible(false);
						pBuyMenuPanelLabel_WpnInfo[iSlot][1][i]->SetVisible(false);
					}
				}
				else if (!Weapon || Weapon.iSlot == 4)
				{
					pBuyMenuPanelLabel_WpnInfo[iSlot][1][0]->SetLabel(GetWeaponInfoFormat(0, pItems));
					pBuyMenuPanelLabel_WpnInfo[iSlot][0][1]->SetLabel(GetLangUni("#CSO_Desc_Lable"));
					pBuyMenuPanelLabel_WpnInfo[iSlot][1][0]->x = 33.75 * scale;
					pBuyMenuPanelLabel_WpnInfo[iSlot][1][1]->x = 33.75 * scale;
					pBuyMenuPanelLabel_WpnInfo[iSlot][1][1]->SetLabel(GetWeaponDescription(pItems));
					for (int i = 2; i < 5; i++)
					{
						pBuyMenuPanelLabel_WpnInfo[iSlot][0][i]->SetVisible(false);
						pBuyMenuPanelLabel_WpnInfo[iSlot][1][i]->SetVisible(false);
					}
				}
				else
				{
					pBuyMenuPanelLabel_WpnInfo[iSlot][0][1]->SetLabel(GetLangUni(WEAPON_INFO_LABEL[1]));
					for (int i = 0; i < 5; i++)
					{
						pBuyMenuPanelLabel_WpnInfo[iSlot][0][i]->SetVisible(true);
						pBuyMenuPanelLabel_WpnInfo[iSlot][1][i]->SetVisible(true);
						pBuyMenuPanelLabel_WpnInfo[iSlot][1][i]->x = 63.75 * scale;
						pBuyMenuPanelLabel_WpnInfo[iSlot][1][i]->SetLabel(GetWeaponInfoFormat(i, pItems));
					}
				}

				if (iPage == 0 && iType == 5 && (!strcmp(pItems, "vest") || strstr(pItems, "ghost_")))
				{
					//for (auto x : g_WeaponInPage)
					//	x = nullptr;
					//memset(iWeaponIdInList, 0, sizeof(iWeaponIdInList));
				}
				else
				{
					int iOffset = pBuyMenuPanelImage_WpnInfo[iSlot][1]->y;

					if (g_iMoney < WeaponManager()[pItems].iPrice && (g_iMod == MOD_NONE || g_iMod == MOD_ZBS || g_iMod == MOD_GHOST))
					{
						pBuyMenuLabel[iSlot]->SetColor(90, 90, 90, 255);
						pBuyMenuButton[iSlot]->SetType(MGUI_BUTTON_TYPE_COMMAND);
						pBuyMenuButton[iSlot]->SetCommand("");
					}
					else
						if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
							pBuyMenuLabel[iSlot]->SetColor(192, 205, 224, 255);
						else
							pBuyMenuLabel[iSlot]->SetColor(216, 182, 183, 255);
				}
			}
			else
			{
				pBuyMenuImageWpn[iSlot]->SetVisible(false);
				pBuyMenuImage[iSlot]->SetVisible(false);
				pBuyMenuButton[iSlot]->SetVisible(false);
				pBuyMenuButton[iSlot]->SetType(MGUI_BUTTON_TYPE_COMMAND);
				pBuyMenuLabel[iSlot]->SetVisible(false);
				pBuyMenuLabelKey[iSlot]->SetVisible(false);
				pBuyMenuButton[iSlot]->m_OverPanel = 0;
			}
		}
		else if (bFromEquipMenu == FALSE)
		{
			pBuyMenuLabel[iSlot]->SetLabel(GetLangUni("#CSO_PrevWpnBuy"));
			pBuyMenuButton[iSlot]->SetEnabled(true);
			pBuyMenuButton[iSlot]->SetCommand("MGUI.ShowBuyMenu");
			pBuyMenuButton[iSlot]->SetType(MGUI_BUTTON_TYPE_COMMAND);
		}
	}
}

void BTEPanel_BuyMenu_SetHaveNextPage(int iHavePrevPage, int iHaveNextPage)
{
	char szCmd[32];
	if (iHavePrevPage && iType == 5)
		iPage++;
	if (iHavePrevPage)
	{
		sprintf(szCmd, "MGUI.BuyMenu_SetPage %d %d", iType, iPage - 1);
		pBuyMenuButtonPage[0]->SetCommand(szCmd);
		pBuyMenuButtonPage[0]->SetEnabled(true);
	}
	else pBuyMenuButtonPage[0]->SetEnabled(false);
	if (iHaveNextPage)
	{
		sprintf(szCmd, "MGUI.BuyMenu_SetPage %d %d", iType, iPage + 1);
		pBuyMenuButtonPage[1]->SetCommand(szCmd);
		pBuyMenuButtonPage[1]->SetEnabled(true);
	}
	else
		pBuyMenuButtonPage[1]->SetEnabled(false);
	pBuyMenuButtonPage[0]->SetVisible(iHavePrevPage);
	pBuyMenuButtonPage[1]->SetVisible(iHaveNextPage);
}

void BTEPanel_BuyMenu_SetPage(int page, int type)
{
	iPage = page;
	iType = type;
}

void BTEPanel_BuyMenu_Close()
{
	pBuyMenuPanel->m_iClosing = 1;
	if (!pTeamSuitPanel || pTeamSuitPanel->m_iClosing)
		g_mgui_candraw = 0;
}
void BTEPanel_BuyMenu_First()
{
	bFirst = false;
	float scale = (float)ScreenHeight / 480;

	pBuyMenuPanel->x = (int)((ScreenWidth - 606 * scale) / 2);
	pBuyMenuPanel->prevx = pBuyMenuPanel->x;
	pBuyMenuPanel->y = (int)((ScreenHeight - 420 * scale) / 2);
	pBuyMenuPanel->prevy = pBuyMenuPanel->y;
	pBuyMenuPanel->w = 596.875 * scale;	//(int)(606 * scale);
	pBuyMenuPanel->h = 413.125 * scale;//(int)(420 * scale);

	int iLocal = gEngfuncs.GetLocalPlayer()->index;
	for (int i = 0; i <= 9; i++)
	{
		pBuyMenuPanel_WpnInfo[i]->x = pBuyMenuPanel->x + 218.75 * scale;
		pBuyMenuPanel_WpnInfo[i]->prevx = pBuyMenuPanel_WpnInfo[i]->x;
		pBuyMenuPanel_WpnInfo[i]->y = pBuyMenuPanel->y + int(60.625 * scale);
		pBuyMenuPanel_WpnInfo[i]->prevy = pBuyMenuPanel_WpnInfo[i]->y;
		pBuyMenuPanel_WpnInfo[i]->w = (int)(146.25 * scale);
		pBuyMenuPanel_WpnInfo[i]->h = (int)(140.625 * scale);

		for (int j = 0; j <= 1; j++)
		{
			pBuyMenuPanelImage_WpnInfo[i][j]->x = 0;
			pBuyMenuPanelImage_WpnInfo[i][j]->y = 0;
			pBuyMenuPanelImage_WpnInfo[i][j]->w = (int)(146.25 * scale);
			pBuyMenuPanelImage_WpnInfo[i][j]->h = (int)(50.625 * scale);
		}

		for (int k = 0; k < 5; k++)
		{
			pBuyMenuPanelLabel_WpnInfo[i][0][k]->x = 0;
			pBuyMenuPanelLabel_WpnInfo[i][0][k]->y = (16.875 + 11.25 * k) * scale;
			pBuyMenuPanelLabel_WpnInfo[i][0][k]->w = 81.875 * scale;

			pBuyMenuPanelLabel_WpnInfo[i][1][k]->x = 63.75 * scale;
			pBuyMenuPanelLabel_WpnInfo[i][1][k]->y = pBuyMenuPanelLabel_WpnInfo[i][0][k]->y + 46.875 * scale;
			pBuyMenuPanelLabel_WpnInfo[i][1][k]->w = pBuyMenuPanelLabel_WpnInfo[i][0][k]->w;

			pBuyMenuPanelLabel_WpnInfo[i][0][k]->h = 96 * scale;
			pBuyMenuPanelLabel_WpnInfo[i][1][k]->h = 0;
			pBuyMenuPanelLabel_WpnInfo[i][1][k]->m_iType = 1;
			pBuyMenuPanelLabel_WpnInfo[i][1][k]->m_iMaxHeight = 0;
		}

		pBuyMenuLabel[i]->x = int(37.5 * scale);
		pBuyMenuLabel[i]->y = int((50.625 + 25.625 * i + 6.25) * scale);
		pBuyMenuLabel[i]->h = int(8.75 * scale);
		pBuyMenuLabel[i]->SetSize(14);
		if (vPlayer[iLocal].team == 1 || IS_ZOMBIE_MODE)
			pBuyMenuLabel[i]->SetColor(192, 205, 224, 255);
		else
			pBuyMenuLabel[i]->SetColor(216, 182, 183, 255);

		pBuyMenuButton[i]->x = int(15.625 * scale);
		pBuyMenuButton[i]->y = int((50 + 25.625 * i) * scale);
		pBuyMenuButton[i]->w = int(200 * scale);
		pBuyMenuButton[i]->h = int(25 * scale);

		pBuyMenuLabelKey[i]->x = int(23.75 * scale);
		pBuyMenuLabelKey[i]->y = int((48 + 25.625 * i + 3.75) * scale);
		pBuyMenuLabelKey[i]->w = int(203 * scale);
		pBuyMenuLabelKey[i]->h = int(24.375 * scale);
		pBuyMenuLabelKey[i]->SetSize(14);

		pBuyMenuImage[i]->x = int(17.5 * scale);
		pBuyMenuImage[i]->y = int((55 + 25.625 * i) * scale);
		pBuyMenuImage[i]->h = int(17.5 * scale);
		pBuyMenuImage[i]->w = pBuyMenuImage[i]->h;
		//pBuyMenuImage[i]->SetVisible(true);

		pBuyMenuImageWpn[i]->y = pBuyMenuButton[i]->y;
		pBuyMenuImageWpn[i]->h = pBuyMenuButton[i]->h;
		pBuyMenuImageWpn[i]->w = int(pBuyMenuImageWpn[i]->h * 2.9);
		pBuyMenuImageWpn[i]->m_iTexture = -1;
		pBuyMenuImageWpn[i]->x = pBuyMenuButton[i]->x + pBuyMenuButton[i]->w - pBuyMenuImageWpn[i]->w;
	}
	for (int i = 0; i <= 1; i++)
	{
		//pBuyMenuButtonPage[i]->y = int(48 * scale) + int(255 * scale);
		//pBuyMenuButtonPage[i]->w = int(65.25 * scale);
		//pBuyMenuButtonPage[i]->h = int(20 * scale);
		pBuyMenuButtonPage[i]->w = 97;
		pBuyMenuButtonPage[i]->h = 25;
		pBuyMenuButtonPage[i]->y = pBuyMenuButton[9]->y + pBuyMenuButton[0]->h + 8;


		pBuyMenuButton2[i]->x = pBuyMenuButton[i]->x + pBuyMenuButton[i]->w + 22 * scale;
		pBuyMenuButton2[i]->y = pBuyMenuButton[8]->y - 3 * scale + i * 29 * scale;
		pBuyMenuButton2[i]->w = int(132 * scale);
		pBuyMenuButton2[i]->h = int(25 * scale);

		pBuyMenuPanelImageTimer[i]->x = 432 * scale;
		pBuyMenuPanelImageTimer[i]->y = 271 * scale + i * 21 * scale;
		pBuyMenuPanelImageTimer[i]->w = 140 * scale;
		pBuyMenuPanelImageTimer[i]->h = 16 * scale;

		pBuyMenuPanelLabelTimer[i]->x = pBuyMenuPanelImageTimer[i]->x + 3;
		pBuyMenuPanelLabelTimer[i]->y = pBuyMenuPanelImageTimer[i]->y - 2;
		pBuyMenuPanelLabelTimer[i]->w = 134 * scale;
		pBuyMenuPanelLabelTimer[i]->h = 16 * scale;

		pBuyMenuPanelLabelTimer2[i]->x = pBuyMenuPanelImageTimer[i]->x + 3;
		pBuyMenuPanelLabelTimer2[i]->y = pBuyMenuPanelImageTimer[i]->y - 2;
		pBuyMenuPanelLabelTimer2[i]->w = 134 * scale;
		pBuyMenuPanelLabelTimer2[i]->h = 16 * scale;
	}

	pBuyMenuButtonPage[1]->x = pBuyMenuButton[0]->x + pBuyMenuButton[0]->w - 141;
	pBuyMenuButtonPage[0]->x = pBuyMenuButton[0]->x + 36;

	for (int i = 0; i <= 4; i++)
	{
		pBuyMenuButtonFav[i]->w = int(175 * scale);
		pBuyMenuButtonFav[i]->h = int(37.5 * scale);
		pBuyMenuButtonFav[i]->x = 370 * scale;
		pBuyMenuButtonFav[i]->y = (50 + 40 * i) * scale;
		pBuyMenuButtonFav[i]->iTga[1] = pBuyMenuButtonFav[i]->iTga[2] = Hud().m_TGA.FindTexture("gfx\\ui\\panel\\select");

		pBuyMenuButtonSaveFav[i]->x = 555 * scale;
		pBuyMenuButtonSaveFav[i]->y = (61.875 + 40 * i) * scale;
		pBuyMenuButtonSaveFav[i]->w = 35 * scale;
		pBuyMenuButtonSaveFav[i]->h = 20 * scale;
		pBuyMenuButtonSaveFav[i]->SetVisible(true);

		pBuyMenuImageFavKey[i]->x = pBuyMenuButtonFav[i]->x + 2 * scale;
		pBuyMenuImageFavKey[i]->w = int(20 * scale);
		pBuyMenuImageFavKey[i]->h = pBuyMenuImageFavKey[i]->w;
		pBuyMenuImageFavKey[i]->y = pBuyMenuButtonFav[i]->y + pBuyMenuButtonFav[i]->h / 2 - pBuyMenuImageFavKey[i]->h / 2;

		pBuyMenuLabelFavKey[i]->x = pBuyMenuButtonFav[i]->x + pBuyMenuImageFavKey[i]->w / 2 - 1;
		pBuyMenuLabelFavKey[i]->y = pBuyMenuImageFavKey[i]->y + pBuyMenuImageFavKey[i]->h / 2 + pBuyMenuLabelFavKey[i]->m_iSize / 2;
		pBuyMenuLabelFavKey[i]->w = pBuyMenuButtonFav[i]->m_iSize;
		pBuyMenuLabelFavKey[i]->h = pBuyMenuButtonFav[i]->m_iSize;

		for (int j = 0; j <= 2; j++)
		{
			pBuyMenuImageFavWpn[i][j]->x = pBuyMenuButtonFav[i]->x + 22 * scale;
			pBuyMenuImageFavWpn[i][j]->y = pBuyMenuButtonFav[i]->y + 1 * scale;
			pBuyMenuImageFavWpn[i][j]->h = pBuyMenuButtonFav[i]->h * 0.95;
			pBuyMenuImageFavWpn[i][j]->w = pBuyMenuImageFavWpn[i][j]->h * 2.9;
		}


		pBuyMenuImageFavWpn[i][1]->h = pBuyMenuImageFavWpn[i][2]->h = pBuyMenuImageFavWpn[i][0]->h * 0.5;
		pBuyMenuImageFavWpn[i][1]->w = pBuyMenuImageFavWpn[i][2]->w = pBuyMenuImageFavWpn[i][0]->w * 0.5;

		pBuyMenuImageFavWpn[i][2]->y = pBuyMenuImageFavWpn[i][0]->y + pBuyMenuImageFavWpn[i][0]->h / 2;

		pBuyMenuImageFavWpn[i][1]->x = pBuyMenuImageFavWpn[i][2]->x = pBuyMenuImageFavWpn[i][0]->x + pBuyMenuImageFavWpn[i][0]->w - 5 * scale;
		pBuyMenuImageFavWpn[i][1]->y += 2 * scale;
	}

	for (int i = 0; i <= 1; i++)
	{
		for (int j = 0; j <= 4; j++)
		{
			pBuyMenuImageQuickBuy[i][j]->x = pBuyMenuButton[i]->x;
			pBuyMenuImageQuickBuy[i][j]->y = 330 * scale;
			pBuyMenuImageQuickBuy[i][j]->w = 146.875 * scale;
			pBuyMenuImageQuickBuy[i][j]->h = 50.625 * scale;
		}
	}

	pBuyMenuImageQuickBuy[0][1]->x += pBuyMenuImageQuickBuy[0][0]->w + 2 * scale;
	pBuyMenuImageQuickBuy[0][1]->w = 108.125 * scale;
	pBuyMenuImageQuickBuy[1][1]->x -= 14 * scale - pBuyMenuImageQuickBuy[0][1]->x;
	pBuyMenuImageQuickBuy[0][2]->x = pBuyMenuImageQuickBuy[0][1]->x + pBuyMenuImageQuickBuy[0][1]->w + 2 * scale;
	pBuyMenuImageQuickBuy[0][2]->w = 60 * scale;
	pBuyMenuImageQuickBuy[1][2]->x = pBuyMenuImageQuickBuy[0][2]->x - 42 * scale;
	pBuyMenuImageQuickBuy[0][3]->x = pBuyMenuImageQuickBuy[0][2]->x + pBuyMenuImageQuickBuy[0][2]->w + 2 * scale;
	pBuyMenuImageQuickBuy[0][3]->w = 156 * scale;
	pBuyMenuImageQuickBuy[1][3]->x = pBuyMenuImageQuickBuy[0][3]->x + 36 * scale;
	pBuyMenuImageQuickBuy[0][4]->x = pBuyMenuImageQuickBuy[0][3]->x + pBuyMenuImageQuickBuy[0][3]->w + 2 * scale;
	pBuyMenuImageQuickBuy[0][4]->w = 80 * scale;
	pBuyMenuImageQuickBuy[1][4]->x = pBuyMenuImageQuickBuy[0][4]->x - 3 * scale;
	pBuyMenuImageQuickBuy[1][4]->y += 10 * scale;

	pBuyMenuImageQuickBuy[1][4]->w = pBuyMenuImageQuickBuy[1][0]->w * 0.6;
	pBuyMenuImageQuickBuy[1][4]->h = pBuyMenuImageQuickBuy[1][0]->h * 0.6;

	pBuyMenuImageQuickBuy[1][2]->w = pBuyMenuImageQuickBuy[1][0]->w * 0.75;
	pBuyMenuImageQuickBuy[1][2]->h = pBuyMenuImageQuickBuy[1][0]->h * 0.75;
	pBuyMenuImageQuickBuy[1][2]->x += 19 * scale;
	pBuyMenuImageQuickBuy[1][2]->y += 5 * scale;

	pBuyMenuImageQuickBuy[1][1]->w = pBuyMenuImageQuickBuy[1][0]->w * 0.75;
	pBuyMenuImageQuickBuy[1][1]->h = pBuyMenuImageQuickBuy[1][0]->h * 0.75;
	pBuyMenuImageQuickBuy[1][1]->y += 5 * scale;

	pBuyMenuButtonClear->x = 345 * scale;
	pBuyMenuButtonClear->y = 386.875 * scale;
	pBuyMenuButtonClear->w = 45 * scale;
	pBuyMenuButtonClear->h = 13.125* scale;

	pBuyMenuButtonBuy->x = 411.25 * scale;
	pBuyMenuButtonBuy->y = 385 * scale;
	pBuyMenuButtonBuy->w = 87.5 * scale;
	pBuyMenuButtonBuy->h = 16.25 * scale;

	pBuyMenuButtonClose->x = 515 * scale;
	pBuyMenuButtonClose->y = 386.875 * scale;
	pBuyMenuButtonClose->w = 65 * scale;
	pBuyMenuButtonClose->h = 13.125 * scale;


	pBuyMenuLabelSlot->x = 234 * scale;
	pBuyMenuLabelSlot->y = 298 * scale;
	pBuyMenuLabelSlot->w = 140 * scale;
	pBuyMenuLabelSlot->h = 22 * scale;

	pBuyMenuImageSlot->x = 234 * scale;
	pBuyMenuImageSlot->y = 298 * scale;
	pBuyMenuImageSlot->w = 140 * scale;
	pBuyMenuImageSlot->h = 22 * scale;

	pBuyMenuButtonSlot[0]->x = 237 * scale;
	pBuyMenuButtonSlot[0]->y = 299 * scale;
	pBuyMenuButtonSlot[0]->w = 21 * scale;
	pBuyMenuButtonSlot[0]->h = 21 * scale;

	pBuyMenuButtonSlot[1]->x = 350 * scale;
	pBuyMenuButtonSlot[1]->y = 299 * scale;
	pBuyMenuButtonSlot[1]->w = 21 * scale;
	pBuyMenuButtonSlot[1]->h = 21 * scale;


	// 检查模式
	if (g_iMod)
	{
		if (g_iMod == MOD_NONE || g_iMod == MOD_ZBS || g_iMod == MOD_GHOST)
		{
			for (int i = 0; i <= 1; i++)
			{
				for (int j = 0; j <= 4; j++)
				{
					pBuyMenuImageQuickBuy[i][j]->SetVisible(false);
					pBuyMenuButtonSaveFav[j]->SetVisible(false);
				}
				pBuyMenuButtonFav[i]->m_bEnabled2 = false;
			}

			pBuyMenuPanelImageTimer[0]->SetVisible(true);
			pBuyMenuPanelLabelTimer[0]->SetVisible(true);
			pBuyMenuPanelLabelTimer2[0]->SetVisible(true);

			pBuyMenuButtonClear->SetVisible(false);
			pBuyMenuButtonBuy->SetVisible(false);
			pBuyMenuButtonClose->SetVisible(false);

			/*if (g_iMod == MOD_GHOST)
			{
			pBuyMenuPanelImageTimer[0]->SetVisible(false);
			pBuyMenuPanelLabelTimer[0]->SetVisible(false);
			pBuyMenuPanelLabelTimer2[0]->SetVisible(false);
			}*/

		}
		else
		{
			for (int i = 0; i <= 1; i++)
			{
				for (int j = 0; j <= 4; j++)
				{
					pBuyMenuImageQuickBuy[i][j]->SetVisible(true);
				}
				//pBuyMenuButtonFav[i]->m_bEnabled2 = true;
			}

			pBuyMenuPanelImageTimer[0]->SetVisible(false);
			pBuyMenuPanelLabelTimer[0]->SetVisible(false);
			pBuyMenuPanelLabelTimer2[0]->SetVisible(false);

			pBuyMenuButtonClear->SetVisible(true);
			pBuyMenuButtonBuy->SetVisible(true);
			pBuyMenuButtonClose->SetVisible(true);

		}
		if (g_iMod != MOD_TDM && g_iMod != MOD_DM && g_iMod != MOD_ZB4 && g_iMod != MOD_ZBS)
		{
			pBuyMenuPanelImageTimer[1]->SetVisible(true);
			pBuyMenuPanelLabelTimer[1]->SetVisible(true);
			pBuyMenuPanelLabelTimer2[1]->SetVisible(true);
			for (int i = 0; i <= 4; i++)
			{
				pBuyMenuButtonFav[i]->SetVisible(true);
				if (g_iMod != MOD_NONE && g_iMod != MOD_GHOST)
					pBuyMenuButtonSaveFav[i]->SetVisible(true);
				else
					pBuyMenuButtonSaveFav[i]->SetVisible(false);
				pBuyMenuImageFavKey[i]->SetVisible(true);
				pBuyMenuLabelFavKey[i]->SetVisible(true);
				for (int j = 0; j <= 2; j++)
				{
					pBuyMenuImageFavWpn[i][j]->SetVisible(true);
				}
			}
			pBuyMenuButton2[0]->SetVisible(true);
			pBuyMenuButton2[1]->SetVisible(true);
		}
		else
		{
			pBuyMenuPanelImageTimer[1]->SetVisible(false);
			pBuyMenuPanelLabelTimer[1]->SetVisible(false);
			pBuyMenuPanelLabelTimer2[1]->SetVisible(false);

			for (int i = 0; i <= 4; i++)
			{
				pBuyMenuButtonFav[i]->SetVisible(false);
				pBuyMenuButtonSaveFav[i]->SetVisible(false);
				pBuyMenuImageFavKey[i]->SetVisible(false);
				pBuyMenuLabelFavKey[i]->SetVisible(false);
				for (int j = 0; j <= 2; j++)
				{
					pBuyMenuImageFavWpn[i][j]->SetVisible(false);
				}
			}
			pBuyMenuButton2[0]->SetVisible(false);
			pBuyMenuButton2[1]->SetVisible(false);
		}
		if (g_iMod != MOD_TDM && g_iMod != MOD_DM && g_iMod != MOD_ZB4)
		{
			pBuyMenuImageSlot->SetVisible(false);
			pBuyMenuButtonSlot[0]->SetVisible(false);
			pBuyMenuButtonSlot[1]->SetVisible(false);
			pBuyMenuLabelSlot->SetVisible(false);
			pBuyMenuButtonClear->SetLabel(GetLangUni("#CSO_BasketClear"));
			pBuyMenuButtonBuy->SetLabel(GetLangUni("#CSO_BasketBuy"));
			pBuyMenuButtonClose->SetLabel(GetLangUni("#CSO_BuyQuit"));
			wcscpy(pBuyMenuPanel->m_pTitle, GetLangUni("#CSO_WeaponSelectMenu"));
		}
		else
		{
			pBuyMenuImageSlot->SetVisible(true);
			pBuyMenuButtonSlot[0]->SetVisible(true);
			pBuyMenuButtonSlot[1]->SetVisible(true);
			pBuyMenuLabelSlot->SetVisible(true);
			pBuyMenuButtonClear->SetLabel(GetLangUni("#CSO_Basket_DefaultSet"));
			pBuyMenuButtonBuy->SetLabel(GetLangUni("#CSO_Basket_SelectSet"));
			pBuyMenuButtonClose->SetLabel(GetLangUni("#CSO_Basket_Back"));
			wcscpy(pBuyMenuPanel->m_pTitle, GetLangUni("#CSO_BuySubMenu_DM"));
		}
	}
}

void BTEPanel_BuyMenu_SetTime(void)
{
	wchar_t sz[32];
	swprintf(sz, 31, L"%d", g_iBuyTime - (int)Hud().m_flTime);
	if (g_iBuyTime - (int)Hud().m_flTime < 0)
		pBuyMenuPanelLabelTimer2[1]->SetLabel(L"0");
	else
		pBuyMenuPanelLabelTimer2[1]->SetLabel(sz);
}
void BTEPanel_BuyMenu_SetMoney(void)
{
	wchar_t sz[32];
	swprintf(sz, 31, L"%d", g_iMoney);
	pBuyMenuPanelLabelTimer2[0]->SetLabel(sz);
}

void BTEPanel_BuyMenu_SetFav(int iSlot)
{
	pBuyMenuImageFavWpn[iSlot][0]->m_iTexture = pBuyMenuImageQuickBuy[1][0]->m_iTexture;
	pBuyMenuImageFavWpn[iSlot][1]->m_iTexture = pBuyMenuImageQuickBuy[1][1]->m_iTexture;
	pBuyMenuImageFavWpn[iSlot][2]->m_iTexture = pBuyMenuImageQuickBuy[1][4]->m_iTexture;
	pBuyMenuButtonFav[iSlot]->SetImage("gfx\\ui\\panel\\favoritebg", "gfx\\ui\\panel\\select", "gfx\\ui\\panel\\select");
	pBuyMenuButtonFav[iSlot]->SetEnabled(true);

	for (int i = 0; i <= 3; i++)
	{
		sprintf(szFavWpn[iSlot][i], "%s", szFavWpn[5][i]);
		char szText[16];
		sprintf(szText, "QuickBuy%d", iSlot + 1);
		WritePrivateProfileString(szText, QUICKBUY_INI[i], szFavWpn[iSlot][i], configPathFavWpn);
	}

	char szCmd[256];
	//sprintf(szCmd,"sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;secammo;primammo;vesthelm;defuser",szFavWpn[5][0],szFavWpn[5][1],szFavWpn[5][2],szFavWpn[5][3]);
	sprintf(szCmd, "MGUI.BuyMenu_BuyFav %d", iSlot);
	pBuyMenuButtonFav[iSlot]->SetCommand(szCmd);
}
int BTEPanel_BuyMenu_Read(int iSlot)
{
	int iCheck = 0;
	char name[16];
	sprintf(name, "QuickBuy%d", iSlot + 1);
	for (int i = 0; i <= 3; i++)
	{
		GetPrivateProfileStringA(name, QUICKBUY_INI[i], "", szFavWpn[iSlot][i], sizeof(szFavWpn[iSlot][i]), configPathFavWpn);
		if (strlen(szFavWpn[iSlot][i]))
		{
			char wpnname[64];
			sprintf(wpnname, "gfx\\vgui\\%s", szFavWpn[iSlot][i]);
			if (i < 3)	pBuyMenuImageFavWpn[iSlot][i]->SetTexture(wpnname);
			iCheck = 1;
		}
	}
	char szCmd[256];
	//sprintf(szCmd,"sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;sv_create_psb 10397 %s;secammo;primammo;vesthelm;defuser",szFavWpn[iSlot][0],szFavWpn[iSlot][1],szFavWpn[iSlot][2],szFavWpn[iSlot][3]);
	sprintf(szCmd, "MGUI.BuyMenu_BuyFav %d", iSlot);
	pBuyMenuButtonFav[iSlot]->SetCommand(szCmd);
	return iCheck;
}

void BTEPanel_BuyMenu_SetQuickBuy(int iSlot, char *pItems)
{
	char name[32];
	int iSlot2 = iSlot;
	if (iSlot == 4) iSlot2 = 3;
	if (iSlot == 3) iSlot2 = 5;
	iSlot2 -= 1;
	sprintf(name, "gfx\\vgui\\%s", pItems);
	pBuyMenuImageQuickBuy[1][iSlot2]->SetTexture(name);
	sprintf(szFavWpn[5][iSlot - 1], "%s", pItems);
	/*
	if (strlen(pItems))
		pBuyMenuImageQuickBuy[1][iSlot2]->SetTexture(name);
	else
		pBuyMenuImageQuickBuy[1][iSlot2]->m_iTexture = -1;

	strcpy(szFavWpn[5][iSlot - 1], pItems);

	if (!strcmp(pItems, szMonthWpn))
		pBuyMenuImageQuickBuy[1][iSlot2]->pfnPrePaint = BuyMenuQuickBuyButton_PrePaint;
	else
		pBuyMenuImageQuickBuy[1][iSlot2]->pfnPrePaint = NULL;
	*/


	char szCmd[512];
	if (!strlen(szFavWpn[5][0]))
		sprintf(szCmd, "sv_create_psb 10397 %s;", szDefaultWpn[0]);
	else
		sprintf(szCmd, "sv_create_psb 10397 %s;", szFavWpn[5][0]);
	for (int i = 1; i < 4; i++)
	{
		if (!strlen(szFavWpn[5][i]))
			strcat(szCmd, va("sv_create_psb 10397 %s;", szDefaultWpn[i]));
		else
			strcat(szCmd, va("sv_create_psb 10397 %s;", szFavWpn[5][i]));
	}

	strcat(szCmd, "secammo;primammo;vesthelm;defuser");

	pBuyMenuButtonBuy->SetCommand(szCmd);
	pBuyMenuButtonBuy->SetType(MGUI_BUTTON_TYPE_CLOSEALL);

	for (int i = 0; i <= 3; i++)
	{
		if (!strlen(szFavWpn[5][i]))
			continue;
		char szText[16];
		sprintf(szText, "QuickBuy%d", 0);
		WritePrivateProfileString(szText, QUICKBUY_INI[i], szFavWpn[5][i], configPathFavWpn);
	}

	//if (g_iMod != MOD_TDM && g_iMod != MOD_DM && g_iMod != MOD_ZB4)
	if (g_iMod == MOD_TDM || g_iMod == MOD_DM || g_iMod == MOD_ZB4)
	{
		BTEPanel_TeamSuit_Save();
	}

	/*

	else
	{
	pBuyMenuButtonBuy->SetCommand("MGUI.SaveTeamSuit");
	pBuyMenuButtonBuy->SetType(MGUI_BUTTON_TYPE_COMMAND_CLOSEPANEL);
	}*/
}