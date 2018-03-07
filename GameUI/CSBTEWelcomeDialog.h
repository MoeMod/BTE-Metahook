#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/URLLabel.h>
#include <vgui_controls/ImagePanel.h>

using namespace vgui;

class CCSBTEWelcomeDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEWelcomeDialog, Frame);

public:
	CCSBTEWelcomeDialog(Panel *parent, const char *panelName, bool showTaskbarIcon = true);

protected:
	void OnCommand(const char *command);
	virtual void OnClose();

private:
	ImagePanel *m_pLogo;
	Label *m_pInfoLabel, *m_pLanguageSelectionLabel;
	ComboBox *m_pLanguageSelection;
	
	Button *m_pOKButton, *m_pCancelButton;
};