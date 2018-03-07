#pragma once

#include "bink/bink.h"

class CHudBinkPlayer : public CHudBase
{
public:
	void Init(void);
	void Draw(float time);
	void SetPos(int X, int Y, int Center);
	void SetColor(int R, int G, int B);
	void BinkInstance(char *pszName, int iStop, int iLoop);
private:
	int m_iX, m_iY, m_iCenter;
	int m_iColor[3];
	uint m_iFrame;
	int m_iBinkTextureID;
	float m_flStartTime;
	BOOL m_bBinkLoop;
	BYTE *m_pBinkMem;
	HBINK m_hBink;
};

CHudBinkPlayer &HudBinkPlayer();