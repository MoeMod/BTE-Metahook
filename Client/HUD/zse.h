#pragma once
#include "Retina.h"
class CHudZSEUI : public CHudBase
{
public:
	void Init();
	void VidInit();
	void Draw(float flTime);
	void SendData();

private:
	CHudRetinaObjects m_Retina[8];
};

CHudZSEUI &HudZSEUI();