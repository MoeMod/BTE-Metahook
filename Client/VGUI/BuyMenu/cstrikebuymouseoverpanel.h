#ifndef CSTRIKEBUYMOUSEOVERPANEL_H
#define CSTRIKEBUYMOUSEOVERPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui_controls/Button.h>
#include <vgui/KeyCode.h>
#include <filesystem.h>
#include "shared_util.h"

#include "game_controls/mouseoverpanelbutton.h"
#include "../newmouseoverpanelbutton.h"

class CSBuyMouseOverPanel : public NewMouseOverPanel
{
	typedef NewMouseOverPanel BaseClass;

public:
	CSBuyMouseOverPanel(vgui::Panel *parent, const char *panelName);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme) override;
	virtual void PerformLayout(void) override;
	void UpdateWeapon(const char *weapon = "");

public:
	vgui::Label *infolabel;

	vgui::Label *pricelabel;
	vgui::Label *calibrelabel;
	vgui::Label *clipcapacitylabel;
	vgui::Label *rateoffirelabel;
	vgui::Label *weightloadedlabel;

	vgui::Label *price;
	vgui::Label *calibre;
	vgui::Label *clipcapacity;
	vgui::Label *rateoffire;
	vgui::Label *weightempty;


	vgui::ImagePanel *imageBG;
	vgui::ImagePanel *classimage;
};

#endif