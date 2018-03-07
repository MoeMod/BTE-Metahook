#pragma once

class CHudRespawnBar : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);

public:
	void SetStatus(BOOL bOn)
	{
		m_bOn = bOn;
	}
	void SetTime(float flTime)
	{
		m_flShowTime = flTime;
	}
	void SetStartTime(float flTime)
	{
		m_flStartTime = flTime;
	}

private:
	BOOL m_bOn;
	float m_flShowTime, m_flStartTime;

	int m_iBG, m_iBar;
};

CHudRespawnBar &HudRespawnBar();