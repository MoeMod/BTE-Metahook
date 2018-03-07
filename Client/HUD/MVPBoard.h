#pragma once

class CHudMVPBoard : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float fTime);
	inline void SetVisible(bool bVisible) { m_bVisible = bVisible; }
	inline bool GetVisible(void) { return m_bVisible; }
	void DrawNormal(void);
	void DrawZB(void);
	int MsgFunc_MVPBoard(const char *pszName, int iSize, void *pbuf);
	int MsgFunc_MVP(const char *pszName, int iSize, void *pbuf);
	int CheckWinTeam(int iTeam, int iPlayer);

	void OnPlayerSpawn(void);
private:
	bool m_bVisible;
	int m_hBoard_Blue, m_hBoard_Red, m_hBoard_BG;
	int m_hBoard;
	int m_iTopFive[3][5];
	int m_iScore[3][5];
	wchar_t m_lpWinText[512];
	wchar_t m_lpMVPText[512];
	wchar_t m_lpTime[512];
	wchar_t m_lpScore[3][5][512];
	wchar_t m_lpTopFive[3][5][512];
	int m_iColor[2][3];
};

CHudMVPBoard &HudMVPBoard();