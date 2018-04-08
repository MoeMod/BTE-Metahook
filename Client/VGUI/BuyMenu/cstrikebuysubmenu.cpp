#include "hud.h"
#include "message.h"
#include "ViewPort.h"
#include "cdll_dll.h"
#include "cl_util.h"
#include "cstrikebuymenu.h"
#include "cstrikebuysubmenu.h"
#include "shared_util.h"
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include "vgui_controls/RichText.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/TextEntry.h"
#include "buymouseoverpanelbutton.h"
#include "cso_controls/ButtonGlow.h"
#include "cso_controls/DarkTextEntry.h"


#include "WeaponManager.h"

#include <string>

using namespace vgui;

static const Color COL_NONE = { 255, 255, 255, 255 };
static const Color COL_CT = { 192, 205, 224, 255 };
static const Color COL_TR = { 216, 182, 183, 255 };

static const char *EQUIPMENT_BUYLIST[] = { "vest","vesthelm","flash","hegrenade","sgren","defuser","nvgs" };
static const char *EQUIPMENT_BUYLIST_CMD[] = { "vest","vesthelm","flash","hegrenade","sgren","defuser","nvgs" };

void CSBuyMouseOverPanelButton::UpdateWeapon(const char *weapon)
{
	m_pPanel->LoadControlSettings("classes/default.res", "GAME");
	Panel *infoPanel = m_pPanel->FindChildByName("infolabel");
	if (infoPanel)
	{
		Label *infoLabel = dynamic_cast<Label *>(infoPanel);
		if (infoLabel)
		{
			infoLabel->SetText(GetWeaponDescription(weapon));
		}
	}
	Panel *imagePanel = m_pPanel->FindChildByName("classimage");
	if (infoPanel)
	{
		ImagePanel *image = dynamic_cast<ImagePanel *>(infoPanel);
		if (image)
		{
			char szBuffer[64];
			sprintf(szBuffer, "gfx\\vgui\\%s", weapon);
			image->SetImage(szBuffer);
		}
	}
}

void CSBuyMouseOverPanelButton::Paint()
{
	Color col = COL_NONE;
	if (m_iTeam == WeaponBuyTeam::TR)
	{
		col = COL_TR;
	}
	else if (m_iTeam == WeaponBuyTeam::CT)
	{
		col = COL_CT;
	}
	SetFgColor(col);
	BaseClass::Paint();
}

CCSBuySubMenu::CCSBuySubMenu(vgui::Panel *parent, const char *name) : CBuySubMenu(parent, name)
{
	m_iniFavorite.OpenFile("quickbuy.ini");
	ReadFavoriteWeapons();

	m_pTitleLabel = new vgui::Label(this, "CaptionLabel", "#CSO_WeaponSelectMenu");

	char buffer[64];

	m_pShowCTWeapon = new NewTabButton(this, "ShowCTWeapon", "#CSO_BuyShowCT");
	m_pShowTERWeapon = new NewTabButton(this, "ShowTERWeapon", "#CSO_BuyShowTER");
	m_pShowCTWeapon->SetTextColor(COL_CT);
	m_pShowTERWeapon->SetTextColor(COL_TR);

	for (int i = 0; i < 10; i++)
	{
		sprintf(buffer, "slot%d", i);
		m_pSlotButtons[i] = new CSBuyMouseOverPanelButton(this, buffer, m_pPanel);
		m_pSlotButtons[i]->GetClassPanel()->SetName("ItemInfo");
	}
	m_pPrevBtn = new vgui::Button(this, "prevBtn", "#CSO_PrevBuy");
	m_pNextBtn = new vgui::Button(this, "nextBtn", "#CSO_NextBuy");

	m_pRebuyButton = new ButtonGlow(this, "RebuyButton", "#Cstrike_BuyMenuRebuy");
	m_pAutobuyButton = new ButtonGlow(this, "AutobuyButton", "#CSO_AutoBuy2");

	m_pBasketClear = new vgui::Button(this, "BasketClear", "#CSO_BasketClear");
	m_pBasketBuy = new vgui::Button(this, "BasketBuy", "#CSO_BasketBuy");
	m_pQuitButton = new vgui::Button(this, "QuitButton", "#CSO_BuyQuit");

	pwpnBG = new vgui::ImagePanel(this, "pwpnBG");
	pammoBG = new vgui::ImagePanel(this, "pammoBG");
	swpnBG = new vgui::ImagePanel(this, "swpnBG");
	sammoBG = new vgui::ImagePanel(this, "sammoBG");
	hgrenBG = new vgui::ImagePanel(this, "hgrenBG");
	sgrenBG = new vgui::ImagePanel(this, "sgrenBG");
	fgrenBG = new vgui::ImagePanel(this, "fgrenBG");
	fgren2BG = new vgui::ImagePanel(this, "fgren2BG");
	dfBG = new vgui::ImagePanel(this, "dfBG");
	nvBG = new vgui::ImagePanel(this, "nvBG");
	kevBG = new vgui::ImagePanel(this, "kevBG");

	primaryBG = new vgui::ImagePanel(this, "primaryBG");
	secondaryBG = new vgui::ImagePanel(this, "secondaryBG");
	knifeBG = new vgui::ImagePanel(this, "knifeBG");
	grenadeBG = new vgui::ImagePanel(this, "grenadeBG");
	equipBG = new vgui::ImagePanel(this, "equipBG");

	for (int i = 0; i < 5; i++)
	{
		sprintf(buffer, "fav%d", i);
		m_pFavButtons[i] = new BuyPresetButton(this, buffer);
		sprintf(buffer, "fav_save%d", i);
		m_pFavSaveButtons[i] = new vgui::Button(this, buffer, "#CSO_FavSave");
	}
	m_pFavDirectBuy = new vgui::CheckButton(this, "fav_direct_buy_ckbtn", "#CSO_FavDirectBuy");

	account_num = new vgui::Label(this, "account_num", "");
	buytime_num = new vgui::Label(this, "buytime_num", "");
	moneyText = new vgui::Label(this, "moneyText", "");

	account = new DarkTextEntry(this, "account");
	buytime = new DarkTextEntry(this, "buytime");
	moneyBack = new DarkTextEntry(this, "moneyBack");

	m_pUpgradeTitle = new vgui::Label(this, "UpgradeTitle", "");
	m_pOppZombiUpgradeTitle = new vgui::Label(this, "OppZombiUpgradeTitle", "");

	m_pSetSelBg = new vgui::ImagePanel(this, "SetSelBg");
	m_pSetLabel = new vgui::Label(this, "SetLabel", "X Set");
	m_pPrevSetBtn = new vgui::Button(this, "PrevSetBtn", "");
	m_pNextSetBtn = new vgui::Button(this, "NextSetBtn", "");

	m_pEditDescLabel_DM = new vgui::Label(this, "EditDescLabel_DM", "#CSO_BuySubMenu_EditDesc");
	m_pEditDescBg = new vgui::ImagePanel(this, "EditDescBg");
	m_pEquipSample = new vgui::ImagePanel(this, "EquipSample");

	SetupItems(WeaponBuyMenuType::NONE);
}

void CCSBuySubMenu::OnCommand(const char *command)
{
	if (!strncmp(command, "VGUI_BuyMenu_Show", 17))
	{
		WeaponBuyMenuType iType;
		if (command[17] == '\0')
			iType = WeaponBuyMenuType::NONE;
		else
			sscanf(command + 17, "%d", &iType); //"Pistol 1"

		SetupItems(iType);
		
		GotoNextSubPanel();
		return;
	}
	else if (!strncmp(command, "VGUI_BuyMenu_SetTeam", 20))
	{
		WeaponBuyTeam team;
		if (command[20] == '\0')
			team = WeaponBuyTeam::ALL;
		else
			sscanf(command + 20, "%d", &team); //"Pistol 1"

		SetTeam(team);

		GotoNextSubPanel();
		return;
	}
	else if (!strncmp(command, "VGUI_BuyMenu_BuyWeapon", 22))
	{
		SelectWeapon(command + 22);
		return;
	}
	else if (!Q_strcmp(command, "showctwpn"))
	{
		SetTeam(WeaponBuyTeam::CT);
		SetupItems(WeaponBuyMenuType::NONE);
		return;
	}
	else if (!Q_strcmp(command, "showterwpn"))
	{
		SetTeam(WeaponBuyTeam::TR);
		SetupItems(WeaponBuyMenuType::NONE);
		return;
	}
	else if (!Q_strcmp(command, "buy_unavailable"))
	{
		BaseClass::OnCommand("vguicancel");
		return;
	}
	else if (!Q_strcmp(command, "autobuy_in"))
	{
		if (m_iTeam == WeaponBuyTeam::CT)
			gEngfuncs.pfnClientCmd("sv_create_psb 10397 m4a1;sv_create_psb 10397 usp;sv_create_psb 10397 hegrenade;sv_create_psb 10397 knife;secammo;primammo;vesthelm;defuser");
		else
			gEngfuncs.pfnClientCmd("sv_create_psb 10397 ak47;sv_create_psb 10397 glock18;sv_create_psb 10397 hegrenade;sv_create_psb 10397 knife;secammo;primammo;vesthelm");
		BaseClass::OnCommand("vguicancel");
		return;
	}
	else if (!Q_strcmp(command, "rebuy_in"))
	{
		gEngfuncs.pfnClientCmd("bte_wpn_rebuy;rebuy;secammo;primammo");
		BaseClass::OnCommand("vguicancel");
		return;
	}
	else if (!Q_strcmp(command, "prevpage"))
	{
		SetupPage(m_iCurrentPage - 1);
		return;
	}
	else if (!Q_strcmp(command, "nextpage"))
	{
		SetupPage(m_iCurrentPage + 1);
		return;
	}

	BaseClass::OnCommand(command);
}

void CCSBuySubMenu::PerformLayout()
{
	BaseClass::PerformLayout();

	int  w, h;
	GetSize(w, h);
	int w2, h2;
	m_pTitleLabel->GetSize(w2, h2);
	m_pTitleLabel->SetPos(w / 2 - w2 / 2, 12);

	for (int i = 0; i < 5; ++i)
	{
		m_pFavButtons[i]->SetPrimaryWeapon(m_FavoriteItems[i][0].name.c_str());
		m_pFavButtons[i]->SetSecondaryWeapon(m_FavoriteItems[i][1].name.c_str());
		m_pFavButtons[i]->SetKnifeWeapon(m_FavoriteItems[i][2].name.c_str());
	}
	
}

void CCSBuySubMenu::OnThink()
{
	BaseClass::OnThink();

	int time = g_iBuyTime - (int)Hud().m_flTime;
	if (time < 0)
		time = 0;
	account_num->SetText(std::to_wstring(g_iMoney).c_str());
	buytime_num->SetText(std::to_wstring(time).c_str());
}

void CCSBuySubMenu::SetupItems(WeaponBuyMenuType type)
{
	m_BuyItemList.clear();
	if (type == WeaponBuyMenuType::NONE)
	{
		static const char szTitles[10][32] = { "#CSO_Pistols", "#CSO_Shotguns", "#CSO_SubMachineGuns", "#CSO_Rifles",
			"#CSO_MachineGuns", "#CSO_Prim_Ammo", "#CSO_Sec_Ammo", "#CSO_Equipment",
			"#CSO_ProximityWpn", "#CSO_EndWpnBuy" };
		static const char szCommands[10][32] = { "VGUI_BuyMenu_Show 0", "VGUI_BuyMenu_Show 1", "VGUI_BuyMenu_Show 2", "VGUI_BuyMenu_Show 3",
			"VGUI_BuyMenu_Show 4", "primammo", "secammo", "VGUI_BuyMenu_Show 5",
			"VGUI_BuyMenu_Show 6", "vguicancel" };

		for (int i = 0; i < 10; ++i)
		{
			m_pSlotButtons[i]->SetText(szTitles[i]);
			m_pSlotButtons[i]->SetCommand(szCommands[i]);
			m_pSlotButtons[i]->SetHotkey('0' + i + 1);
			m_pSlotButtons[i]->SetVisible(true);
		}
		m_pSlotButtons[9]->SetHotkey('0');
		m_pPrevBtn->SetVisible(false);
		m_pNextBtn->SetVisible(false);
		return;
	}
	else
	{
		WeaponBuyTeam team = g_iTeam == 1 ? WeaponBuyTeam::CT : WeaponBuyTeam::TR;

		if (type == WeaponBuyMenuType::EQUIP)
		{
			for (int i = 0; i < 7; ++i)
			{
				m_BuyItemList.push_back({ EQUIPMENT_BUYLIST[i], EQUIPMENT_BUYLIST_CMD[i] });
			}
		}

		for (auto &x : WeaponManager())
		{
			if (type != x.iMenu)
				continue;
			if (x.iTeam != WeaponBuyTeam::ALL && team != x.iTeam)
				continue;
			const char *name = x.szName;

			if (type == WeaponBuyMenuType::EQUIP)
			{
				if (!strcmp(name, "flashbang") || !strcmp(name, "hegrenade") || !strcmp(name, "smokegrenade"))
					continue;
			}

			m_BuyItemList.push_back(ItemInfo{ name, (std::string("VGUI_BuyMenu_BuyWeapon ") += name) });
		}
		SetupPage(0);
	}
}

void CCSBuySubMenu::SetupPage(size_t iPage)
{
	int totalpages = m_BuyItemList.size() / 9 + 1;
	if (iPage <= 0) iPage = 0;
	else if (iPage >= totalpages)
		iPage = totalpages - 1;

	m_iCurrentPage = iPage;
	
	// page buttons
	m_pPrevBtn->SetVisible(iPage != 0);
	m_pNextBtn->SetVisible(m_iCurrentPage != totalpages - 1);
	
	for (int i = 0; i < 9; ++i)
	{
		int iElement = m_iCurrentPage * 9 + i;
		if (iElement >= m_BuyItemList.size())
		{
			m_pSlotButtons[i]->SetText("");
			m_pSlotButtons[i]->SetCommand("");
			m_pSlotButtons[i]->SetVisible(false);
			m_pSlotButtons[i]->SetHotkey('\0');
		}
		else
		{
			const char *weapon = m_BuyItemList[iElement].name.c_str();
			const char *cmd = m_BuyItemList[iElement].command.c_str();
			m_pSlotButtons[i]->SetText(GetWeaponNameFormat(weapon));
			m_pSlotButtons[i]->SetCommand(cmd);
			m_pSlotButtons[i]->SetVisible(true);
			m_pSlotButtons[i]->SetHotkey('0' + i + 1);
			m_pSlotButtons[i]->UpdateWeapon(weapon);
			
		}
	}
	
	m_pSlotButtons[9]->SetText("#CSO_EndWpnBuy");
	m_pSlotButtons[9]->SetCommand("vguicancel");
	m_pSlotButtons[9]->SetHotkey('0');

}

void CCSBuySubMenu::SetTeam(WeaponBuyTeam team)
{
	m_iTeam = team;

	for (int i = 0; i < 10; ++i)
	{
		m_pSlotButtons[i]->SetTeam(team);
	}
	
	m_pShowCTWeapon->SetActive(false);
	m_pShowTERWeapon->SetActive(false);
	if (team== WeaponBuyTeam::CT)
		m_pShowCTWeapon->SetActive(true);
	if (team == WeaponBuyTeam::TR)
		m_pShowTERWeapon->SetActive(true);

	Color col = { 255,255,255,255 };
	Color bg = { 255,255,255,255 };
	if (m_iTeam == WeaponBuyTeam::TR)
	{
		col = COL_TR;
	}
	else if (m_iTeam == WeaponBuyTeam::CT)
	{
		col = COL_CT;
	}

}

void CCSBuySubMenu::SelectWeapon(const char *weapon)
{
	char szBuffer[64];

	sprintf(szBuffer, "sv_create_psb 10397 %s", weapon);
	gEngfuncs.pfnClientCmd(szBuffer);
}

void CCSBuySubMenu::ReadFavoriteWeapons()
{
	for(int i:xrange(1, 6))
	{
		std::string app("QuickBuy");
		app += std::to_string(i);

		auto &keyvalue = m_iniFavorite[app];

		--i;
		m_FavoriteItems[i][0].name =  keyvalue["Primary"] ;
		m_FavoriteItems[i][1].name =  keyvalue["Secondary"];
		m_FavoriteItems[i][2].name =  keyvalue["Knife"];
		m_FavoriteItems[i][3].name =  keyvalue["Grenade"];
		for (auto &item : m_FavoriteItems[i])
		{
			item.command = (std::string("VGUI_BuyMenu_BuyWeapon ") += item.name);
		}
	}

	auto &keyvalue = m_iniFavorite[std::string("QuickBuy0")];
	m_RebuyItems[0].name = keyvalue["Primary"];
	m_RebuyItems[1].name = keyvalue["Secondary"];
	m_RebuyItems[2].name = keyvalue["Knife"];
	m_RebuyItems[3].name = keyvalue["Grenade"];
	for (auto &item : m_RebuyItems)
	{
		item.command = (std::string("VGUI_BuyMenu_BuyWeapon ") += item.name);
	}
}

CSBuyMouseOverPanelButton *CCSBuySubMenu::CreateNewMouseOverPanelButton(EditablePanel *panel)
{
	return new CSBuyMouseOverPanelButton(this, NULL, panel);
}

CCSBuySubMenu *CCSBuySubMenu::CreateNewSubMenu(const char *name)
{
	return new CCSBuySubMenu(this, name);
}

void CCSBuySubMenu::LoadControlSettings(const char *dialogResourceName, const char *pathID, KeyValues *pPreloadedKeyValues)
{
	BaseClass::LoadControlSettings(dialogResourceName, pathID, pPreloadedKeyValues);

	// 0->exit
	m_pSlotButtons[9]->SetText("#Cstrike_Cancel");
	m_pSlotButtons[9]->SetCommand("vguicancel");

	account->SetText("#Cstrike_Current_Money");
	buytime->SetText("#CSO_BuyTime");
	for (vgui::TextEntry *p : { account , buytime })
	{
		p->SetMouseInputEnabled(false);
	}

	m_pTitleLabel->SetFont(scheme()->GetIScheme(m_pTitleLabel->GetScheme())->GetFont("Title", IsProportional()));
	m_pTitleLabel->SizeToContents();

	const wchar key[5] = { L'S',L'D', L'F', L'G', L'H' };
	for (int i = 0; i < 5; ++i)
	{
		m_pFavButtons[i]->SetHotkey(key[i]);
	}
}

void CCSBuySubMenu_DefaultMode::LoadControlSettings(const char *dialogResourceName, const char *pathID, KeyValues *pPreloadedKeyValues)
{
	BaseClass::LoadControlSettings(dialogResourceName, pathID, pPreloadedKeyValues);

	// hide zbs
	m_pUpgradeTitle->SetVisible(false);
	m_pOppZombiUpgradeTitle->SetVisible(false);

	// hide dm set
	m_pSetSelBg->SetVisible(false);
	m_pSetLabel->SetVisible(false);
	m_pPrevSetBtn->SetVisible(false);
	m_pNextSetBtn->SetVisible(false);
	m_pEditDescLabel_DM->SetVisible(false);
	m_pEditDescBg->SetVisible(false);
	m_pEquipSample->SetVisible(false);

	// Lower Right Corner
	m_pBasketClear->SetVisible(false);
	m_pBasketBuy->SetVisible(false);
	m_pQuitButton->SetVisible(false);

	// hide set
	for (vgui::Panel * pPanel : { primaryBG, secondaryBG, knifeBG, grenadeBG, equipBG })
	{
		pPanel->SetVisible(false);
	}

	m_pShowCTWeapon->SetVisible(false);
	m_pShowTERWeapon->SetVisible(false);

	moneyText->SetVisible(false);
	moneyBack->SetVisible(false);
}

void CCSBuySubMenu_ZombieMod::LoadControlSettings(const char *dialogResourceName, const char *pathID, KeyValues *pPreloadedKeyValues)
{
	BaseClass::LoadControlSettings(dialogResourceName, pathID, pPreloadedKeyValues);

	// hide zbs
	m_pUpgradeTitle->SetVisible(false);
	m_pOppZombiUpgradeTitle->SetVisible(false);

	// hide dm set
	m_pSetSelBg->SetVisible(false);
	m_pSetLabel->SetVisible(false);
	m_pPrevSetBtn->SetVisible(false);
	m_pNextSetBtn->SetVisible(false);
	m_pEditDescLabel_DM->SetVisible(false);
	m_pEditDescBg->SetVisible(false);
	m_pEquipSample->SetVisible(false);

	// hide money
	for(auto pPanel : { account_num ,moneyText })
		pPanel->SetVisible(false);
	for (auto pPanel : { freezetime, account, moneyBack })
		pPanel->SetVisible(false);
}

void CCSBuySubMenu_DeathMatch::LoadControlSettings(const char *dialogResourceName, const char *pathID, KeyValues *pPreloadedKeyValues)
{
	BaseClass::LoadControlSettings(dialogResourceName, pathID, pPreloadedKeyValues);

	// hide zbs
	m_pUpgradeTitle->SetVisible(false);
	m_pOppZombiUpgradeTitle->SetVisible(false);

	// Right Fav List Hide
	for (int i = 0; i < 5; i++)
	{
		m_pFavButtons[i]->SetVisible(false);
		m_pFavSaveButtons[i]->SetVisible(false);
	}
	m_pFavDirectBuy->SetVisible(false);

	// hide money & buy time
	for (auto pPanel : { account_num ,buytime_num ,moneyText })
		pPanel->SetVisible(false);
	for (auto pPanel : { freezetime, account, buytime, moneyBack })
		pPanel->SetVisible(false);

	// hide rebuy & autobuy
	m_pRebuyButton->SetVisible(false);
	m_pAutobuyButton->SetVisible(false);
}

void CCSBuySubMenu_DefaultMode::SelectWeapon(const char *weapon)
{
	
	BaseClass::SelectWeapon(weapon);
	BaseClass::OnCommand("vguicancel");
}