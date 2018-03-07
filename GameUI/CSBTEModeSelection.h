#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ImagePanel.h>

using namespace vgui;

#define MAX_GAMEMODES 12


class CCSBTEModeSelection : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEModeSelection, Frame);

public:
	CCSBTEModeSelection(Panel *parent, const char *panelName, bool showTaskbarIcon = true);

protected:
	void OnCommand(const char *command);

private:
	void UpdateCurrentMode();
	Label *m_pInfoLabel;
	ImagePanel *m_pModeImage[MAX_GAMEMODES];
	ImagePanel *m_pSelectedImage;
	Button *m_pModeButton[MAX_GAMEMODES];
	Button *m_pCancelButton;
};
