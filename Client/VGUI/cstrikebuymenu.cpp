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
#include "buymouseoverpanelbutton.h"

#include <string>

using namespace vgui;

CCSBuyMenu_CT::CCSBuyMenu_CT(void) : CCSBaseBuyMenu("BuySubMenu")
{
	m_iTeam = TEAM_CT;
}

CCSBuyMenu_TER::CCSBuyMenu_TER(void) : CCSBaseBuyMenu("BuySubMenu")
{
	m_iTeam = TEAM_TERRORIST;
}

CCSBaseBuyMenu::CCSBaseBuyMenu(const char *subPanelName) : CBuyMenu()
{
	SetTitle("#Cstrike_Buy_Menu", true);
	SetProportional(true);

	if (m_pMainMenu)
		delete m_pMainMenu;

	m_pMainMenu = new CCSBuySubMenu_DefaultMode(this, subPanelName);
	LoadControlSettings("Resource/UI/Buymenu.res", "GAME");
	LoadControlSettings("Resource/UI/CSO_Buymenu.res", "GAME");

	m_pMainMenu->LoadControlSettings("Resource/UI/cso_buysubmenu_ver5.res", "GAME");
	m_pMainMenu->SetVisible(false);


}

void CCSBaseBuyMenu::SetVisible(bool state)
{
	BaseClass::SetVisible(state);

	if (state)
	{
		Panel *defaultButton = FindChildByName("QuitButton");

		if (defaultButton)
			defaultButton->RequestFocus();

		SetMouseInputEnabled(true);
		m_pMainMenu->SetMouseInputEnabled(true);
	}
}

void CCSBaseBuyMenu::Init(void)
{
}

void CCSBaseBuyMenu::VidInit(void)
{
	SetVisible(false);
}

void CCSBaseBuyMenu::ShowPanel(bool bShow)
{
	if (bShow)
	{
		if (gHUD.m_iIntermission || gEngfuncs.IsSpectateOnly())
			return;
	}

	BaseClass::ShowPanel(bShow);
}

void CCSBaseBuyMenu::Paint(void)
{
	BaseClass::Paint();
}

void CCSBaseBuyMenu::PaintBackground(void)
{
	BaseClass::PaintBackground();
}

void CCSBaseBuyMenu::PerformLayout(void)
{
	BaseClass::PerformLayout();
}

void CCSBaseBuyMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CCSBaseBuyMenu::GotoMenu(int iMenu)
{
	if (!m_pMainMenu)
		return;

	const char *command = NULL;

	switch (iMenu)
	{
		case MENU_BUY_PISTOL:
		{
			if (m_iTeam == TEAM_CT)
				command = "Resource/UI/BuyPistols_CT.res";
			else
				command = "Resource/UI/BuyPistols_TER.res";

			break;
		}

		case MENU_BUY_SHOTGUN:
		{
			if (m_iTeam == TEAM_CT)
				command = "Resource/UI/BuyEquipment_CT.res";
			else
				command = "Resource/UI/BuyEquipment_TER.res";

			break;
		}

		case MENU_BUY_RIFLE:
		{
			if (m_iTeam == TEAM_CT)
				command = "Resource/UI/BuyRifles_CT.res";
			else
				command = "Resource/UI/BuyRifles_TER.res";

			break;
		}

		case MENU_BUY_SUBMACHINEGUN:
		{
			if (m_iTeam == TEAM_CT)
				command = "Resource/UI/BuySubMachineguns_CT.res";
			else
				command = "Resource/UI/BuySubMachineguns_TER.res";

			break;
		}

		case MENU_BUY_MACHINEGUN:
		{
			command = "Resource/UI/BuyMachineguns.res";
			break;
		}

		case MENU_BUY_ITEM:
		{
			if (m_iTeam == TEAM_CT)
				command = "Resource/UI/BuyEquipment_CT.res";
			else
				command = "Resource/UI/BuyEquipment_TER.res";

			break;
		}
	}

	ActivateNextSubPanel(m_pMainMenu);

	if (command)
	{
		m_pMainMenu->SetupNextSubPanel(command);
		m_pMainMenu->GotoNextSubPanel();
	}
}

void CCSBaseBuyMenu::ActivateMenu(int iMenu)
{
	GotoMenu(iMenu);

	g_pViewPort->ShowPanel(this, true);
}