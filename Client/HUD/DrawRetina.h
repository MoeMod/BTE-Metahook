#pragma once

class CHudRetina : public CHudBase
{
public:
	void Init(void);
	void VidInit(void);
	void Draw(float time);
	void Reset(void);
	void ParseData(int Index, int iDraw, int iType, int iFlash, int iChan, float fTime);
	void ParseData(char *pszName, int iDraw, int iType, int iFlash, int iChan, float fTime);

private:
	int m_iRetina[8];
	int m_iRetinaDraw[8];
	int m_iRetinaFull[8];
	int m_iRetinaFlash[8];
	float m_flRetinaFinishTime[8];
	int m_iRetinaAlpha[8];
};

CHudRetina &HudRetina();