#pragma once
#include "BinkPlayer.h"

class CHudZBSelect : public CHudBase
{
public:
	void Init();
	void VidInit();
	void Draw(float flTime);
	void SetDrawTime(float flTime);
	void SetCount(int count);
	void SetIcon(int slot, const char *name);
	void AddZombie(const char *name);
	void Select(int i);

public:
	bool m_bCanDraw;

private:
	CBinkPlayer m_Bink;

	int m_iIcon[20];
	wrect_t m_rcIcon[20];
	int m_iSlotCount;

	float m_flTimeEnd;

	int m_iTimeRemaining;

	wchar_t m_szTimeRemaining[64];

};

CHudZBSelect &HudZBSelect();