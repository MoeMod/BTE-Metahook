#ifndef BUYPRESETPANELBUTTON_H
#define BUYPRESETPANELBUTTON_H

#ifdef _WIN32
#pragma once
#endif

#include <vgui/IScheme.h>
#include <vgui_controls/Button.h>
#include <vgui/KeyCode.h>
#include <filesystem.h>
#include "shared_util.h"

class BuyPresetImagePanel;

class BuyPresetButton : public vgui::Button
{
	typedef vgui::Button BaseClass;

public:
	BuyPresetButton(Panel *parent, const char *panelName);
	~BuyPresetButton(void);

public:
	void ApplySettings(KeyValues *resourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout(void);
	void DrawBox(int x, int y, int wide, int tall, Color color, float normalizedAlpha, bool hollow);
	void Paint(void);
	void PaintBackground(void);
	void FireActionSignal(void);

public:
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);

public:
	void GetText(char *textOut, int bufferLen);
	void GetText(wchar_t *textOut, int bufLenInBytes);
	void SetHotkey(wchar_t ch);
	void SetText(const wchar_t *wtext);
	void SetText(const char *text);

public:
	void ClearWeapons(void);
	void SetPrimaryWeapon(vgui::IImage *image);
	void SetSecondaryWeapon(vgui::IImage *image);
	void SetKnifeWeapon(vgui::IImage *image);

	void SetPrimaryWeapon(const char *name);
	void SetSecondaryWeapon(const char *name);
	void SetKnifeWeapon(const char *name);

protected:
	BuyPresetImagePanel *m_pImagePanel;

	vgui::IImage *m_pKeyboard;
	vgui::IImage *m_pBlankSlot;
	vgui::IImage *m_pArmed;
	vgui::IImage *m_pSelect;

	wchar_t *m_pFullText;
	wchar_t *m_pText;
	wchar_t m_cHotkey;
	int m_iKeyOffset;
	int m_iKeySize;

	char *m_pSetCommand;
	vgui::MouseCode m_iMousePressed;
};

#endif