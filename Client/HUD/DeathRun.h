#pragma once

class CHudDeathRunUI : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	void ResetUI(void);
	void Start(void);
	void Stop(void);
	void ParseMessage(void);
	void InitUI(void);

private:
	void DrawNumber(int x, int y, int num);

private:
	int m_iMinutes, m_iSeconds, m_iMilliSeconds;
	int m_iRank[3], m_iMedalRank[3], m_iRecord[3], m_iDRID[3], m_iDRTime[3];
	int m_iBoard, m_iNumber, m_iColon;
	int m_iNumberWidth;
	float m_flDelta, m_flStartTime;
	int m_iReset, m_iStop;
	char m_szDRName[3][32], m_szDRTime[3][32];
};

CHudDeathRunUI &HudDeathRunUI();