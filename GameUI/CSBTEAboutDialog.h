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

class CCSBTEAboutDialogMessageBox : public MessageBox
{
	DECLARE_CLASS_SIMPLE(CCSBTEAboutDialogMessageBox, MessageBox);

public:
	CCSBTEAboutDialogMessageBox(const char *title, const char *text, Panel *parent = NULL) : MessageBox(title, text, parent) { ; }
	CCSBTEAboutDialogMessageBox(const wchar_t *wszTitle, const wchar_t *wszText, Panel *parent = NULL) : MessageBox(wszTitle, wszText, parent) { ; }

	virtual void OnCommand(const char *pCommand)
	{
		BaseClass::OnCommand("");
		GetParent()->OnCommand(pCommand);
	}
};
/*
class CCSBTEAboutDialogComboBox : public ComboBox
{
	DECLARE_CLASS_SIMPLE(CCSBTEAboutDialogComboBox, ComboBox);

public:
	CCSBTEAboutDialogComboBox(Panel *parent, RichText *pInsert, const char *panelName, int numLines, bool allowEdit) : ComboBox(parent, panelName, numLines, allowEdit) { m_pText = pInsert; }

	virtual void OnMenuItemSelected();

private:
	RichText *m_pText;
};
*/
class CCSBTEAboutDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEAboutDialog, Frame);

public:
	CCSBTEAboutDialog(Panel *parent, const char *panelName, bool showTaskbarIcon = true);

protected:
	void OnCommand(const char *command);

private:
	ImagePanel *m_pLogo;
	Label *m_pMoreInfoLabel, *m_pNewHUDSwitchLabel, *m_pNewMuzzleflashSwitchLabel, *m_pFireDlightSwitchLabel, *m_pBloodSwitchLabel, *m_pLanguageSelectionLabel;
	RichText *m_pUpdateInfo;
	URLLabel *m_pCsoldjbURL;
	ComboBox *m_pNewHUDSwitch, *m_pNewMuzzleflashSwitch, *m_pFireDlightSwitch, *m_pBloodSwitch, *m_pLanguageSelection;
	//CCSBTEAboutDialogComboBox *m_pVersionSwitch;
	
	Button *m_pOKButton, *m_pCancelButton;
};