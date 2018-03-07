#include "base.h"
#include "hud.h"
#include "weapons.h"
#include "util.h"

#include "Client/WeaponManager.h"

int g_iCurrentWeapon; // weapon id

WEAPONDATA g_iWeaponData[MAX_WEAPONS];

void WeaponInitialize(void)
{
	memset(g_iWeaponData, 0, sizeof(g_iWeaponData));


	/*FILE* pWeaponList;
	static char szBuff[512];
	szBuff[0] = '\0';
	char szFileName[128];
	sprintf(szFileName,"metahook\\weapons.lst",gConfigs.szLanguage);
	pWeaponList = fopen(szFileName, "rt");
	if(pWeaponList == NULL)									
	{
		return;
	}
	char szTemp[258];
	int id = 0;

	while(!feof(pWeaponList))
	{
		fgets(szBuff, sizeof(szBuff)-1, pWeaponList);
		if(szBuff[strlen(szBuff)-1] == '\n') szBuff[strlen(szBuff)-1] = '\0';

		if(szBuff[0] == ';' || !szBuff[0]) continue;

		LoadWeaponData(szBuff);
	}
	fclose(pWeaponList);*/
}

client_sprite_t *LoadWeaponSprite(char *psz, int *piCount)
{
	char szSpritePath[128];

	for (int i = 1; i < MAX_WEAPONS; i++)
	{
		if (!strlen(g_iWeaponData[i].szName))
			continue;

		sprintf(szSpritePath, "sprites/%s.txt", g_iWeaponData[i].szName);

		if (strcmp(szSpritePath, psz))
			continue;

		memset(&g_iWeaponData[i].rcAmmoIcon, 0, sizeof(wrect_t));
		memset(&g_iWeaponData[i].rcAmmoIcon2, 0, sizeof(wrect_t));
		memset(&g_iWeaponData[i].rcAutoaim, 0, sizeof(wrect_t));
		memset(&g_iWeaponData[i].rcCrosshair, 0, sizeof(wrect_t));

		client_sprite_t *pList = gEngfuncs.pfnSPR_GetList(psz, piCount);

		if (!pList)
			continue;

		client_sprite_t *p;
		int ii = *piCount;
		char sz[128];
		if ((p = GetSpriteList(pList, "ammo", Hud().m_iRes, *piCount)))
		{
			sprintf(sz, "sprites/%s.spr", p->szSprite);
			g_iWeaponData[i].hAmmoIcon = gEngfuncs.pfnSPR_Load(sz);
			g_iWeaponData[i].rcAmmoIcon = p->rc;
		}

		if ((p = GetSpriteList(pList, "ammo2", Hud().m_iRes, *piCount)))
		{
			sprintf(sz, "sprites/%s.spr", p->szSprite);
			g_iWeaponData[i].hAmmoIcon2 = gEngfuncs.pfnSPR_Load(sz);
			g_iWeaponData[i].rcAmmoIcon2 = p->rc;
		}

		if ((p = GetSpriteList(pList, "crosshair", Hud().m_iRes, *piCount)))
		{
			sprintf(sz, "sprites/%s.spr", p->szSprite);
			g_iWeaponData[i].hCrosshair = gEngfuncs.pfnSPR_Load(sz);
			g_iWeaponData[i].rcCrosshair = p->rc;
		}

		if ((p = GetSpriteList(pList, "autoaim", Hud().m_iRes, *piCount)))
		{
			sprintf(sz, "sprites/%s.spr", p->szSprite);
			g_iWeaponData[i].hAutoaim = gEngfuncs.pfnSPR_Load(sz);
			g_iWeaponData[i].rcAutoaim = p->rc;
		}

		if ((p = GetSpriteList(pList, "zoom", Hud().m_iRes, *piCount)))
		{
			
			if (pCvar_DrawScope->value && !WeaponManager()[g_iWeaponData[i].szName + 7].iSniperScopeType)
				p->rc.bottom = p->rc.right = 1;
		}

		return pList;
	}

	return gEngfuncs.pfnSPR_GetList(psz, piCount);
}

const int PRIMARY_WEAPONS_BIT_SUM = (1 << WEAPON_SCOUT) | (1 << WEAPON_XM1014) | (1 << WEAPON_MAC10) | (1 << WEAPON_AUG) | (1 << WEAPON_UMP45) | (1 << WEAPON_SG550) | (1 << WEAPON_GALIL) | (1 << WEAPON_FAMAS) | (1 << WEAPON_AWP) | (1 << WEAPON_MP5NAVY) | (1 << WEAPON_M249) | (1 << WEAPON_M3) | (1 << WEAPON_M4A1) | (1 << WEAPON_TMP) | (1 << WEAPON_G3SG1) | (1 << WEAPON_SG552) | (1 << WEAPON_AK47) | (1 << WEAPON_P90);
const int SECONDARY_WEAPONS_BIT_SUM = (1 << WEAPON_P228) | (1 << WEAPON_ELITE) | (1 << WEAPON_FIVESEVEN) | (1 << WEAPON_USP) | (1 << WEAPON_GLOCK18) | (1 << WEAPON_DEAGLE);
const int GRENADE_WEAPONS_BIT_SUM = (1 << WEAPON_HEGRENADE) | (1 << WEAPON_SMOKEGRENADE) | (1 << WEAPON_FLASHBANG);
const int KNIFE_WEAPONS_BIT_SUM = (1 << WEAPON_KNIFE);

int FindWeaponId(const char *str)
{
	if (!Q_stricmp(str, "P228")) return WEAPON_P228;
	if (!Q_stricmp(str, "SCOUT")) return WEAPON_SCOUT;
	if (!Q_stricmp(str, "HEGERNADE")) return WEAPON_HEGRENADE;
	if (!Q_stricmp(str, "XM1014")) return WEAPON_XM1014;
	if (!Q_stricmp(str, "C4")) return WEAPON_C4;
	if (!Q_stricmp(str, "MAC10")) return WEAPON_MAC10;
	if (!Q_stricmp(str, "AUG")) return WEAPON_AUG;
	if (!Q_stricmp(str, "SMOKEGRENADE")) return WEAPON_SMOKEGRENADE;
	if (!Q_stricmp(str, "ELITE")) return WEAPON_ELITE;
	if (!Q_stricmp(str, "FIVESEVEN")) return WEAPON_FIVESEVEN;
	if (!Q_stricmp(str, "UMP45")) return WEAPON_UMP45;
	if (!Q_stricmp(str, "SG550")) return WEAPON_SG550;
	if (!Q_stricmp(str, "GALIL")) return WEAPON_GALIL;
	if (!Q_stricmp(str, "FAMAS")) return WEAPON_FAMAS;
	if (!Q_stricmp(str, "USP")) return WEAPON_USP;
	if (!Q_stricmp(str, "GLOCK18")) return WEAPON_GLOCK18;
	if (!Q_stricmp(str, "AWP")) return WEAPON_AWP;
	if (!Q_stricmp(str, "MP5")) return WEAPON_MP5NAVY;
	if (!Q_stricmp(str, "M249")) return WEAPON_M249;
	if (!Q_stricmp(str, "M3")) return WEAPON_M3;
	if (!Q_stricmp(str, "M4A1")) return WEAPON_M4A1;
	if (!Q_stricmp(str, "TMP")) return WEAPON_TMP;
	if (!Q_stricmp(str, "G3SG1")) return WEAPON_G3SG1;
	if (!Q_stricmp(str, "FLASHBANG")) return WEAPON_FLASHBANG;
	if (!Q_stricmp(str, "DEAGLE")) return WEAPON_DEAGLE;
	if (!Q_stricmp(str, "SG552")) return WEAPON_SG552;
	if (!Q_stricmp(str, "AK47")) return WEAPON_AK47;
	if (!Q_stricmp(str, "KNIFE")) return WEAPON_KNIFE;
	if (!Q_stricmp(str, "P90")) return WEAPON_P90;

	return 0;
}

int FindWeaponSlot(const char *sz)
{
	return FindWeaponSlot(FindWeaponId(sz));
}

int FindWeaponSlot(int weaponid)
{
	if (PRIMARY_WEAPONS_BIT_SUM & (1 << weaponid)) return 1;
	if (SECONDARY_WEAPONS_BIT_SUM & (1 << weaponid)) return 2;
	if (KNIFE_WEAPONS_BIT_SUM & (1 << weaponid)) return 3;
	if (GRENADE_WEAPONS_BIT_SUM & (1 << weaponid)) return 4;
	return 0;
}