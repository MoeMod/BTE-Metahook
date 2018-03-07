#pragma once

#include <vgui_controls/PHandle.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/MenuButton.h>
#include <vgui_controls/MessageBox.h>

#include "GameUI/OptionsDialog.h"
#include "GameUI/CreateMultiplayerGameDialog.h"

using namespace vgui;

class CCSBTEGameMenuButton : public MenuButton
{
	DECLARE_CLASS_SIMPLE(CCSBTEGameMenuButton, Button);

public:
	CCSBTEGameMenuButton(Panel *parent, const char *panelName, const char *text);
	~CCSBTEGameMenuButton();


protected:
	// Paint button on screen
	virtual void Paint(void);
	virtual void PaintBorder(void) { return; }
	virtual void PaintBackground(void);

};


class CCSBTEGameMenu : public Menu
{
	DECLARE_CLASS_SIMPLE(CCSBTEGameMenu, Menu);

public:
	CCSBTEGameMenu(Panel *parent, const char *panelName);
	~CCSBTEGameMenu();

public:
	void OnOpenCreateMultiplayerGameDialog();
	void OnOpenOptionsDialog();
	void OnOpenQuitConfirmationDialog();

protected:
	virtual void PaintBackground();
	void OnCommand(const char *command);
	void PositionDialog(vgui::PHandle dlg);

public:
	DHANDLE<COptionsDialog>m_hOptionsDialog;
	DHANDLE<CCreateMultiplayerGameDialog>m_hCreateMultiplayerGameDialog;

private:
	CCSBTEGameMenuButton *m_pNewGame;
	CCSBTEGameMenuButton *m_pServerBrowser;
	CCSBTEGameMenuButton *m_pOptions;
	CCSBTEGameMenuButton *m_pExit;
};