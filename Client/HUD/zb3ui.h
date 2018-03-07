#pragma once

#include "vgui_controls/Label.h"

enum
{
	ZB3DATA_HM_MORALE = 1,
	ZB3DATA_ZB_LEVEL,
	ZB3DATA_ZB_RAGE,
	ZB3DATA_HM_MORALE_MAX,
	ZB3DATA_PASS,
	ZB3DATA_RETINA,
	ZB3DATA_RETINASTOP,
	ZB3DATA_CLEARUI,
	ZB3DATA_TOPUI,
	ZB3DATA_BECOME_ZB,
	ZB3DATA_RESETSKILL,
	ZB3DATA_USESKILL,
};

enum
{
	TANK_ZB = 0,
	SPEED_ZB,
	HEAVY_ZB,
	HEAL_ZB,
	PC_ZB,
	DEIMOS_ZB,
	DEIMOS2_ZB,
	WITCH_ZB,
	UNDERTAKER_ZB,
	CHINA_ZB,
	BOMMER_ZB,
	RESIDENT_ZB,
};

class CHudZombieMod3 : public CHudBase
{
	struct UIElement
	{
		int r, g, b, a;
		float flStartDisplayTime;
		float flEndDisplayTime;
		int iType;
		int iTextureID;
		int iWidth;
		int iHeight;
		char *szKey;
	};

public:
	CHudZombieMod3() : m_pLabelMorale(nullptr) {}
	void Init(void);
	void VidInit(void);
	void Draw(float time);
	//void Think(void);

public:
	void DrawRetina(float time);
	void DrawHeroIcon(float time);
	void DrawHumanMorale(float time);
	void DrawZombieRage(float time);
	void DrawSkillBoard(float time);
	void DrawBinkMenu(float time);

public:
	void CheckTeam(int team);
	void ParseData(int DataType, int iData);
	void ParseIcon(char *pszName, int iSlot, int iType, int iData);
	void AddHeadIcon(int idx, char *szSprite, float time, float flOffsetZ);

public:
	void FindHumanInRadius(void);
	bool CheckSkillKey(int eventcode, int keynum, const char *pszCurrentBinding);

public:
	int m_iSupplyBox, m_iRadarItem, m_iZombieTrap;

public:
	int m_iNVG;

private:
	UIElement m_UISlot[5];
	int m_iClass, m_iRage, m_iMoraleInt, m_iAdditionMorale, m_iMoraleUp;
	float m_flMorale, m_flAdditionMorale, m_flRagePercent, m_flRageTimer;
	int m_iRageFrame;
	int m_iSkillType;
	BOOL m_bZBMenuVisible;

private:
	int m_iMoraleIcon, m_iMoraleLevel, m_iMoraleEffect;
	int m_iRageLevel, m_iRageBG, m_iRageIndex;
	//int m_iSkullSlotBG, m_iKeySlotBG;
	int m_iRetina[10], m_iRetinaDraw, m_iRetinaClass;
	float m_flRetinaStart;
	HSPRITE m_hRage[7];
	vgui::Label *m_pLabelMorale;

private:
	int m_iWeaponTexture[8], m_iCancelTexture;
	int m_iBinkTextureID;
	
};

CHudZombieMod3 &HudZombieMod3();