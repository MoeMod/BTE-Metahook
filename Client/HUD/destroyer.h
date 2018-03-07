#pragma once

#include "bink/bink.h"

class CHudDestroyerSniperScope : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);
	void Run(void);
	void End(void);
	~CHudDestroyerSniperScope();

private:
	int DrawDestroyerSniperScopeNumbers(int x, int y, int iFlags, int iNumber, int r, int g, int b);
	int CalculateDistance(void);
	void SetCharge(BOOL bAimEnemy);
	void Finish(void);

private:
	int m_iCurrentAim;
	uint m_iFrame;
	float m_flStartTime;
	BOOL m_bStartBink;

private:
	HBINK m_hBink;
	BYTE *m_pMem;
	int m_iBink;
	int m_iAim01, m_iAim02;
	int m_iFrame01, m_iFrame02;
	int m_iRange;
	int m_iNumberZero, m_iMeter;
	int m_iTexture;
	wrect_t m_rcMeter;
};

CHudDestroyerSniperScope &HudDestroyerSniperScope();