#pragma once

class HudBuffClassDamagePanel;

class CHudBuffClassDamage : public CHudBase
{
public:
	CHudBuffClassDamage() : m_pPanel(nullptr){}
	void Init(void);
	void VidInit(void);
	void Draw(float flTime){};
	void Reset(void);
	void UpdateDamage(int iDamage);

private:
	int m_iLastDamage, m_iTotalDamage;
	HudBuffClassDamagePanel *m_pPanel; // auto delete by vgui2.dll, no need destructor
};

CHudBuffClassDamage &HudBuffClassDamage();