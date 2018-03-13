#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/ScrollBar.h>
#include <vgui_controls/MessageBox.h>

#include <vector>
#include <util/iniparser.h>

using namespace vgui;

class CCSBTEWpnDataEditorMessageBox : public MessageBox
{
	DECLARE_CLASS_SIMPLE(CCSBTEWpnDataEditorMessageBox, MessageBox);

public:
	CCSBTEWpnDataEditorMessageBox(const char *title, const char *text, Panel *parent = NULL) : MessageBox(title, text, parent) { ; }

	virtual void OnCommand(const char *pCommand)
	{
		BaseClass::OnCommand("");
		GetParent()->OnCommand(pCommand);
	}
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

protected:
	void OnCommand(const char *command);

private:
	CIniParser m_iniData;
	CIniParser::iterator m_iniDataIterator;
	Button * m_pCancelButton, *m_pNextWpn, *m_pPrevWpn, *bSaveData;
	TextEntry *m_pDamage, *m_pDamageZombie, *m_pAttackInterval, *m_pMaxClip, *m_pMaxAmmo, *m_pMaxSpeed, *m_pReloadTime, *m_pDeployTime, *m_pKnockback, *m_pVelocityModifier, *m_pCost;
	Label *m_pName, *m_plDamage, *m_plDamageZombie, *m_plAttackInterval, *m_plMaxClip, *m_plMaxAmmo, *m_plMaxSpeed, *m_plReloadTime, *m_plDeployTime, *m_plKnockback, *m_plVelocityModifier, *m_plCost;
	CCSBTEWpnDataEditorMessageBox *msgbox;
};
