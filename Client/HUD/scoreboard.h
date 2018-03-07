#pragma once

class CHudScoreBoard : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	int DrawHudsNumber(int x, int y, int iNumber, int r, int g, int b, int length);
	int DrawHudNumber(int x, int y, int iNumber, int r, int g, int b, int length);

public:
	int m_iLeft, m_iCenter, m_iRight, m_iLeft2, m_iRight2, m_iMode;

private:

	int m_SBHUD_number_0, m_iSBFontWidth, m_iSBFontHeight;
	int m_SBHUD_snumber_0, m_iSBsFontWidth, m_iSBsFontHeight;

	int m_iBgNone, m_iBgTd;
	int m_iTextCT, m_iTextTR, m_iTextRound, m_iTextKill;
	int m_iTextHM, m_iTextZB;
	int m_iText1st;
};

CHudScoreBoard &HudScoreBoard();