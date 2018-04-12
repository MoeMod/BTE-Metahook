#ifndef NEWTABBUTTON_H
#define NEWTABBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <VGUI/IBorder.h>
#include <VGUI/IScheme.h>
#include <KeyValues.h>

#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>

class NewTabButton : public vgui::Button
{
	typedef vgui::Button BaseClass;

private:
	Color _replaceColor;
	int m_bMaxTabWidth;

public:
	NewTabButton(vgui::Panel *parent, const char *panelName, const char *text, int maxTabWidth = 64) :
		BaseClass(parent, panelName, text)
	{
		SetCommand(new KeyValues("TabPressed"));
		m_bMaxTabWidth = maxTabWidth;
	}

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		// set up the scheme settings
		Button::ApplySchemeSettings(pScheme);

		GetTextImage()->SetDrawWidth(vgui::scheme()->GetProportionalScaledValue(2));

		int wide, tall;
		int contentWide, contentTall;
		GetSize(wide, tall);
		GetContentSize(contentWide, contentTall);

		wide = max(m_bMaxTabWidth, contentWide + 10);  // 10 = 5 pixels margin on each side
		SetSize(wide, tall);

		SetDefaultBorder(pScheme->GetBorder("TabBorder"));
		SetKeyFocusBorder(pScheme->GetBorder("TabBorder"));
		SetDepressedBorder(pScheme->GetBorder("TabActiveBorder"));

		/*_imageBackground = true;
		_depressedImage[0] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_abled_left_c", true);
		_depressedImage[1] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_abled_center_c", true);
		_depressedImage[2] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_abled_right_c", true);
		_defaultImage[0] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_disabled_left_n", true);
		_defaultImage[1] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_disabled_center_n", true);
		_defaultImage[2] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_disabled_right_n", true);
		_armedImage[0] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_abled_left_o", true);
		_armedImage[1] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_abled_center_o", true);
		_armedImage[2] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_small_abled_right_o", true);*/

		_imageBackground = true;
		_depressedImage[0] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.LeftC"), true);
		_depressedImage[1] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.CenterC"), true);
		_depressedImage[2] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.RightC"), true);
		_defaultImage[0] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.LeftN"), true);
		_defaultImage[1] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.CenterN"), true);
		_defaultImage[2] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.RightN"), true);
		_armedImage[0] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.LeftO"), true);
		_armedImage[1] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.CenterO"), true);
		_armedImage[2] = vgui::scheme()->GetImage(pScheme->GetResourceString("PropertyTab.RightO"), true);
	}

	virtual void SetActive(bool state)
	{
		ForceDepressed(state);

		InvalidateLayout();
		Repaint();
	}

	virtual bool CanBeDefaultButton(void)
	{
		return false;
	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();
	}

	virtual Color GetButtonFgColor()
	{
		if (IsDepressed())
			return _replaceColor;
		return GetDisabledFgColor1();
	}

	void SetTextColor(Color col)
	{
		_replaceColor = col;
	}
};

#endif