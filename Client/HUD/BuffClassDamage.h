#pragma once

class CHudBuffClassDamage : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float flTime);
	void Reset(void);
	void ShutDown(void);
	void UpdateDamage(int iDamage);

protected:
	//int CalcNumWidth(int x);
	void BuildNumberRC(wrect_t *rgrc, int w, int h);
	void DrawNumbers(int n, int x, int y, int from_right, float size = 1.0f);
	void DrawNumber(int n, int x, int y, float size = 1.0f);

private:
	bool m_bEnabled;
	int m_iLastDamage, m_iTotalDamage;
	int m_iTgaBG;
	//int m_iNumWidth[9];
	int m_iNumber;
	wrect_t m_rcNumber[10];
};

CHudBuffClassDamage &HudBuffClassDamage();