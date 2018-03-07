#ifndef SKILLKEYPOPUPDLG_H
#define SKILLKEYPOPUPDLG_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ComboBox.h>

using namespace vgui;

struct SkillKeyConfig_s;

class CSkillKeyPopupDlgComboBox : public vgui::ComboBox
{
	DECLARE_CLASS_SIMPLE(CSkillKeyPopupDlgComboBox, vgui::ComboBox);
public:
	CSkillKeyPopupDlgComboBox(Panel *parent, const char *panelName, int numLines, bool allowEdit) : ComboBox(parent, panelName, numLines, allowEdit) 
	{
		m_ppGroup = NULL;
		m_iGroupItemCount = 1;
	}
	void SetGroup(CSkillKeyPopupDlgComboBox **ppGroup, int iCount)
	{
		m_ppGroup = ppGroup;
		m_iGroupItemCount = iCount;
	}
public:
	MESSAGE_FUNC_WCHARPTR(OnSetText, "SetText", text);
	CSkillKeyPopupDlgComboBox **m_ppGroup;
	int m_iGroupItemCount;
	int m_iLastItem;
};

class CSkillKeyPopupDlg : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CSkillKeyPopupDlg, vgui::Frame);

	class RoundPanel : public vgui::Panel
	{
		DECLARE_CLASS_SIMPLE(RoundPanel, vgui::Panel);
	public:
		RoundPanel(Panel *parent, const char *panelName) : Panel(parent, panelName) {}
	};

public:
	CSkillKeyPopupDlg(vgui::Panel *parent);
	~CSkillKeyPopupDlg(void);

public:
	virtual void Activate(void);

private:
	void ReadConfig(void);
	void SaveConfig(void);
	void UpdateItems(void);
	void UpdateZombieSkill(int id);
	void UpdateHumanSkill();

private:
	// Human
	TextEntry *m_pHumanSlotBg[4];
	ImagePanel *m_pHumanSlot[4];
	ImagePanel *m_pExpired[4];
	CSkillKeyPopupDlgComboBox *m_pHumanSkill[4];
	CSkillKeyPopupDlgComboBox *m_pHumanSkillKey[4];

	// Zombie
	Label *m_pZombieType;
	TextEntry *m_pZombieTypeBG;
	TextEntry *m_pZombieSlotBg[4];
	ImagePanel *m_pZombieSlot[4];
	Label *m_pZombieSkill[4];
	CSkillKeyPopupDlgComboBox *m_pZombieSkillKey[4];
	Button *m_pbtn_OK;
	Button *m_pbtn_close;

	int m_iZombieID;

public:
	virtual void OnCommand(const char *command);
	virtual void OnClose(void);
	virtual void OnKeyCodeTyped(vgui::KeyCode code);

public:
	static char * const m_szZombieName[14];
	static char * const m_szZombieSkillName[14][4];
	static char * const m_szZombieSkillIcon[14][4];
	static char * const m_szZombieKeyInfo[11];
	static const int m_iZombieKeyVK[11];
	static char * const m_szHumanSkillName[4];
	static char * const m_szHumanSkillIcon[4];
	static char * const m_szHumanKeyInfo[10];
	static const int m_iHumanKeyVK[10];

	friend void SwapHumanSkill(CSkillKeyPopupDlg *pthis, CSkillKeyPopupDlgComboBox *pthat, int iNewSkillSlotAlias);

	CSkillKeyPopupDlgComboBox *FindHumanSlotComboBox(int iSlot)
	{
		for (int i = 0; i < 4;i++)
			if (m_pHumanSkill[i]->GetActiveItem() == iSlot)
			{
				return m_pHumanSkill[i];
			}
		return NULL;
	}

	CSkillKeyPopupDlgComboBox *FindHumanKeyComboBox(int iKeyID)
	{
		for (int i = 0; i < 4; i++)
			if (m_pZombieSkillKey[i]->GetActiveItem() == iKeyID)
			{
				return m_pHumanSkill[i];
			}
		return NULL;
	}

};

#endif
