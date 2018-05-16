#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>

using namespace vgui;

class CCSBTECreatorDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTECreatorDialog, Frame);

public:
	CCSBTECreatorDialog(Panel *parent, const char *panelName, bool showTaskbarIcon = true);

protected:
	void OnCommand(const char *command);

private:
	RichText *m_pCreatorList;
	Button *m_pCancelButton;
};