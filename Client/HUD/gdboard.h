#pragma once

class CHudGunDeathBoard : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	int m_iY;
	int m_iRank[3], m_iFrag[33];
};

CHudGunDeathBoard &HudGunDeathBoard();