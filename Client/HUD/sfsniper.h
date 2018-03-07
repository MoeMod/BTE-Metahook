#pragma once

#include "bink/bink.h"

class CHudSfsniperScope : public CHudBase
{
public:
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Draw(float flTime);

	void Run(void);
	void End(void);
	~CHudSfsniperScope();

private:
	BYTE *m_pSniperScopeBinkMem;
	HBINK m_hSniperScopeBinkHandle;
	uint m_iScopeFrame;
	int m_iSniperScopeBinkTextureIndex;
	float m_flSniperScopePlayStartTime;
	int m_iSniperScopeBinkColor[3];
};

CHudSfsniperScope &HudSfsniperScope();