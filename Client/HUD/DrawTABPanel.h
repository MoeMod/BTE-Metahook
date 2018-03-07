#pragma once

#include <metahook.h>
#include <com_model.h>

#define TAB_TEAM_T	1
#define TAB_TEAM_CT	2
#define TAB_SPEC	3
#define MAX_PLAYERS	32
struct team_info_t 
{
	short frags;
	short deaths;
	short ping;
	short packetloss;
	short players;
	int teamnumber;
};

extern team_info_t g_TeamInfo[MAX_PLAYERS + 1];
extern hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS+1];
extern int iValidPlayer[33];

cl_entity_t *GetPlayerByIndex(int index);

class CHudTABBoard : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);
	void Think(void);

	void ResetLevel(void);
	void ResetServerName(char *pszName);
	void ResetSpec(void);
	void ResetAlivePlayer(void);
	void DrawScore(bool bDivideTeam);
	void SetCanDraw(bool x) { m_bCanDraw = x; }
	bool GetCanDraw() { return m_bCanDraw; }

public:
	Vector2D m_vecOrigin;

	int m_iHeight;

	model_t *m_rgpPlayerDead[2];

	float m_flOffsetScale;
	int m_iSortedId[33];
	int m_iTeamScore[2];
	bool m_bValidPlayer[33];
private:
	wchar_t m_szServerName[32];
	wchar_t m_szGameMode[64];
	wchar_t m_szSpec[256];
	wchar_t m_szCT[32], m_szTR[32], m_szZB[32], m_szHM[32];
	wchar_t m_szBOT[32], m_szHost[32], m_szBomb[32], m_szHero[32], m_szZombie[32], m_szDead[32], m_szVIP[32];
	wchar_t m_szName[32], m_szKill[32], m_szDeath[32], m_szPing[32];
	wchar_t m_szAlivePlayer[2][128];

	int m_iX, m_iY, m_iW, m_iH;
	float m_flScale, m_flScale2;



	float m_flNextThink;

	int m_iSpec;
	int m_iPlayer[2][2];

	int m_iBuffIcon;
	bool m_bLargeFont;
	bool m_bHostOwnBuff;
	bool m_bCanDraw;

public:
	void SetHostOwnBuff(bool x) { m_bHostOwnBuff = x; }
};

CHudTABBoard &HudTABBoard();