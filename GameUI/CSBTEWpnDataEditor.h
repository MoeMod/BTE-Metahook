#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/ScrollBar.h>
#include <vgui_controls/MessageBox.h>

#include "PanelListPanel.h"
#include "ScriptObject.h"

#include <vector>
#include <forward_list>
#include <map>
#include <util/iniparser.h>

using namespace vgui;

#ifdef MessageBox
#undef MessageBox
#endif

class CCSBTEWpnDataEditorMessageBox : public MessageBox
{
	DECLARE_CLASS_SIMPLE(CCSBTEWpnDataEditorMessageBox, MessageBox);

public:
	CCSBTEWpnDataEditorMessageBox(const char *title, const char *text, Panel *parent = NULL) : MessageBox(title, text, parent) {}

	virtual void OnCommand(const char *pCommand)
	{
		BaseClass::OnCommand("");
		GetParent()->OnCommand(pCommand);
	}
};

class CCSBTEWpnDataEditorSearchBarMenu : public Menu
{
	DECLARE_CLASS_SIMPLE(CCSBTEWpnDataEditorSearchBarMenu, Menu);
public:
	CCSBTEWpnDataEditorSearchBarMenu(Panel *parent, const char *panelName)
		:Menu(parent, panelName)
	{
		SetKeyBoardInputEnabled(false);
	}
	virtual void RequestFocus(int direction = 0)
	{
		OnRequestFocus(GetVPanel(), NULL);
	}
};

class CCSBTEWpnDataEditorSearchBar : public ComboBox
{
	DECLARE_CLASS_SIMPLE(CCSBTEWpnDataEditorSearchBar, ComboBox);

public:
	CCSBTEWpnDataEditorSearchBar(Panel *parent, const char *panelName, int numLines = 6)
		: ComboBox(parent, panelName, numLines, true)
	{
		auto menu = new CCSBTEWpnDataEditorSearchBarMenu(this, NULL);
		SetMenu(menu);
	}
	virtual void OnSetFocus(){ TextEntry::OnSetFocus(); }

	MESSAGE_FUNC(OnMenuItemSelected, "MenuItemSelected");
};

class CCSBTEWpnDataEditor : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEWpnDataEditor, Frame);

public:
	CCSBTEWpnDataEditor(Panel *parent, const char *panelName, bool showTaskbarIcon = true);
	void SetLayout();
	void UpdateCurrentWeapons();
	void SaveData();
	void NextWpn();
	void PrevWpn();
	void CreateControls();
	void DestroyControls();
	void CountWpn();
	MESSAGE_FUNC(OnTextChanged, "TextChanged");
	MESSAGE_FUNC_CHARPTR(OnMenuItemSelected, "MenuItemSelected", weapon);

protected:
	virtual void OnCommand(const char *command) override;
	virtual void OnClose() override;

private:
	struct WeaponInfoKey_Less
	{
		bool operator()(const std::string &lhs, const std::string &rhs) const;
	};

	using MyIni = CBasicIniParser<std::unordered_map<std::string, std::map<std::string, std::string, WeaponInfoKey_Less>>>;
	MyIni m_iniData;
	MyIni::iterator m_iniDataIterator;
	std::vector<std::pair<std::string, std::wstring>> m_wpnNames;

	Button *m_pNextWpn, *m_pPrevWpn, *m_pSaveButton, *m_pSearchWpn;
	CCSBTEWpnDataEditorSearchBar *m_pWeaponSearch;
	//TextEntry *m_pDamage, *m_pDamageZombie, *m_pAttackInterval, *m_pMaxClip, *m_pMaxAmmo, *m_pMaxSpeed, *m_pReloadTime, *m_pDeployTime, *m_pKnockback, *m_pVelocityModifier, *m_pCost;
	Label *m_pName, *curWpnID, *totalWpnID, *Divider;
	CPanelListPanel *m_pListPanel;

};