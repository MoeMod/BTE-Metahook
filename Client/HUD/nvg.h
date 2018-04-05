#pragma once

class CHudNVG : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Reset(void);
	void Draw(float time);
	void DrawBackground(void);
	BOOL GetStatus(void);
	void ZM_OffNightVision(void);
	void ZM_OpenNightVision(void);

public:
	int MsgFunc_NVGToggle(const char *pszName, int iSize, void *pbuf);
	void UserCmd_NVGAdjustDown(void);
	void UserCmd_NVGAdjustUp(void);

public:
	BOOL m_bOn;
	bool g_bZBNightVisionOn;

private:
	int m_iFadeType;
	float m_fFade;
	int m_iR, m_iG, m_iB;
	float m_flAlpha;
};

CHudNVG &HudNVG();