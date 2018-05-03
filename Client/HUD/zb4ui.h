#pragma once

#define ZB4_ADD_MEMBER_TO_BASE(x, y) ::CHudZB4UI##x y;

enum
{
	Z4_NORMAL = 0,
	Z4_LIGHT,
	Z4_HEAVY,
	Z4_HIDE,
	Z4_HUMPBACK
};

#include "BinkPlayer.h"
#include "Retina.h"
#include "ZombieSelect.h"

class CHudZB4UIBase
{
public:
	virtual void Init(void) { return; }
	virtual void VidInit(void) { return; }
	virtual void Redraw(void) { return; }

public:
	void LoadTexture(char *name, int &id, wrect_t &rc);
	void DrawTexture(int id, wrect_t rc, int x, int y);
	void BuildNumberRC(wrect_t *rgrc, int w, int h);
	void DrawNumbers(int n, int x, int y, int from_right, float size = 1.0f);
	void DrawNumber(int n, int x, int y, float size = 1.0f);
	int GetNumWidth(int n, int w, float size);

	int m_iNumber;
	wrect_t m_rcNumber[10];

};

class CHudZB4UIScoreBoard : public CHudZB4UIBase
{
public:
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Redraw(void);

public:
	void DrawScoreBoard(void);
	void DrawScore(void);
	void SetTeam(int team);
	void SetDamage(int damage);
	void SetLastScore() { m_iLastScore = g_PlayerExtraInfo[gEngfuncs.GetLocalPlayer()->index].frags; }

private:
	int m_iScroreBorad;
	wrect_t m_rcScroreBorad;
	int m_iDamageBorad;
	wrect_t m_rcDamageBorad;

	int m_iTeamIcon[2];
	wrect_t m_rcTeamIcon[2];

	int m_iTeam;
	int m_iDamage;
	int m_iLastScore;

};

class CHudZB4UIIcon : public CHudZB4UIBase
{
public:
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Redraw(void);
public:
	void SetPower(int power) { m_iPower = power; }
	void AddIcon(int slot, int id, int status);
	void SetTeam(int team) { m_iTeam = team; }

private:
	int m_iIcon[12];
	wrect_t m_rcIcon[12];

	int m_iStatus[12];
	int m_iId[12];
	int m_iAlpha[12];
	float m_flFlashTime[12];
	int m_iFlash[12];

	int m_iPower;
	int m_iTeam;
};

class CHudZB4UITip : public CHudZB4UIBase
{
public:
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Redraw(float flTime);
public:
	void SetTimeEnd(float flTime);
	void LoadText(char *text);
	void ShowTip(int type);
	void GetFontColor(int team, int &r, int &g, int &b, int &a);
	void SetNightTime(int start, int end)
	{
		m_iNightStart = start;
		m_iNightEnd = end;
	}
	void ZombieIconsCheck(float flTime);

private:
	int m_iBg[2];
	wrect_t m_rcBg[2];

	int m_iTeam;
	float m_flTimeEnd;
	float m_flTimeStart;

	float m_flDisplayEnd;
	int m_iCount;

	int m_iNightStart;
	int m_iNightEnd;

	wchar_t m_szText[128];
};

class CHudZB4UIPower : public CHudZB4UIBase
{
public:
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Redraw(void);

public:
	void SetPower(int power, float flashtime);
	void SetZbSkill(int text, int usage)
	{
		m_iText = text;
		m_iSkillUsage = usage;
	}

private:
	int m_iPowerBackground;
	wrect_t m_rcPowerBackground;

	int m_iPowerLine;
	wrect_t m_rcPowerLine;

	int m_iPowerLine2;
	wrect_t m_rcPowerLine2;

	int m_iPowerLeft2;
	wrect_t m_rcPowerLeft2;

	int m_iPowerPoint;
	wrect_t m_rcPowerPoint;

	int m_iPowerRightClick[2];
	wrect_t m_rcRightClick[2];

	int m_iKey[2];
	wrect_t m_rcKey[2];

	int m_iPower[2];
	int m_iPower2[2];
	int m_iPowerServer;

	int m_iUseTimesOld;
	int m_iSkillUsage;
	int m_iText;

	float m_flPowerStartRefreshTime[2];
	float m_flPowerEndRefreshTime[2];

	float m_flFlashTime;
	int m_iAlpha;

	wchar_t m_szDrawText[64];
	wchar_t m_szDrawText2[64];
	float m_flTextFlashStart;
	int m_iTextFlash;

};

class CHudZB4UIRadar : public CHudZB4UIBase
{
public:
	virtual void VidInit(void);
	virtual void Redraw(void);
public:
	void DrawBackground(void);

private:
	int m_iBackground;
	int m_iCover;
};

class CHudZB4UI : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);

public:
	ZB4_ADD_MEMBER_TO_BASE(ScoreBoard, m_ScoreBoard);
	ZB4_ADD_MEMBER_TO_BASE(Radar, m_Radar);
	ZB4_ADD_MEMBER_TO_BASE(Power, m_Power);
	ZB4_ADD_MEMBER_TO_BASE(Icon, m_Icon);
	ZB4_ADD_MEMBER_TO_BASE(Tip, m_Tip);

	CBinkPlayer m_Bink;
	CHudRetinaObjects m_Retina[8];

private:

};

CHudZB4UI &HudZombieMod4();