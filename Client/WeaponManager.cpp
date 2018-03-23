
#include "hud.h"
#include "vgui_controls/Controls.h"
#include "weapons.h"
#include "util.h"
#include "common.h"

#include "WeaponManager.h"
#include "Client/HUD/DrawTGA.h"

CWeaponManager &WeaponManager()
{
	static CWeaponManager x;
	return x;
}

char *AliasWeaponName(char *name)
{
	if (!stricmp(name, "vest"))
		return "kevlar";
	if (!stricmp(name, "vesthelm"))
		return "kevlar_helmet";
	if (!stricmp(name, "sgren"))
		return "smokegrenade";
	if (!stricmp(name, "nvgs"))
		return "nightvision";
	if (!stricmp(name, "flash"))
		return "flashbang";
	if (!stricmp(name, "glock18"))
		return "glock";
	if (!stricmp(name, "usp"))
		return "usp45";
	if (!stricmp(name, "elite"))
		return "elites";
	if (!stricmp(name, "deagle"))
		return "deaglered";
	if (!stricmp(name, "awp"))
		return "awpcamo";
	if (!stricmp(name, "usas"))
		return "usas-12";
	return name;
}

wchar_t *GetWeaponNameFormat(char *name)
{
	char *pTemp = name;
	if (!stricmp(name, "vest"))
		pTemp = "Kevlar";
	else if (!stricmp(name, "vesthelm"))
		pTemp = "Kevlar_Helmet";
	else if (!stricmp(name, "hegrenade"))
		pTemp = "HE_Grenade";
	else if (!stricmp(name, "sgren"))
		pTemp = "Smoke_Grenade";
	else if (!stricmp(name, "nvgs"))
		pTemp = "NightVision_Button";
	else if (!stricmp(name, "fgrenade"))
		pTemp = "FireBomb";
	else if (!stricmp(name, "usas"))
		pTemp = "USAS12";
	else if (!stricmp(name, "usp"))
		pTemp = "usp45";
	else if (!stricmp(name, "deagle"))
		pTemp = "DesertEagle";
	else if (!stricmp(name, "cake"))
		pTemp = "CakeBomb";
	else if (!stricmp(name, "elite"))
		pTemp = "Elites";
	else if (!stricmp(name, "r93"))
		pTemp = "blaser93";
	else if (!stricmp(name, "automagv"))
		pTemp = "automag";
	else if (!stricmp(name, "m249xmas"))
		pTemp = "m249RED";
	else if (!stricmp(name, "awpxmas"))
		pTemp = "awpRED";
	else if (!stricmp(name, "strong_knife"))
		pTemp = "NataKnife";
	else if (!stricmp(name, "g_ak47"))
		pTemp = "AK47G";
	else if (!stricmp(name, "g_m4a1"))
		pTemp = "M4A1G";
	else if (!stricmp(name, "g_deagle"))
		pTemp = "DesertEagleG";

	wchar_t *wstrResult = vgui::localize()->Find(va("#CSO_%s", pTemp));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSO_Item_Name_%s", pTemp));

	if (!wstrResult)
		wstrResult = vgui::localize()->Find(va("#CSBTE_%s", pTemp));

	return wstrResult ? wstrResult : UTF8ToUnicode(va("#CSBTE_%s", pTemp));
}

wchar_t *GetWeaponNameFormat(const std::string &name)
{
	static char buffer[128];
	strcpy(buffer, name.c_str());
	return GetWeaponNameFormat(buffer);
}

wchar_t *GetWeaponDescription(char *pItem)
{
	char *pName = AliasWeaponName(pItem);
	if (!stricmp(pName, "fgrenade"))
		pName = "FireBomb";
	return GetLangUni(va("#CStrike_%sDescription", pName));
}

auto CWeaponManager::OnPrecacheWeapon(const char *szName) -> CustomWeapon &
{
	char szLowerName[127];
	strcpy(szLowerName, szName);
	strlwr(szLowerName);

	auto it = m_DataMap.find(szLowerName);
	if (it != m_DataMap.end())
		return *(it->second);

	if (szLowerName[0])
	{
		CustomWeapon Data = LoadWeaponData(szLowerName);
		CustomWeapon *pResult = new CustomWeapon(Data);
		m_DataMap[szLowerName] = pResult;
		return *pResult;
	}
	return m_NullWeapon;
}

auto CWeaponManager::Find(const char *szName)->CustomWeapon &
{
	static char szRealName[127];
	strcpy(szRealName, szName);
	strlwr(szRealName);

	if (!stricmp(szName, "sfsword_off"))
		szRealName[7] = 0;

	if (!stricmp(szName, "janus9_a") || !stricmp(szName, "janus9_b"))
		szRealName[6] = 0;

	if (!stricmp(szName, "thanatos9a") || !stricmp(szName, "thanatos9b"))
		szRealName[9] = 0;

	if (!stricmp(szName, "crow9a") || !stricmp(szName, "crow9b"))
		szRealName[5] = 0;

	if (!stricmp(szName, "blockar1") || !stricmp(szName, "blockar2"))
		szRealName[7] = 0;

	if (!stricmp(szName, "blockas1") || !stricmp(szName, "blockas2"))
		szRealName[7] = 0;

	if (!stricmp(szName, "blocksmg1") || !stricmp(szName, "blocksmg2"))
		szRealName[8] = 0;

	if (!stricmp(szName, "desperado_m") || !stricmp(szName, "desperado_w"))
		szRealName[9] = 0;

	if (!stricmp(szName, "dualsword_a") || !stricmp(szName, "dualsword_b"))
		szRealName[9] = 0;

	if (!stricmp(szName, "blockchange"))
	{
		if (g_iCurrentWeapon == WEAPON_M3 || g_iCurrentWeapon == WEAPON_XM1014)
			strcpy(szRealName, "blockas");
		else if (g_iCurrentWeapon == WEAPON_MP5NAVY || g_iCurrentWeapon == WEAPON_MAC10 || g_iCurrentWeapon == WEAPON_TMP || g_iCurrentWeapon == WEAPON_UMP45 || g_iCurrentWeapon == WEAPON_P90)
			strcpy(szRealName, "blocksmg");
		else if (g_iCurrentWeapon == WEAPON_M249)
			strcpy(szRealName, "blockmg");
		else
			strcpy(szRealName, "blockar");
	}

	auto it = m_DataMap.find(szRealName);
	if (it != m_DataMap.end())
		return *(it->second);

	return m_NullWeapon;
}

auto CWeaponManager::LoadWeaponData(const char *szName) -> CustomWeapon
{
	CustomWeapon Result;
	
	char szValue[128];
	char szValue2[128];
	int iValue;
	float fValue;

	sprintf(Result.szName, "%s", szName);

	Result.iMenu = GetWeaponBuyMenuType(szName);

	char WeaponID[32];
	GetPrivateProfileStringA(szName, "WeaponID", "", WeaponID, sizeof(WeaponID), "cstrike/weapons.ini");
	Result.iSlot = FindWeaponSlot(WeaponID);

	GetPrivateProfileStringA(szName, "FireSrcOfs", "0", szValue, sizeof(szValue), "cstrike/weapons.ini");
	fValue = atof(szValue);
	Result.flSrcOfs = fValue;

	if (!strcmp(WeaponID, "ELITE") && !Result.flSrcOfs)
		Result.flSrcOfs = 5.0;

	GetPrivateProfileStringA(szName, "CanBuy", "TRUE", szValue, sizeof(szValue), "cstrike/weapons.ini");
	if (!strcmp(szValue, "TRUE"))
		Result.bCanBuy = true;
	else
		Result.bCanBuy = false;

	GetPrivateProfileStringA(szName, "Team", "ALL", szValue, sizeof(szValue), "cstrike/weapons.ini");
	if (!strcmp(szValue, "TR"))
		Result.iTeam = TR;
	else if (!strcmp(szValue, "CT"))
		Result.iTeam = CT;
	else
		Result.iTeam = ALL;

	GetPrivateProfileStringA(szName, "GameModeLimit", "0", szValue, sizeof(szValue), "cstrike/weapons.ini");
	Result.iModeLimit = atoi(szValue);

	GetPrivateProfileStringA(szName, "Cost", "0", szValue, sizeof(szValue), "cstrike/weapons.ini");
	Result.iPrice = atoi(szValue);

	GetPrivateProfileStringA(szName, "ShellModel", "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0])
	{
		Result.bHaveShellModel = true;
		sprintf(Result.szShell, "models/%s", szValue);
		/*IEngineStudio.Mod_ForName(g_WeaponData[iTotalWeapons].szShell, true);

		int id;
		gEngfuncs.CL_LoadModel(g_WeaponData[iTotalWeapons].szShell, &id);*/
	}
	else
	{
		Result.bHaveShellModel = false;
		Result.szShell[0] = '\0';
	}
	
	GetPrivateProfileStringA(szName, "Shell", "!", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0] != '!')
	{
		Result.bHaveShellVec = true;

		char *p;
		p = strtok(szValue, ",");

		if (p)
		{
			Result.fShellVec[0] = atof(p);
			p = strtok(NULL, ",");
			Result.fShellVec[1] = atof(p);
			p = strtok(NULL, ",");
			Result.fShellVec[2] = atof(p);
			p = strtok(NULL, ",");
			Result.fShellVec[3] = atof(p);
		}
	}
	else
		Result.bHaveShellVec = false;

	GetPrivateProfileStringA(szName, "MaxClip", "0", szValue, sizeof(szValue), "cstrike/weapons.ini");
	iValue = atoi(szValue);
	Result.iClip = iValue;

	GetPrivateProfileStringA(szName, "MaxAmmo", "0", szValue, sizeof(szValue), "cstrike/weapons.ini");
	iValue = atoi(szValue);
	Result.iAmmo = iValue;

	// --------------------------------- //

	GetPrivateProfileStringA(szName, "ShootSound", "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0])
		sprintf(Result.szSound, "sound/weapons/%s", szValue);
	else
		sprintf(Result.szSound, "sound/weapons/%s-1.wav", szName);

	GetPrivateProfileStringA(szName, "ShootSound2", "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0])
		sprintf(Result.szSound2, "sound/weapons/%s", szValue);
	else
		sprintf(Result.szSound2, "sound/weapons/%s-1.wav", szName);

	GetPrivateProfileStringA(szName, "ShootSoundUnSilen", "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0])
		sprintf(Result.szSoundSilen, "sound/weapons/%s", szValue);
	else
		sprintf(Result.szSoundSilen, "sound/weapons/%s_unsil-1.wav", szName);

	GetPrivateProfileStringA(szName, "ShootSoundUnSilen2", "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0])
		sprintf(Result.szSoundSilen2, "sound/weapons/%s", szValue);
	else
		sprintf(Result.szSoundSilen2, "sound/weapons/%s_unsil-1.wav", szName);

	GetPrivateProfileStringA(szName, "RandomSound", "true", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (!strcmp(szValue, "true"))
		Result.bRandomSound = true;
	else
		Result.bRandomSound = false;

	for (int i = 0; i <= 4; i++)
	{
		char key[16];
		sprintf(key, "Slash%d", i + 1);
		GetPrivateProfileStringA(szName, key, "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
		if (szValue[0]) Result.iSlash = i;
		sprintf(Result.szSlash[i], "sound/weapons/%s", szValue);

		sprintf(key, "SlashHit%d", i + 1);
		GetPrivateProfileStringA(szName, key, "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
		if (szValue[0]) Result.iSlashHit = i;
		sprintf(Result.szSlashHit[i], "sound/weapons/%s", szValue);

		sprintf(key, "Wall%d", i + 1);
		GetPrivateProfileStringA(szName, key, "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
		if (szValue[0]) Result.iWall = i;
		sprintf(Result.szWall[i], "sound/weapons/%s", szValue);

		sprintf(key, "Stab%d", i + 1);
		GetPrivateProfileStringA(szName, key, "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
		if (szValue[0]) Result.iStab = i;
		sprintf(Result.szStab[i], "sound/weapons/%s", szValue);

		sprintf(key, "StabHit%d", i + 1);
		GetPrivateProfileStringA(szName, key, "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
		if (szValue[0]) Result.iStabHit = i;
		sprintf(Result.szStabHit[i], "sound/weapons/%s", szValue);
	}

	GetPrivateProfileStringA(szName, "Draw", "", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0])
		sprintf(Result.szDraw, "sound/weapons/%s", szValue);


	GetPrivateProfileStringA(szName, "ShotgunSpread", "0", szValue2, sizeof(szValue2), "cstrike/weapons_res.ini");
	if (szValue2[1])
	{
		int i = 1;
		char *p;
		p = strtok(szValue2, ",");

		Result.fShotGunSpread[0] = atof(p);

		while (p = strtok(NULL, ","))
		{
			Result.fShotGunSpread[i] = atof(p);

			i++;
		}
	}

	GetPrivateProfileStringA(szName, "ShotgunShots", "!", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (szValue[0] != '!')
	{
		iValue = atoi(szValue);
		Result.iShotGunShots = iValue;
	}

	GetPrivateProfileStringA(szName, "ViewModelDisplay", "0", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	iValue = atoi(szValue);
	Result.iViewModelDisplay = iValue;

	GetPrivateProfileStringA(szName, "SniperScopeType", "0", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	Result.iSniperScopeType = atoi(szValue);

	GetPrivateProfileStringA(szName, "SniperScope", "", Result.szSniperScope, sizeof(Result.szSniperScope), "cstrike/weapons_res.ini");

	if (Result.iSniperScopeType == 1)
	{
		char szSniperScope[64];
		sprintf(szSniperScope, "sprites\\%s_nw", Result.szSniperScope);
		Result.iSniperScope[0] = Hud().m_TGA.FindTexture(szSniperScope);
		sprintf(szSniperScope, "sprites\\%s_ne", Result.szSniperScope);
		Result.iSniperScope[1] = Hud().m_TGA.FindTexture(szSniperScope);
		sprintf(szSniperScope, "sprites\\%s_sw", Result.szSniperScope);
		Result.iSniperScope[2] = Hud().m_TGA.FindTexture(szSniperScope);
		sprintf(szSniperScope, "sprites\\%s", Result.szSniperScope);
		Result.iSniperScope[3] = Hud().m_TGA.FindTexture(szSniperScope);
	}

	int iEffect = 0;
	GetPrivateProfileStringA(szName, "Effect", "0", szValue, sizeof(szValue), "cstrike/weapons_res.ini");
	if (strstr(szValue, "EFFECT_SHELL_M134"))
		iEffect |= (1 << EFFECT_SHELL_M134);
	if (strstr(szValue, "EFFECT_SHAKE"))
		iEffect |= (1 << EFFECT_SHAKE);
	if (strstr(szValue, "EFFECT_M1887_XMAS"))
		iEffect |= (1 << EFFECT_M1887_XMAS);
	if (strstr(szValue, "EFFECT_BLOCK_ORIGIN"))
		iEffect |= (1 << EFFECT_BLOCK_ORIGIN);
	Result.iEffect = iEffect;

	// --------------------------------- //

	GetPrivateProfileStringA(szName, "ShootAnim", "0", szValue2, sizeof(szValue2), "cstrike/weapons_anim.ini");
	if (szValue2[1])
	{
		Result.bHaveAnim = true;

		char *p;

		p = strtok(szValue2, ",");
		Result.iAnimStart = atoi(p);

		p = strtok(NULL, ",");
		if (p)
			Result.iAnimEnd = atoi(p);
	}
	else
		Result.bHaveAnim = false;

	GetPrivateProfileStringA(szName, "ShootAnim2", "0", szValue2, sizeof(szValue2), "cstrike/weapons_anim.ini");
	if (szValue2[1])
	{
		char *p;

		p = strtok(szValue2, ",");
		Result.iAnimStart2 = atoi(p);

		p = strtok(NULL, ",");
		if (p)
			Result.iAnimEnd2 = atoi(p);
	}

	GetPrivateProfileStringA(szName, "ShootAnimEmpty", "", szValue, sizeof(szValue), "cstrike/weapons_anim.ini");
	iValue = atoi(szValue);
	if (iValue)
		Result.iAnimEmpty = iValue;



	return std::move(Result);
}

auto CWeaponManager::GetWeaponBuyMenuType(const char *szName) -> WeaponBuyMenuType
{
	char szValue[128];
	GetPrivateProfileStringA(szName, "Menu", "0", szValue, sizeof(szValue), "cstrike/weapons.ini");

	if (!strcmp(szValue, "PISTOL"))
	{
		return PISTOL;
	}
	else if (!strcmp(szValue, "SHOTGUN"))
	{
		return SHOTGUN;
	}
	else if (!strcmp(szValue, "SMG"))
	{
		return SMG;
	}
	else if (!strcmp(szValue, "RIFLE"))
	{
		return RIFLE;
	}
	else if (!strcmp(szValue, "MG"))
	{
		return MG;
	}
	else if (!strcmp(szValue, "EQUIP"))
	{
		return EQUIP;
		char szValue2[32];
		GetPrivateProfileStringA(szName, "WeaponID", "", szValue2, sizeof(szValue2), "cstrike/weapons.ini");

		//if (strcmp(szValue2, "HEGERNADE"))
		//g_WeaponData[iTotalWeapons].iSlot = 1;

	}
	else if (!strcmp(szValue, "KNIFE"))
	{
		return KNIFE;
	}
	
	return NONE;
}
