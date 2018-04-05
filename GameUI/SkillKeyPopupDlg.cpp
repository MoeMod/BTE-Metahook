

#include "configs.h"
#include "common.h"
#include "SkillKeyPopupDlg.h"

#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui_controls/ListPanel.h>
#include <KeyValues.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>
#include "PanelListPanel.h"
#include <vgui/IInput.h>

#include <keydefs.h>

#include "FileSystem.h"

using namespace vgui;

char * const CSkillKeyPopupDlg::m_szZombieName[14] = {
	"#CSO_ZombieType_defaultzb", "#CSO_ZombieType_lightzb", "#CSO_ZombieType_heavyzb", "#CSO_ZombieType_pczb", 
	"#CSO_ZombieType_doctorzb", "#CSO_ZombieType_deimoszb", "#CSO_ZombieType_deimos2zb", "#CSO_ZombieType_witchzb", 
	"#CSO_ZombieType_undertakerzb", "#CSO_ZombieType_chinazb", "#CSO_ZombieType_boomerzb", "#CSO_ZombieType_residentzb",
	"#CSO_ZombieType_revivalzb", "#CSO_ZombieType_teleportzb"
};

char * const CSkillKeyPopupDlg::m_szZombieSkillName[14][4] = {
	{ "#CSO_ZombieSkill_zombicrazy", "#CSO_ZombieSkill_strengthrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombiHiding", "#CSO_ZombieSkill_jumpupm", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombitrap", "#CSO_ZombieSkill_armorrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombismoke", "#CSO_ZombieSkill_strengthrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombiheal", "#CSO_ZombieSkill_strengthrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombideimos", "#CSO_ZombieSkill_strengthrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombicrazy2", "#CSO_ZombieSkill_armorrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombihook", "#CSO_ZombieSkill_jumpupm", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombipile", "#CSO_ZombieSkill_armorrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombistiffen", "#CSO_ZombieSkill_strengthrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_recoverylast", "#CSO_ZombieSkill_armorrecovery", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombipenetration", "#CSO_ZombieSkill_jumpupm", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombirevival", "#CSO_ZombieSkill_hpbuff", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
	{ "#CSO_ZombieSkill_zombiteleport", "#CSO_ZombieSkill_hpbuff", "#CSO_ZombieSkill_divisioncome", "#CSO_ZombieSkill_divisionfly" },
};

char * const CSkillKeyPopupDlg::m_szZombieSkillIcon[14][4] = {
	{ "zombieskill_zombicrazy", "zombieskill_strengthrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombihiding", "zombieskill_jumpupm", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombitrap", "zombieskill_armorrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombismoke", "zombieskill_strengthrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombiheal", "zombieskill_strengthrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_tentacle", "zombieskill_strengthrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombicrazy2", "zombieskill_armorrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombihook", "zombieskill_jumpupm", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombipile", "zombieskill_armorrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombistiffen", "zombieskill_strengthrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_recoverylast", "zombieskill_armorrecovery", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombipenetration", "zombieskill_jumpupm", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombirevival", "zombieskill_hpbuff", "zombieskill_divisioncome", "zombieskill_divisionfly" },
	{ "zombieskill_zombiteleport", "zombieskill_hpbuff", "zombieskill_divisioncome", "zombieskill_divisionfly" },
};

char * const CSkillKeyPopupDlg::m_szHumanSkillName[4] = {
	"#CSO_HumanSkill_hm_spd",
	"#CSO_HumanSkill_hm_hd",
	"#CSO_HumanSkill_hm_rambo",
	"#CSO_HumanSkill_hm_ss"
};

char * const CSkillKeyPopupDlg::m_szHumanSkillIcon[4] = {
	"humanskill_hm_spd",
	"humanskill_hm_hd",
	"humanskill_hm_rambo",
	"humanskill_hm_ss"
};

char * const CSkillKeyPopupDlg::m_szZombieKeyInfo[11] = {
	"0", "5", "6", "7", "8", "9", "G", "F1", "F2", "F3", "F4"
};
const int CSkillKeyPopupDlg::m_iZombieKeyVK[11] = {
	'0', '5', '6', '7', '8', '9', 'g', K_F1, K_F2, K_F3, K_F4
};

char * const CSkillKeyPopupDlg::m_szHumanKeyInfo[10] = {
	"0", "5", "6", "7", "8", "9", "F1", "F2", "F3", "F4"
};

const int CSkillKeyPopupDlg::m_iHumanKeyVK[10] = {
	'0', '5', '6', '7', '8', '9', K_F1, K_F2, K_F3, K_F4
};

CSkillKeyPopupDlg::CSkillKeyPopupDlg(vgui::Panel *parent) : BaseClass(NULL, "SkillKeyPopupDlg")
{
	SetBounds(0, 0, 500, 500);
	SetSizeable(false);

	SetTitle("#CSO_SkillKeySetting", true);

	Button *m_pbtn_close = new Button(this, "btn_close", "#CSO_Cancel_Btn");
	m_pbtn_close->SetCommand("Close");

	Button *m_pbtn_OK = new Button(this, "btn_OK", "#CSO_OKl_Btn");
	m_pbtn_OK->SetCommand("Ok");

	Button *m_pPrevBtn = new Button(this, "PrevBtn", "<");
	m_pPrevBtn->SetCommand("Prev");

	Button *m_pNextBtn = new Button(this, "NextBtn", ">");
	m_pNextBtn->SetCommand("Next");

	m_pZombieTypeBG = new TextEntry(this, "ZombieTypeBG");
	m_pZombieTypeBG->SetMouseInputEnabled(false);
	m_pZombieType = new Label(this, "ZombieType", "");

	for (int i = 0; i < 4; i++)
	{
		m_pZombieSlotBg[i] = new TextEntry(this, va("ZombieSlotBg%d", i + 1));
		m_pZombieSlotBg[i]->SetMouseInputEnabled(false);
		
		m_pZombieSlot[i] = new ImagePanel(this, va("ZombieSlot%d", i + 1));
		m_pZombieSkill[i] = new Label(this, va("ZombieSkill%d", i + 1), "");
		m_pZombieSkillKey[i] = new CSkillKeyPopupDlgComboBox(this, va("ZombieSkillKey%d", i + 1), 11, false);
		for (int j = 0; j < 11; j++)
		{
			m_pZombieSkillKey[i]->AddItem(m_szZombieKeyInfo[j], NULL);
		}

		m_pHumanSlotBg[i] = new TextEntry(this, va("HumanSlotBg%d", i + 1));
		m_pHumanSlotBg[i]->SetMouseInputEnabled(false);
		m_pHumanSlot[i] = new ImagePanel(this, va("HumanSlot%d", i + 1));
		m_pHumanSkill[i] = new CSkillKeyPopupDlgComboBox(this, va("HumanSkill%d", i + 1), 4, false);
		
		for (int j = 0; j < 4; j++)
		{
			m_pHumanSkill[i]->AddItem(m_szHumanSkillName[j], NULL);
		}
		m_pHumanSkillKey[i] = new CSkillKeyPopupDlgComboBox(this, va("HumanSkillKey%d", i + 1), 10, false);
		for (int j = 0; j < 10; j++)
		{
			m_pHumanSkillKey[i]->AddItem(m_szHumanKeyInfo[j], NULL);
		}
	}
	
	for (int i = 0; i < 4; i++)
	{
		m_pZombieSkillKey[i]->SetGroup(m_pZombieSkillKey, 4);
		m_pHumanSkill[i]->SetGroup(m_pHumanSkill, 4);
		m_pHumanSkillKey[i]->SetGroup(m_pHumanSkillKey, 4);
	}
	
	LoadControlSettings("Resource\\zombi\\popup_option_bindskill.res");

	MoveToCenterOfScreen();
	SetSizeable(false);
	SetDeleteSelfOnClose(true);

	ReadConfig();
	UpdateZombieSkill(0);
	UpdateHumanSkill();
}

CSkillKeyPopupDlg::~CSkillKeyPopupDlg(void)
{
	
}

void CSkillKeyPopupDlg::UpdateZombieSkill(int id)
{
	if (id > 13) id = 0;
	if (id < 0) id = 13;
	m_iZombieID = id;

	m_pZombieType->SetText(m_szZombieName[id]);
	for (int i = 0; i < 4; i++)
	{
		m_pZombieSlot[i]->SetImage(va("resource/zombi/%s", m_szZombieSkillIcon[id][i]));
		m_pZombieSkill[i]->SetText(m_szZombieSkillName[id][i]);
	}
}

void CSkillKeyPopupDlg::UpdateHumanSkill()
{
	for (int i = 0; i < 4; i++)
	{
		m_pHumanSlot[i]->SetImage(va("resource/zombi/%s", m_szHumanSkillIcon[m_pHumanSkill[i]->GetActiveItem()]));
	}
}

void CSkillKeyPopupDlgComboBox::OnSetText(const wchar_t *text)
{
	int iCurItem = GetActiveItem();
	if (m_ppGroup)
	{
		for (int i = 0; i < m_iGroupItemCount; i++)
		{
			if (m_ppGroup[i] != this && m_ppGroup[i]->GetActiveItem() == iCurItem)
			{
				m_ppGroup[i]->ComboBox::ActivateItem(m_iLastItem);
			}
		}
	}

	m_iLastItem = GetActiveItem();
	BaseClass::OnSetText(text);
	GetParent()->OnCommand("UpdateItems");
}

void CSkillKeyPopupDlg::Activate(void)
{
	BaseClass::Activate();

	input()->SetAppModalSurface(GetVPanel());
}

void CSkillKeyPopupDlg::OnClose(void)
{
	BaseClass::OnClose();

	vgui::input()->SetAppModalSurface(NULL);
}

void CSkillKeyPopupDlg::OnCommand(const char *command)
{
	if (!stricmp(command, "Ok"))
	{
		SaveConfig();
		OnClose();
		return;
	}
	else if (!stricmp(command, "Prev"))
	{
		UpdateZombieSkill(m_iZombieID - 1);
		return;
	}
	else if (!stricmp(command, "Next"))
	{
		UpdateZombieSkill(m_iZombieID + 1);
		return;
	}
	else if (!stricmp(command, "UpdateItems"))
	{
		UpdateZombieSkill(m_iZombieID);
		UpdateHumanSkill();
		return;
	}

	BaseClass::OnCommand(command);
}

void CSkillKeyPopupDlg::OnKeyCodeTyped(KeyCode code)
{
	if (code == KEY_ESCAPE)
	{
		SetAlpha(0);
		Close();
	}
	else
		BaseClass::OnKeyCodeTyped(code);
}

void CSkillKeyPopupDlg::ReadConfig(void)
{
	for (int i = 0; i < 4; i++)
	{
		m_pHumanSkill[i]->ComboBox::ActivateItem(g_SkillKeyConfig.iHumanSlotAlias[i]);
		m_pHumanSkillKey[i]->ComboBox::ActivateItem(g_SkillKeyConfig.iHumanSkillKey[i]);
		m_pZombieSkillKey[i]->ComboBox::ActivateItem(g_SkillKeyConfig.iZombieSkillKey[i]);
	}
}

void CSkillKeyPopupDlg::SaveConfig(void)
{
	for (int i = 0; i < 4; i++)
	{
		g_SkillKeyConfig.iHumanSlotAlias[i] = m_pHumanSkill[i]->GetActiveItem();
		g_SkillKeyConfig.iHumanSkillKey[i] = m_pHumanSkillKey[i]->GetActiveItem();
		g_SkillKeyConfig.iZombieSkillKey[i] = m_pZombieSkillKey[i]->GetActiveItem();
	}
	Config_SaveSkillKey();
}