#pragma once

#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MenuItem.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/ImageList.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/TextImage.h>
#include "BinkPanel.h"

using namespace vgui;

#define MAX_DNA 12

class CCSBTEZombieDNAButton : public ImagePanel
{
	DECLARE_CLASS_SIMPLE(CCSBTEZombieDNAButton, ImagePanel);

public:
	CCSBTEZombieDNAButton(Panel *parent, const char *panelName) : ImagePanel(parent, panelName)
	{
		for (int i = 0; i < 2; i++)
		{
			m_pImage[i] = NULL;
		}
		m_iId = 0;
		m_bSelected = false;
	}

	void SetId(int id);
	void SetSelected(bool x);

protected:
	// Paint button on screen
	virtual void Paint(void);
	virtual void PaintBorder(void) { return; }
	virtual void PaintBackground(void);
	virtual void OnMouseDoublePressed(MouseCode code);
	

private:
	IImage *m_pImage[2];
	int m_iId;
	bool m_bSelected;
};

class CCSBTEZombieDNA : public Frame
{
	DECLARE_CLASS_SIMPLE(CCSBTEZombieDNA, Frame);

public:
	CCSBTEZombieDNA(Panel *parent, const char *panelName, bool showTaskbarIcon = true);
	~CCSBTEZombieDNA();
	void SetDNA(int id, bool bEnabled);

protected:
	void OnCommand(const char *command);
	void ApplySettings();
	void UpdateLabel();

private:
	CBinkPanel *m_pBackground;
	CCSBTEZombieDNAButton *m_pDNAButton[19];
	Button *m_pCancelButton;
	Label *m_pCountLabel;

	unsigned long m_bitsDNA;

	static const int m_iDNAButtonPos[19][2];
};
