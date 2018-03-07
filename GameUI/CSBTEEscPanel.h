#pragma once

#include <vgui_controls/PHandle.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>

using namespace vgui;

class CCSBTEEscPanelButton : public Button
{
	DECLARE_CLASS_SIMPLE(CCSBTEEscPanelButton, Button);

public:
	CCSBTEEscPanelButton(Panel *parent, const char *panelName, const char *text, Panel *pActionSignalTarget = NULL, const char *pCmd = NULL) : Button(parent, panelName, text, pActionSignalTarget, pCmd) 
	{
		for (int i = 0; i < 3; i++)
		{
			m_pszImageName[i] = NULL;
			m_pImage[i] = NULL;
		}
	}
	CCSBTEEscPanelButton(Panel *parent, const char *panelName, const wchar_t *text, Panel *pActionSignalTarget = NULL, const char *pCmd = NULL) : Button(parent, panelName, text, pActionSignalTarget, pCmd) 
	{
		for (int i = 0; i < 3; i++)
		{
			m_pszImageName[i] = NULL;
			m_pImage[i] = NULL;
		}
	}
	~CCSBTEEscPanelButton();

	void SetImage(char *c, char *n, char *o);

protected:
	// Paint button on screen
	virtual void Paint(void);
	virtual void PaintBorder(void) { return; }
	virtual void PaintBackground(void);

private:
	IImage *m_pImage[3];
	char *m_pszImageName[3];

};



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