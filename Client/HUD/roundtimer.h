class CHudRoundTimer : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);
	void Think(float flTime);
	void SetTimeEnd(float flTime);
public:
	int m_timer;
private:
	float m_flTimeEnd;
	int m_iIcon;
	wrect_t m_rcIcon;

	int m_iAlpha;
	int m_R;
	int m_G;
	int m_B;

	float m_flNextToggle;
	bool m_bPanicColor;
};

CHudRoundTimer &HudRoundTimer();