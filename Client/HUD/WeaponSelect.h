#pragma once

#include "BinkPlayer.h"

class CHudWeaponSelect : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);

public:
	void SetDrawTime(float flTime);
	void SetIcon(int iSlot, char *name);
	void Select(int item);

public:
	bool m_bCanDraw;

private:
	CBinkPlayer m_Bink;

	int m_iIcon[5];
	bool m_bDraw[5];
	wrect_t m_rcIcon[5];

	float m_flTimeEnd;

	int m_iTimeRemaining;

	wchar_t m_szTimeRemaining[64];
};

CHudWeaponSelect &HudWeaponSelect();