#ifndef CSBUYSUBMENU_H
#define CSBUYSUBMENU_H
#ifdef _WIN32
#pragma once
#endif

#include "game_controls\buysubmenu.h"

using namespace vgui;

class CCSBuySubMenu : public CBuySubMenu
{
private:
	DECLARE_CLASS_SIMPLE(CCSBuySubMenu, CBuySubMenu);

public:
	CCSBuySubMenu(vgui::Panel *parent, const char *name = "BuySubMenu");

protected:
	virtual void OnCommand(const char *command);
	virtual void PerformLayout(void);
	virtual void OnSizeChanged(int newWide, int newTall);
	virtual void OnDisplay(void);

public:
	void UpdateLoadout(void);

protected:
	virtual MouseOverPanelButton *CreateNewMouseOverPanelButton(EditablePanel *panel) override;
	virtual CBuySubMenu *CreateNewSubMenu(const char *name = "BuySubMenu") override;

public:
	virtual void SetupNextSubPanel(const char *fileName);

protected:
	// Left Column
	BuyMouseOverPanelButton * m_pSlotButtons[10]; // slot0 ... slot10
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
	vgui::ImagePanel *pammoBG;
	vgui::ImagePanel *swpnBG;
	vgui::ImagePanel *sammoBG;
	vgui::ImagePanel *hgrenBG;
	vgui::ImagePanel *sgrenBG;
	vgui::ImagePanel *fgrenBG;
	vgui::ImagePanel *fgren2BG;
	vgui::ImagePanel *dfBG;
	vgui::ImagePanel *nvBG;
	vgui::ImagePanel *kevBG;

	// Right Fav List
	vgui::Button *m_pFavButtons[5]; // fav0 ... fav4
	vgui::Button *m_pFavSaveButtons[5]; // fav_save0 .. fav_save4
	vgui::CheckButton *m_pFavDirectBuy; // fav_direct_buy_ckbtn

	vgui::Label *moneyText;
	vgui::Label *buytime_num;

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
	typedef CBuySubMenu BaseClass;

public:
	CCSBuySubMenu_DefaultMode(vgui::Panel *parent, const char *name = "BuySubMenu") : CCSBuySubMenu(parent, name){}
	virtual void LoadControlSettings(const char *dialogResourceName, const char *pathID = NULL, KeyValues *pPreloadedKeyValues = NULL) override;
};

#endif
