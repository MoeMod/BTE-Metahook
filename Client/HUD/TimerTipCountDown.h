#pragma once

class HudTimerTipCountDownPanel;

class CHudTimerTipCountDown : public CHudBase
{
public:
	CHudTimerTipCountDown() : m_pPanel(nullptr){}
	virtual void Init(void);
	virtual void VidInit(void);
	virtual void Draw(float flTime);
	virtual void Reset(void);

	int MsgFunc_TimerTip(const char *pszName, int iSize, void *pbuf);

	void Activate(const char *name, int time);

private:
	float m_flTimeEnd;
	bool m_bEnabled;
	HudTimerTipCountDownPanel *m_pPanel; // auto delete by vgui2.dll, no need destructor
};

CHudTimerTipCountDown &HudTimerTipCountDown();