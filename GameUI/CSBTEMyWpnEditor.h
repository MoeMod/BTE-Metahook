#pragma once

#include <vgui_controls/PHandle.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/PropertyPage.h>
#include <vgui_controls/PropertySheet.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/ScrollBar.h>
#include <vgui_controls/TextImage.h>

#include <vector>

using namespace vgui;

//#define MAX_WPN 512

class CSBTEDescPanel : public TextImage
{
	DECLARE_CLASS_SIMPLE(CSBTEDescPanel, TextImage);
public:
	// Constructors
	CSBTEDescPanel(const char *text) : TextImage(text) {}

};

class CCSBTEMyWpnElement : public ImagePanel
{
	DECLARE_CLASS_SIMPLE(CCSBTEMyWpnElement, ImagePanel);
public:
	CCSBTEMyWpnElement(Panel *parent, const char *name);
	void SetId(int id);
	void SetWeapon(char *szWeaponName, int iTeam);
	void SetSelected(bool bSelected);
	virtual void OnMouseDoublePressed(MouseCode code);
	virtual void OnCursorEntered();
	virtual void OnCursorExited();
	MESSAGE_FUNC_INT_INT(OnCursorMoved, "OnCursorMoved", x, y);

private:
	ImagePanel *m_pTextLine;
	Label *m_pText;
	Label *m_pText2;
	ImagePanel *m_pWeaponImage;
	ImagePanel *m_pTeamImage;
	ImagePanel *m_pItemBackground;
	ImagePanel *m_pSelectedBackground;
	Button *m_pApplyButton;
	CSBTEDescPanel *m_pDescPanel;

	bool m_bShowDesc;
};

class CCSBTEMyWpnPropertyPage : public PropertyPage
{
	DECLARE_CLASS_SIMPLE(CCSBTEMyWpnPropertyPage, PropertyPage);
	CCSBTEMyWpnPropertyPage(Panel *parent, const char *panelName) : PropertyPage(parent, panelName) {}

	MESSAGE_FUNC_PTR(OnPageTabActivated, "PageTabActivated", panel);
};

class CCSBTEMyWpnEditor : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEMyWpnEditor, Frame);

public:
	CCSBTEMyWpnEditor(Panel *parent, const char *panelName, bool showTaskbarIcon = true);
	virtual void PerformLayout();
	virtual void OnMouseWheeled(int delta);

	MESSAGE_FUNC(OnScrollBarSliderMoved, "ScrollBarSliderMoved");
	MESSAGE_FUNC_PTR(OnPageTabActivated, "PageTabActivated", panel);

	void SwitchToLine(int iLine);
	bool MyWpnRewrite();
	

protected:
	void OnCommand(const char *command);

private:
	CCSBTEMyWpnElement *m_pElements[8];
	ScrollBar *m_pScrollBar;
	PropertySheet *m_pPropertySheet;
	CCSBTEMyWpnPropertyPage *m_pPropertyPage[8];


	int m_iLine;
	
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

	enum WeaponBuyTeam
	{
		ALL,
		TR,
		CT
	};

	struct WeaponInfo
	{
		char szName[32];
		WeaponBuyTeam iTeam;
		WeaponType iMenu;
		bool isMyWpn;
	};
	std::vector<WeaponInfo> m_WeaponInfoList;
	
	std::vector<int> m_WeaponInfoDisplayId;
};
