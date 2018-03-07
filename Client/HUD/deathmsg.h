#pragma once


#define MAX_DEATHNOTICES	8
#define MAX_PLAYER_NAME_LENGTH 64
#define DEATHNOTICE_TOP		32

struct DeathNoticeItem {
	char szKiller[32 * 32 + 3];
	char szVictim[32];
	int iId;
	int iSuicide;
	int iNonPlayerKill;
	float flDisplayTime;
	int Killer;
	int Victim;
	int iHeadShot;
	int iLocal;
};

int DrawConsoleString(int x, int y, const char *string);
void GetConsoleStringSize(const char *string, int *width, int *height);
int ConsoleStringLen(const char *string);

class CHudDeathNotice : public CHudBase
{
public:
	CHudDeathNotice()
	{
		m_fCountResetTime[0] = 0.0;
		m_fCountResetTime[1] = 3;
		m_fCountResetTime[2] = 2.5;
		m_fCountResetTime[3] = 1.7;
		m_fCountResetTime[4] = 1.7;

		m_bFirstKill = 1;
		Init();
	};


	void Init(void);
	void VidInit(void);
	void Draw(float time);
	void OnPlayerSpawn(void);
	void OnRoundStart(void);

	int MsgFunc_DeathMsg(const char *pszName, int iSize, void *pbuf);

private:
	DeathNoticeItem m_rgDeathNoticeList[MAX_DEATHNOTICES + 1];
	int m_iDeathTga[2][3];
	int m_iKillCount;
	float m_fLastKillTime;
	int m_iTotalKills;
	int m_bFirstKill, m_iInfects, m_iKills, m_iDeaths, m_iRoundDidNotKill, m_bLastRoundKilled;
	float m_fCountResetTime[5];
};

int RankFirst(int iTeam);
int RankLast(int iTeam);

CHudDeathNotice &HudDeathNotice();