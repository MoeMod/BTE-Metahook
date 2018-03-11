#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>
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
	CCSBTEWpnDataEditorMessageBox(const wchar_t *wszTitle, const wchar_t *wszText, Panel *parent = NULL) : MessageBox(wszTitle, wszText, parent) { ; }

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
	void UpdateCurrentWeapon();
	void NextWpn();

protected:
	void OnCommand(const char *command);

private:
	CIniParser m_iniData;
	CIniParser::iterator m_iniDataIterator;
	Button * m_pCancelButton, *m_pNextWpn, *m_pPreviousWpn;
	Label *m_pName, *m_pDamage, *m_pDamageZombie, *m_pAttackInterval, *m_pMaxClip, *m_pMaxAmmo, *m_pMaxSpeed, *m_pReloadTime, *m_pDeployTime, *m_pKnockback, *m_pVelocityModifier, *m_pCost;
	
	char m_szConfigPath[256];

	enum WeaponType
	{
		NONE = 0,
		PISTOL,
		SHOTGUN,
		SMG,
		RIFLE,
		MG,
		KNIFE,
		EQUIP
	};

	struct WeaponInfo
	{
		char szName[32];
		WeaponType iMenu;
		bool isMyWpn;
	};

	std::vector<WeaponInfo> m_WeaponInfoList;

	std::vector<int> m_WeaponInfoDisplayId;
};
