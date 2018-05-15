#ifndef WEAPONIMAGEPANEL_H
#define WEAPONIMAGEPANEL_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/KeyCode.h>
#include <filesystem.h>
#include "shared_util.h"

#include "WeaponManager.h"

class WeaponImagePanel : public vgui::ImagePanel
{
private:
	typedef vgui::ImagePanel BaseClass;
public:
	WeaponImagePanel(Panel *parent, const char *name);

	virtual void PaintBackground() override;

	void SetWeapon(const char *weapon);
	void SetWeapon(nullptr_t);
private:
	virtual void SetImage(vgui::IImage *image) override { return BaseClass::SetImage(image); }
	virtual void SetImage(const char *imageName) override { return BaseClass::SetImage(imageName); }

	bool m_bBanned;
	vgui::IImage *m_pBannedImage;
};

#endif