#pragma once

#include <metahook.h>
#include <vector.h>
#include <cvardef.h>
#include <cl_entity.h>

#define MAX_PLAYERS	32
#define WPN_INFINITYEX	1
#define WPN_SFMG 2
#define WPN_SFSNIPER 3
#define WPN_M200 4
#define WPN_JANUS5 5
#define WPN_BOW 6
#define WPN_JANUS 7
#define WPN_M2 8
#define WPN_DESTROYER 9
#define WPN_GAUSS 10
#define WPN_BUFFAWP 11
#define WPN_BLOODHUNTER 12
#define WPN_STORMGIANT 13
#define WPN_ZOMBIBOMB 14
#define WPN_DUALSWORD 15

extern int g_iBTEWeapon;
extern int g_iButton;
//extern char g_szCurWeapon[32];
extern wchar_t g_szCurWeapon2[64];
//extern int g_iCurWeapon;
extern int g_iAnim;
extern int g_iTattooSmallEnable;
extern char g_szWeaponSkin[20][20];
extern int g_iWeaponSkin[20][2],g_iSetupSkin;
extern int g_iWeaponStat,g_iButton;
extern int g_iWeaponStat,g_iButton,g_iBl3Muzz, g_bInReload;
extern int g_iSpoce;
extern bool g_iszb4kicking;
extern int g_iCountBg;

typedef struct
{
	double time;
	double oldtime;
}client_t;
extern client_t cl;

struct PlayerInfo
{
	int alive;
	cl_entity_s* info;
	float killtime;
	int team;
	float fZBTime;
	Vector vLastPos;
	int iHero;
	int iDamage;
	float fAudioTime;
	float fDeathTime;
	float fInfectTime;
};
struct extra_player_info_t 
{
	short frags;
	short deaths;
	int iFlag;
	int iFake;
};

extern cvar_t *pCvar_TipIcon,*pCvar_Name,*pCvar_Blood,*pCvar_DrawBink,*pCvar_DrawPanel,*pCvar_DrawScope,*mh_scoreboard,*pCvar_DrawMenu,*pCvar_ZombieMenu;
extern cvar_t *cl_righthand;
extern cvar_t *cl_dynamiccrosshair;
extern cvar_t *cl_crosshair_color;
extern cvar_t *cl_crosshair_size;
extern cvar_t *cl_crosshair_translucent;
extern cvar_t *cl_crosshair_type;
extern cvar_t *cl_crosshair_hit;
extern cvar_t *cl_gunsmoke;

extern cvar_t *tracerred;
extern cvar_t *tracergreen;
extern cvar_t *tracerblue;
extern cvar_t *traceralpha;
extern cvar_t *tracerlength;
extern cvar_t *traceroffset;
extern cvar_t *tracerspeed;

extern cvar_t *viewmodel_widescreen;

extern cvar_t *cl_gunstreak;

extern cvar_t *hud_fastswitch_wheel;

extern cvar_t *gl_fxaa;

extern cvar_t *g_pcvarEnableConsole;

extern PlayerInfo vPlayer[36];
//extern extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];
extern extra_player_info_t *g_PlayerExtraInfo;
extern char g_szLocation[2048];
extern bool g_bAlive;
extern int g_iWeapon;
extern Vector g_vecOrigin, g_vecAngles, g_vecEyePos,g_vecEye,g_vecEndPos,g_vecVAngles;
extern int g_tga_Scope,g_tga_Scope_Target,g_tga_custom_scope,g_tga_custom_scope_m200;

extern int g_iViewEntityBody, g_iViewEntityRenderMode, g_iViewEntityRenderFX, g_iViewEntityRenderAmout;
extern color24 g_byViewEntityRenderColor;

//ExtraAmmo
extern int g_ExtraAmmo,g_ExtraAmmoIndex;
extern HSPRITE g_ExtraAmmoHsprite;
extern float g_fMuzzleTime;

//#define FLAG_HAND	(1<<31)
//#define FLAG_MALE	(1<<30)
//#define FLAG_FEMALE	(1<<29)

//#define FLAG_SHORT	(1<<28)
//#define FLAG_LONG	(1<<27)
//#define FLAG_ORIGIN	(1<<26)

//#define FLAG_UNCHANGE (1<<25)

//#define SEX_MALE	0
//#define SEX_FEMALE	1

//#define HAND_LONG	0
//#define HAND_SHORT	1
//#define HAND_ORIGIN	2
//extern char g_sTattoo[128];
//extern char g_sTattooSmall[128];
void CheckViewEntity(void);
//void RebuildTattoo(model_t *pModel);
//void LoadTattoo(void);

extern int g_isZombie;