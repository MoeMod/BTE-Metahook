#pragma once

#define MAX_ALARM_TYPES 34

enum AlarmType
{
	ALARM_HEADSHOT = 0,	// Killed a player and the last hitgroup is the head
	ALARM_GRENADE,		// Killed a player with grenade
	ALARM_KNIFE,		// Killed a player with knife
	ALARM_1KILL,		// Normally killed a player
	ALARM_2KILL,		// 2 combo
	ALARM_3KILL,		// 3 combo
	ALARM_4KILL,		// 4 combo
	ALARM_EXCELLENT,	// 5 combo but didn't die
	ALARM_INCREDIBLE,	// 10 combo but didn't die
	ALARM_CRAZY,		// 15 combo but didn't die
	ALARM_CANTBELIEVE,	// 20 combo but didn't die
	ALARM_OUTOFWORLD,	// 25 combo but didn't die
	ALARM_FIRSTBLOOD,	// Be the first killer in a round
	ALARM_WELCOME,	// Killed after 3 rounds that the player didn't killed any enemies or after 5 round-respawns
	ALARM_COMEBACK,	// Killed after 2 rounds that the player didn't killed any enemies or after 3 round-respawns
	ALARM_KINGMURDER,	// Killed the first ranker in the enemy's team
	ALARM_BACKMARKER,	// Killed the last ranker in the enemy's team
	ALARM_PAYBACK,		// Re-killed the player who killed me
	ALARM_SAVIOR,		// Killed the enemy who is attacking the teammate
	ALARM_THELAST,		// Did the last kill, just clear the round
	ALARM_ASSIST,		// Took damage to the enemy but hasn't become the killer
	ALARM_SURPPORTER,	// Assisted 5 times but didn't die
	ALARM_KINGMAKER,	// Helped the first ranker in the team to kill the enemy
	ALARM_INVISIBLEHAND,	// Helped the teammate to kill the first ranker in the enemy's team but hasn't become the killer
	ALARM_C4PLANT,		// Planted the bomb
	ALARM_C4PLANTBLOCK,	// Kill the player who is planting the bomb
	ALARM_C4DEFUSEBLOCK,	//Kill the player who is defusing the bomb
	ALARM_C4MANKILL,	// Kill the player who has the bomb
	ALARM_LASTSOLDIER,	// Be the last one (except zombies) after a kill event happened
	ALARM_INFECTOR,	// Infected 8 humans in a round as a zombie
	ALARM_LIBERATOR,	// Killed 6 zombies in a round as a human
	ALARM_ALLROUND,	// Killed 2 or more than 2 zombies as a human and infected 3 or more than 3 humans as a zombie
	ALARM_ZOMBIETANKER,
	ALARM_ZOMBIEBOMB
};

class CHudAlarm : CHudBase
{
public:
	void Init(void);
	void Draw(float time);
	int MsgFunc_StatusAlarm(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_Assist(const char *pszName, int iSize, void *pbuf);
	inline void PlayAlarmSound(void);
	void AddAlarm(AlarmType iAlarm, bool bNeedCheck = true);
	inline int GetAssistTimes(void)
	{
		int iTimes = 0;
		for (int i = ALARM_ASSIST; i <= ALARM_INVISIBLEHAND; i++)
		{
			iTimes += m_iHappenedTimes[i];
		}
		return iTimes;
	}
	inline int GetHappenedTimes(AlarmType iAlarm)
	{
		return m_iHappenedTimes[iAlarm];
	}
	inline void ClearAllHappenedTimes(void)
	{
		for (int i = 0; i < MAX_ALARM_TYPES; i++)
		{
			m_iHappenedTimes[i] = 0;
		}
	}
	inline void SetClearAssistTimes(void)
	{
		for (int i = ALARM_ASSIST; i <= ALARM_INVISIBLEHAND; i++)
		{
			m_iHappenedTimes[i] = 0;
		}
	}
	inline bool ClearSmallerIcon(AlarmType iAlarm)
	{
		/*
		if (iAlarm <= ALARM_1KILL) return false;
		if (iAlarm > ALARM_OUTOFWORLD) return false;
		for (int i = iAlarm - 1; i >= ALARM_1KILL; i--)
		{
			while (m_bAlarms[i])
				m_bAlarms[i]=false;
		}*/
		return false;
	}

	inline unsigned long GetAlarm(int x)
	{
		return m_bAlarms[x];
	}

	inline unsigned long GetPrepareAlarmBits(int x)
	{
		return m_bPrePareAlarms[x];
	}

	inline void RemovePrepareAlarmBits(int iAlarm)
	{
		m_bPrePareAlarms[iAlarm]=false;
	}

	inline int CheckFree(void)
	{
		return m_iSlotFree;
	}

	inline AlarmType GetPrivate(int iAlarm)
	{
		return static_cast <AlarmType>(iAlarm);
	}

	inline int GetNext(void)
	{
		m_bDrawBackground = false;
		for (int i = 0; i < MAX_ALARM_TYPES; i++)
		{
			if (m_bAlarms[m_iOrder[i]])
			{
				if (m_iOrder[i] == m_iCurAlarm) continue;
				while (m_bAlarms[m_iOrder[i]])
					m_bAlarms[m_iOrder[i]]=false;
				m_iCurAlarm = m_iOrder[i];
				PlayAlarmSound();
				return 1;
			}
		}
		m_iCurAlarm = -1;
		//m_bitsAlarms = 0;
		memset(m_bAlarms, 0, sizeof(m_bAlarms));
		m_iSlotFree = 1;
		return 0;
	}
private:
	int m_iCurAlarm;
	int m_iSlotFree;
	int m_iAlpha;
	int m_bDrawBackground;
	//unsigned m_bitsAlarms, m_bitsPrePareAlarms;
	bool m_bAlarms[MAX_ALARM_TYPES], m_bPrePareAlarms[MAX_ALARM_TYPES];
	static int m_iOrder[MAX_ALARM_TYPES];
	static const char *m_szSound[MAX_ALARM_TYPES];
	static const char *m_szAlarmNames[MAX_ALARM_TYPES];
	int m_iFontBG;
	int m_iAlarmBG;
	int m_iAlarms[MAX_ALARM_TYPES];
	int m_iHappenedTimes[MAX_ALARM_TYPES];
	float m_flEndDraw;
	wchar_t m_lpAlarms[MAX_ALARM_TYPES][512];
};

CHudAlarm &HudAlarm();