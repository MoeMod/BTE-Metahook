#ifndef NEWTABPANELH
#define NEWTABPANELH

#ifdef _WIN32
#pragma once
#endif

#include <VGUI/IBorder.h>
#include <VGUI/IScheme.h>
#include <KeyValues.h>

#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>

class ColoredButton : public vgui::Button
{
	DECLARE_CLASS_SIMPLE(ColoredButton, Button);

	Color _replaceColor;
public:
	ColoredButton(vgui::Panel *parent, const char *panelName, const char *text) :
		Button(parent, panelName, text) {}

	void SetTextColor(Color col)
	{
		_replaceColor = col;
	}

	virtual Color GetButtonFgColor()
	{
		return _replaceColor;
	}
};

class NewTabButton : public ColoredButton
{
	DECLARE_CLASS_SIMPLE(NewTabButton, ColoredButton);

private:
	bool _active;
	
	int m_bMaxTabWidth;
	vgui::IBorder *m_pActiveBorder;
	vgui::IBorder *m_pNormalBorder;

public:
	NewTabButton(vgui::Panel *parent, const char *panelName, const char *text, int maxTabWidth = 64) :
		ColoredButton( parent, panelName, text)
	{
		SetCommand(new KeyValues("TabPressed"));
		_active = false;
		m_bMaxTabWidth = maxTabWidth;
		m_pActiveBorder = m_pNormalBorder = NULL;
	}

	virtual void Paint()
	{
		BaseClass::Paint();
	}

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{
		// set up the scheme settings
		Button::ApplySchemeSettings(pScheme);

		m_pActiveBorder = pScheme->GetBorder("TabActiveBorder");
		m_pNormalBorder = pScheme->GetBorder("TabBorder");

		int wide, tall;
		int contentWide, contentTall;
		GetSize(wide, tall);
		GetContentSize(contentWide, contentTall);

		wide = max(m_bMaxTabWidth, contentWide + 10);  // 10 = 5 pixels margin on each side
		SetSize(wide, tall);
		
		_imageBackground = true;
		_depressedImage[0] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_left_c", true);
		_depressedImage[1] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_center_c", true);
		_depressedImage[2] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_right_c", true);
		_defaultImage[0] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_left_n", true);
		_defaultImage[1] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_center_n", true);
		_defaultImage[2] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_right_n", true);
		_armedImage[0] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_left_o", true);
		_armedImage[1] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_center_o", true);
		_armedImage[2] = vgui::scheme()->GetImage("resource/control/tabbutton/tab_big_abled_right_o", true);
	}

	vgui::IBorder *GetBorder(bool depressed, bool armed, bool selected, bool keyfocus)
	{
		if (_active && m_pActiveBorder)
		{
			return m_pActiveBorder;
		}
		if(m_pNormalBorder)
			return m_pNormalBorder;
		return BaseClass::GetBorder(depressed, armed, selected, keyfocus);
	}

	virtual void SetActive(bool state)
	{
		_active = state;
		InvalidateLayout();
		Repaint();
	}

    virtual bool CanBeDefaultButton(void)
    {
        return false;
    }

	//Fire action signal when mouse is pressed down instead  of on release.
	virtual void OnMousePressed(vgui::MouseCode code)
	{
		// check for context menu open
		if (!IsEnabled())
			return;
		
		if (!IsMouseClickEnabled(code))
			return;
		
		if (IsUseCaptureMouseEnabled())
		{
			{
				RequestFocus();
				FireActionSignal();
				SetSelected(true);
				Repaint();
			}
			
			// lock mouse input to going to this button
			vgui::input()->SetMouseCapture(GetVPanel());
		}
	}

	virtual void OnMouseReleased(vgui::MouseCode code)
	{
		// ensure mouse capture gets released
		if (IsUseCaptureMouseEnabled())
		{
			vgui::input()->SetMouseCapture(NULL);
		}

		// make sure the button gets unselected
		SetSelected(false);
		Repaint();

	}

	virtual void PerformLayout()
	{
		BaseClass::PerformLayout();
	}
};

#endif