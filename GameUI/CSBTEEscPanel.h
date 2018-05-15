#pragma once

#include <vgui_controls/PHandle.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>

using namespace vgui;

using CCSBTEEscPanelButton = class TexturedButton;

class CCSBTEEscPanel : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEEscPanel, Frame);

public:
	CCSBTEEscPanel(Panel *parent, const char *panelName, bool showTaskbarIcon = true);
	~CCSBTEEscPanel();

protected:
	virtual void PaintBackground();
	void OnCommand(const char *command);

private:
	Panel *m_pBasePanel;
	CCSBTEEscPanelButton *m_pShooting;
	CCSBTEEscPanelButton *m_pRecording;
	CCSBTEEscPanelButton *m_pReport;
	CCSBTEEscPanelButton *m_pOption;
	CCSBTEEscPanelButton *m_pExit;
	CCSBTEEscPanelButton *m_pCancel;
};