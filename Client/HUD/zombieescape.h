#pragma once

class CHudZombieEscape : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	int LoadPoint(char *pName);
	void ResetUI(void);

private:
	int m_iPoint[20][3], m_iPlayerLastCount[33];
	float m_flZEPoint[20][3], m_flZECheck;

	int m_iHM, m_iZB, m_iFlag, m_iSelf, m_iBar;
};

CHudZombieEscape &HudZombieEscape();