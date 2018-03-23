#include "base.h"
#include "hud.h"
#include "BaseUI.h"
#include "message.h"
#include "parsemsg.h"
#include "util.h"

#include "Client/WeaponManager.h"

#include "Client/HUD/FontText.h"
#include "deathinfo.h"

#include <iostream>
#include <algorithm>
#include <string>
#include <functional>

static CHudDeathInfo g_HudDeathInfo;
CHudDeathInfo &HudDeathInfo()
{
	return g_HudDeathInfo;
}

const char *CHudDeathInfo::m_pHitgroupName[5] = {
	"#CSO_Head",
	"#CSO_Chest",
	"#CSO_Stomach",
	"#CSO_Arm",
	"#CSO_Leg"
};

void CHudDeathInfo::Init(void)
{
	m_iFlags |= HUD_ACTIVE;

	gEngfuncs.pfnHookUserMsg("DeathInfo", CHudDeathInfo::MsgFunc_DeathInfo);
}

void CHudDeathInfo::VidInit(void)
{
	
}

void CHudDeathInfo::Draw(float flTime)
{
	
}

int CHudDeathInfo::MsgFunc_DeathInfo(const char * pszName, int iSize, void * pbuf)
{
	std::wstring strMessage;
	wchar_t strBuffer[512];
	static DrawFontTextItem rgTempDrawText;

	BEGIN_READ(pbuf, iSize);
	int iType = READ_BYTE();
	if (iType == 1)
	{
		unsigned iEnemy = READ_BYTE();
		unsigned iDist = READ_BYTE();
		char *pszWeaponName = READ_STRING();

		unsigned iTotalDamage1 = READ_SHORT();

		hud_player_info_t hEnemyInfo;
		gEngfuncs.pfnGetPlayerInfo(iEnemy, &hEnemyInfo);
		char *pszEnemyName = hEnemyInfo.name;
		if (!pszEnemyName)
			pszEnemyName = "";

		/*
		"CSO_EnemyDeathMsg"		"µÐÈËËÀÍö£º%s
		¾àÀë£º%dm
		ÉËº¦(Ê¹ÓÃ%s)£º%d \n"
		*/

		swprintf(strBuffer, vgui::localize()->Find("#CSO_EnemyDeathMsg"), UTF8ToUnicode(pszEnemyName), iDist, GetWeaponNameFormat(pszWeaponName), iTotalDamage1);
		strMessage += strBuffer;

		for (int i = 0; i < 5; i++)
		{
			unsigned iDamage = READ_SHORT();
			unsigned iHitCount = READ_BYTE();
			if (iDamage && iHitCount)
			{
				swprintf(strBuffer, L"\n%s %d(%d%s)", vgui::localize()->Find(CHudDeathInfo::m_pHitgroupName[i]), iDamage, iHitCount, vgui::localize()->Find("#CSO_AmmoCount"));
				strMessage += strBuffer;
			}
		}

		rgTempDrawText.iCenter = 0;
		rgTempDrawText.x = 0.55 * ScreenWidth;
		rgTempDrawText.y = 0.65 * ScreenHeight;
		rgTempDrawText.color.r = 168;
		rgTempDrawText.color.g = 211;
		rgTempDrawText.color.b = 253;
	}
	else if (iType == 2)
	{
		unsigned iEnemy = READ_BYTE();
		unsigned iDist = READ_BYTE();
		char *pszWeaponName = READ_STRING();
	
		unsigned iTotalDamage1 = READ_SHORT();
		
		hud_player_info_t hEnemyInfo;
		gEngfuncs.pfnGetPlayerInfo(iEnemy, &hEnemyInfo);
		char *pszEnemyName = hEnemyInfo.name;
		if (!pszEnemyName)
			pszEnemyName = "";
		/*
		"CSO_DeathMsg"		"±»%s»÷±Ð
		¾àÀë£º%dm
		ÉËº¦(Ê¹ÓÃ%s)£º%d \n"
		*/
		swprintf(strBuffer, vgui::localize()->Find("#CSO_DeathMsg"), UTF8ToUnicode(pszEnemyName), iDist, GetWeaponNameFormat(pszWeaponName), iTotalDamage1);
		strMessage += strBuffer;
		

		for (int i = 0; i < 4; i++)
		{
			unsigned iDamage = READ_SHORT();
			unsigned iHitCount = READ_BYTE();
			if (iDamage && iHitCount)
			{
				swprintf(strBuffer, L"\n%s %u(%u%s)", vgui::localize()->Find(CHudDeathInfo::m_pHitgroupName[i]), iDamage, iHitCount, vgui::localize()->Find("#CSO_AmmoCount"));
				strMessage += strBuffer;
			}
		}

		unsigned iTotalDamage2 = READ_SHORT();

		for (int i = 0; i < 4; i++)
		{
			int iDamage = READ_SHORT();
			int iHitCount = READ_BYTE();
			if (iDamage && iHitCount)
			{
				swprintf(strBuffer, L"\n%s %u(%u%s)", vgui::localize()->Find(CHudDeathInfo::m_pHitgroupName[i]), iDamage, iHitCount, vgui::localize()->Find("#CSO_AmmoCount"));
				strMessage += strBuffer;
			}
		}


		unsigned iHP = READ_SHORT();
		unsigned iAP = READ_SHORT();
		swprintf(strBuffer, vgui::localize()->Find("#CSO_HpAp"), iHP, iAP);
		strMessage += strBuffer;

		rgTempDrawText.iCenter = 0;
		rgTempDrawText.x = 0.4 * ScreenWidth;
		rgTempDrawText.y = 0.6 * ScreenHeight;
		rgTempDrawText.color.r = 239;
		rgTempDrawText.color.g = 155;
		rgTempDrawText.color.b = 155;
	}

	/*std::wstring::size_type i = 0;
	while ((i = strMessage.find(L'\r', i)) != std::wstring::npos)
	strMessage.erase(i);*/

	strMessage.erase(std::remove(strMessage.begin(), strMessage.end(), '\r'), strMessage.end());

	rgTempDrawText.iScale = 14;
	rgTempDrawText.flDisplayTime = (float)cl.time + 6.0f;
	rgTempDrawText.flStartTime = cl.time;
	rgTempDrawText.fFadeTime = 0.0;
	rgTempDrawText.iFillBg = 0;
	rgTempDrawText.iChanne = 2;
	Q_strcpy(rgTempDrawText.szText, UnicodeToUTF8(strMessage.c_str()));

	Hud().m_FontText.AddElement(rgTempDrawText);
	return 1;
}