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

class NewTabPanel : public vgui::Button
{
	DECLARE_CLASS_SIMPLE(NewTabPanel, Button );

private:
	bool _active;
	Color _replaceColor;
	int m_bMaxTabWidth;
	vgui::IBorder *m_pActiveBorder;
	vgui::IBorder *m_pNormalBorder;

public:
	NewTabPanel(vgui::Panel *parent, const char *panelName, const char *text, int maxTabWidth = 64) :
		Button( parent, panelName, text)
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

	void SetTextColor(Color col)
	{
		_replaceColor = col;
	}

	virtual Color GetButtonFgColor()
	{
		return _replaceColor;
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