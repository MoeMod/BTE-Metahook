#pragma once

class CHudDeathInfo : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);
	static int MsgFunc_DeathInfo(const char * pszName, int iSize, void * pbuf);

	static const char *m_pHitgroupName[5];
};

CHudDeathInfo &HudDeathInfo();