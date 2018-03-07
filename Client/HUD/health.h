#pragma once
class CHudHealth : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);
	void Think(void);
	int MsgFunc_HealthExtra(const char * pszName, int iSize, void * pbuf);

public:
	void DrawHealth(float time);
	void DrawHealthExtra(float time);
	void DrawArmor(float time);

public:
	int m_iHealthFlags, m_iArmorFlags;
	int m_iHealth, m_iArmor, m_iArmorType;
	int m_iHealthExtra;
	float m_flHealthFade, m_flArmorFade;

private:
	int m_iHealthIcon, m_iHealthExtraIcon, m_iArmorIcon[2], m_iArmorFullIcon[2];
};

CHudHealth &HudHealth();
