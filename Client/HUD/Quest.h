#pragma once

class CHudQuest : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);
public:
	void Add(char *pNpcName, char *pNpcPic, char *pMission);

private:
	int m_iBG, m_iNPC, m_iComplete;
	int m_iStartPos;
	BOOL m_bDraw;
	char m_szNPCName[32], m_szMission[32];
};

CHudQuest &HudQuest();