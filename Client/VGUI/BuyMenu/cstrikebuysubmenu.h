#ifndef CSBUYSUBMENU_H
#define CSBUYSUBMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "game_controls\buysubmenu.h"
#include "buymouseoverpanelbutton.h"
#include "BuyPresetButton.h"
#include "cso_controls/NewTabButton.h"
#include "WeaponManager.h"

#include "util/iniParser.h"

using namespace vgui;

class CSBuyMouseOverPanelButton : public BuyMouseOverPanelButton
{
private:
	typedef BuyMouseOverPanelButton BaseClass;
public:
	CSBuyMouseOverPanelButton(vgui::Panel *parent, const char *panelName, vgui::EditablePanel *panel) : BaseClass(parent, panelName, panel)
	{
		
	}
	void UpdateWeapon(const char *weapon);
	virtual void Paint() override;

	using WeaponBuyTeam = CWeaponManager::WeaponBuyTeam;
	void SetTeam(WeaponBuyTeam team) 
	{ 
		m_iTeam = team; 
	}
	WeaponBuyTeam m_iTeam;
};

class CCSBuySubMenu : public CBuySubMenu
{
public:
	using WeaponBuyTeam = CWeaponManager::WeaponBuyTeam;
	using WeaponBuyMenuType = CWeaponManager::WeaponBuyMenuType;
private:
	DECLARE_CLASS_SIMPLE(CCSBuySubMenu, CBuySubMenu);

public:
	CCSBuySubMenu(vgui::Panel *parent, const char *name = "BuySubMenu");

protected:
	virtual CSBuyMouseOverPanelButton *CreateNewMouseOverPanelButton(EditablePanel *panel) override;
	virtual CCSBuySubMenu *CreateNewSubMenu(const char *name = "BuySubMenu") override;

public:
	virtual void LoadControlSettings(const char *dialogResourceName, const char *pathID = NULL, KeyValues *pPreloadedKeyValues = NULL) override;
	virtual void OnCommand(const char *command) override;
	virtual void PerformLayout() override;
	virtual void OnThink() override;

	virtual void SetupItems(CWeaponManager::WeaponBuyMenuType type);
	virtual void SetupPage(size_t page);
	virtual void SetTeam(WeaponBuyTeam team);
	virtual void SelectWeapon(const char *weapon);
	virtual void ReadFavoriteWeapons();

protected:
	struct ItemInfo
	{
		std::string name;
		std::string command;
	};
	std::vector<ItemInfo> m_BuyItemList;
	ItemInfo m_FavoriteItems[5][4];
	ItemInfo m_RebuyItems[4];
	CIniParser m_iniFavorite;
	size_t m_iCurrentPage;
	WeaponBuyTeam m_iTeam;

protected:
	vgui::Label *m_pTitleLabel;
	// Left Column
	NewTabButton *m_pShowCTWeapon;
	NewTabButton *m_pShowTERWeapon;
	CSBuyMouseOverPanelButton * m_pSlotButtons[10]; // slot0 ... slot10
	vgui::Button *m_pPrevBtn; // prevpage
	vgui::Button *m_pNextBtn; // nextpage

	// Middle Column
	vgui::Button *m_pRebuyButton; // rebuy_in
	vgui::Button *m_pAutobuyButton; // autobuy_in

	// Lower Right Corner
	vgui::Button *m_pBasketClear; // basketclear
	vgui::Button *m_pBasketBuy;	// basketbuy
	vgui::Button *m_pQuitButton;  // vguicancel

								  // Lower Weapon Slot
	vgui::ImagePanel *pwpnBG;
	vgui::ImagePanel *swpnBG;
	vgui::ImagePanel *hgrenBG;
	vgui::ImagePanel *sgrenBG;
	vgui::ImagePanel *fgrenBG;
	vgui::ImagePanel *fgren2BG;
	vgui::ImagePanel *dfBG;
	vgui::ImagePanel *nvBG;
	vgui::ImagePanel *kevBG;

	vgui::ImagePanel *pammoBG;
	vgui::ImagePanel *sammoBG;

	vgui::ImagePanel *primaryBG;
	vgui::ImagePanel *secondaryBG;
	vgui::ImagePanel *knifeBG;
	vgui::ImagePanel *grenadeBG;
	vgui::ImagePanel *equipBG;

	// Right Fav List
	BuyPresetButton *m_pFavButtons[5]; // fav0 ... fav4
	vgui::Button *m_pFavSaveButtons[5]; // fav_save0 .. fav_save4
	vgui::CheckButton *m_pFavDirectBuy; // fav_direct_buy_ckbtn

	vgui::Label *account_num;
	vgui::Label *buytime_num;
	vgui::Label *moneyText;

	vgui::TextEntry *freezetime;
	vgui::TextEntry *account;
	vgui::TextEntry *buytime;
	vgui::TextEntry *moneyBack;

	// ZBS
	vgui::Label *m_pUpgradeTitle;
	vgui::Label *m_pOppZombiUpgradeTitle;

	// DM
	vgui::ImagePanel *m_pSetSelBg;
	vgui::Label *m_pSetLabel;
	vgui::Button *m_pPrevSetBtn;
	vgui::Button *m_pNextSetBtn;

	vgui::Label *m_pEditDescLabel_DM;
	vgui::ImagePanel *m_pEditDescBg;
	vgui::ImagePanel *m_pEquipSample;
};

class CCSBuySubMenu_DefaultMode : public CCSBuySubMenu
{
private:
	typedef CCSBuySubMenu BaseClass;

public:
	CCSBuySubMenu_DefaultMode(vgui::Panel *parent, const char *name = "BuySubMenu") : CCSBuySubMenu(parent, name){}
	virtual void LoadControlSettings(const char *dialogResourceName, const char *pathID = NULL, KeyValues *pPreloadedKeyValues = NULL) override;
	virtual void SelectWeapon(const char *weapon) override;
};

class CCSBuySubMenu_ZombieMod : public CCSBuySubMenu
{
private:
	typedef CCSBuySubMenu BaseClass;

public:
	CCSBuySubMenu_ZombieMod(vgui::Panel *parent, const char *name = "BuySubMenu") : CCSBuySubMenu(parent, name) {}
	virtual void LoadControlSettings(const char *dialogResourceName, const char *pathID = NULL, KeyValues *pPreloadedKeyValues = NULL) override;
};

class CCSBuySubMenu_DeathMatch : public CCSBuySubMenu
{
private:
	typedef CCSBuySubMenu BaseClass;

public:
	CCSBuySubMenu_DeathMatch(vgui::Panel *parent, const char *name = "BuySubMenu") : CCSBuySubMenu(parent, name) {}
	virtual void LoadControlSettings(const char *dialogResourceName, const char *pathID = NULL, KeyValues *pPreloadedKeyValues = NULL) override;
};

#endif
