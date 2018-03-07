#pragma once

class CHudZBCounter : public CHudBase
{
public:
	void Init();
	void VidInit();
	void Draw(float flTime);
	void SetStartTime(float flTime);
	bool CanDraw();

private:
	int m_iStartTime;
	int m_iCurTime;
};

CHudZBCounter &HudZBCounter();