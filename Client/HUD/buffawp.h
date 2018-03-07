#pragma once

class CHudBuffAWPSniperScope : public CHudBase
{
public:
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Draw(float flTime);

	void Run(void);
	void End(void);

private:
	int m_iTgaBG, m_iTgaLight[3], m_iTgaBar, m_iTexture;
	float m_flStartTime;
};

CHudBuffAWPSniperScope &HudBuffAWPSniperScope();