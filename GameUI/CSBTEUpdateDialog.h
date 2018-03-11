#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/RichText.h>

using namespace vgui;

class CCSBTEUpdateDialog : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEUpdateDialog, Frame);

public:
	CCSBTEUpdateDialog(Panel *parent, const char *panelName, bool showTaskbarIcon = true);

protected:
	void OnCommand(const char *command);

private:
	RichText *m_pUpdateNews;
	Button *m_pCancelButton;
};