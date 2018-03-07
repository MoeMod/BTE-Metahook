#pragma once

#define vec3_t Vector

#define WEAPON_P228          1
#define WEAPON_SCOUT         3
#define WEAPON_HEGRENADE     4
#define WEAPON_XM1014        5
#define WEAPON_C4            6
#define WEAPON_MAC10         7
#define WEAPON_AUG           8
#define WEAPON_SMOKEGRENADE  9
#define WEAPON_ELITE         10
#define WEAPON_FIVESEVEN     11
#define WEAPON_UMP45         12
#define WEAPON_SG550         13
#define WEAPON_GALIL         14
#define WEAPON_FAMAS         15
#define WEAPON_USP           16
#define WEAPON_GLOCK18       17
#define WEAPON_AWP           18
#define WEAPON_MP5NAVY       19
#define WEAPON_M249          20
#define WEAPON_M3            21
#define WEAPON_M4A1          22
#define WEAPON_TMP           23
#define WEAPON_G3SG1         24
#define WEAPON_FLASHBANG     25
#define WEAPON_DEAGLE        26
#define WEAPON_SG552         27
#define WEAPON_AK47          28
#define WEAPON_KNIFE         29
#define WEAPON_P90           30

#define WEAPON_VEST          31
#define CSW_VESTHELM         32

#define MAX_WEAPON_NAME		128
#define MAX_WEAPONS			32
#define MAX_AMMO_TYPES		32

struct WEAPONDATA
{
	char szName[MAX_WEAPON_NAME];
	int iClip;
	int iAmmoType;
	wrect_t rcAmmoIcon;
	int hAmmoIcon;
	wrect_t rcAmmoIcon2;
	int hAmmoIcon2;
	wrect_t rcCrosshair;
	int hCrosshair;
	wrect_t rcAutoaim;
	int hAutoaim;

	int iAmmoDisplay;
	int iHideCrosshair;
	int iCrosshair;
};

extern WEAPONDATA g_iWeaponData[MAX_WEAPONS];

extern int g_iCurrentWeapon; // weapon id

void WeaponInitialize(void);
client_sprite_t *LoadWeaponSprite(char *psz, int *piCount);

int FindWeaponId(const char *str);
int FindWeaponSlot(const char *sz);
int FindWeaponSlot(int weaponid);