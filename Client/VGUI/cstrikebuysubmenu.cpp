#include "hud.h"
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
#include "buymouseoverpanelbutton.h"

#include <string>

using namespace vgui;

CCSBuySubMenu::CCSBuySubMenu(vgui::Panel *parent, const char *name) : CBuySubMenu(parent, name)
{
	char buffer[64];

	for (int i = 0; i < 10; i++)
	{
		sprintf(buffer, "slot%d", i);
		m_pSlotButtons[i] = new BuyMouseOverPanelButton(this, buffer, m_pPanel);
	}
	m_pPrevBtn = new vgui::Button(this, "prevBtn", "#CSO_PrevBuy");
	m_pNextBtn = new vgui::Button(this, "nextBtn", "#CSO_NextBuy");

	m_pRebuyButton = new vgui::Button(this, "RebuyButton", "#Cstrike_BuyMenuRebuy");
	m_pAutobuyButton = new vgui::Button(this, "AutobuyButton", "#CSO_AutoBuy2");

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

	for (int i = 0; i < 5; i++)
	{
		sprintf(buffer, "fav%d", i);
		m_pFavButtons[i] = new vgui::Button(this, buffer, "");
		sprintf(buffer, "fav_save%d", i);
		m_pFavSaveButtons[i] = new vgui::Button(this, buffer, "#CSO_FavSave");
	}
	m_pFavDirectBuy = new vgui::CheckButton(this, "fav_direct_buy_ckbtn", "#CSO_FavDirectBuy");

	moneyText = new vgui::Label(this, "moneyText", "");
	buytime_num = new vgui::Label(this, "buytime_num", "");

	m_pUpgradeTitle = new vgui::Label(this, "UpgradeTitle", "");
	m_pOppZombiUpgradeTitle = new vgui::Label(this, "OppZombiUpgradeTitle", "");

	m_pSetSelBg = new vgui::ImagePanel(this, "SetSelBg");
	m_pSetLabel = new vgui::Label(this, "SetLabel", "X Set");
	m_pPrevSetBtn = new vgui::Button(this, "PrevSetBtn", "");
	m_pNextSetBtn = new vgui::Button(this, "NextSetBtn", "");

	m_pEditDescLabel_DM = new vgui::Label(this, "EditDescLabel_DM", "#CSO_BuySubMenu_EditDesc");
	m_pEditDescBg = new vgui::ImagePanel(this, "EditDescBg");
	m_pEquipSample = new vgui::ImagePanel(this, "EquipSample");
	
}

void CCSBuySubMenu::OnCommand(const char *command)
{
	if (!Q_strcmp(command, "buy_unavailable"))
	{
		BaseClass::OnCommand("vguicancel");
		return;
	}

	BaseClass::OnCommand(command);
}

void CCSBuySubMenu::OnSizeChanged(int newWide, int newTall)
{
	BaseClass::OnSizeChanged(newWide, newTall);
}

void CCSBuySubMenu::OnDisplay(void)
{
	BaseClass::OnDisplay();
}

void CCSBuySubMenu::PerformLayout(void)
{
	BaseClass::PerformLayout();

}

MouseOverPanelButton *CCSBuySubMenu::CreateNewMouseOverPanelButton(EditablePanel *panel)
{
	return new BuyMouseOverPanelButton(this, NULL, panel);
}

CBuySubMenu *CCSBuySubMenu::CreateNewSubMenu(const char *name)
{
	return new CCSBuySubMenu(this, name);
}

void CCSBuySubMenu::SetupNextSubPanel(const char *fileName)
{
	return BaseClass::SetupNextSubPanel(fileName);
}

void CCSBuySubMenu::UpdateLoadout(void)
{
	
}

void CCSBuySubMenu::LoadControlSettings(const char *dialogResourceName, const char *pathID, KeyValues *pPreloadedKeyValues)
{
	BaseClass::LoadControlSettings(dialogResourceName, pathID, pPreloadedKeyValues);

	// 0->exit
	m_pSlotButtons[9]->SetText("#Cstrike_Cancel");
	m_pSlotButtons[9]->SetCommand("vguicancel");

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

	// hide money & buy time
	moneyText->SetVisible(false);
	buytime_num->SetVisible(false);
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
	moneyText->SetVisible(false);
	buytime_num->SetVisible(false);

	// hide rebuy & autobuy
	m_pRebuyButton->SetVisible(false);
	m_pAutobuyButton->SetVisible(false);
}