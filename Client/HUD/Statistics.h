#pragma once

class CHudStatistics : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);
	void OnThink(float flTime);

	void Open(void);
	void Close(void);
	void Save(void);
	void Load(void);

	void ResetText();

	void UserCmd_StatisticsOpen();

public:
	int m_iKill, m_iDeath, m_iHs, m_iShoots, m_iHit, m_iKnifeKill, m_iGrenadeKill;
	int m_iAnimStatus;
	bool m_bCanDraw;
	float m_flDie;

	cvar_t *m_pKilledShow;
	cvar_t *m_pRoundEndShow;

private:
	bool m_bSave;

	int m_iIcon[3];
	int m_iX, m_iY, m_iW, m_iH;
	int m_iY_Ofs;
	int m_iAlpha;
	float m_flAnimTime;

	int m_iKillSave, m_iDeathSave, m_iHsSave, m_iShootsSave, m_iHitSave, m_iKnifeKillSave, m_iGrenadeKillSave;

	wchar_t m_szTitle[64];
	wchar_t m_szKill[2][64];
	wchar_t m_szDeath[2][64];
	wchar_t m_szHs[3][2][64];
	wchar_t m_szShoots[2][64];
	wchar_t m_szHit[2][64];

	wchar_t m_szTip[64];
};

CHudStatistics &HudStatistics();