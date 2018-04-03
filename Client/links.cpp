#include "hud.h"
#include "parsemsg.h"
#include "cl_util.h"
#include <demo_api.h>
#include "msghook.h"
#include "Modules.h"
#include <cl_entity.h>
#include <event_args.h>
#include <pmtrace.h>
#include <usercmd.h>

#include "CounterStrikeViewport.h"
#include "CVARs.h"

void *gpViewPortInterface = (void *)0x1A3A7B8;

extra_player_info_t *g_PlayerExtraInfo = (extra_player_info_t *)0x1A25618;
hostage_info_t *g_HostageInfo = (hostage_info_t *)0x19F73E8;

void (__fastcall *g_pfnCHudHealth_DrawRadar)(void *, int, float) = (void (__fastcall *)(void *, int, float))0x194A780;
int (__fastcall *g_pfnCHudHealth_Draw)(void *, int, float) = (int (__fastcall *)(void *, int, float))0x19490E0;
void (__fastcall *g_pfnCHudHealth_DrawRadarDot)(void *, int, int, int, float, int, int, int, int, int, int) = (void (__fastcall *)(void *, int, int, int, float, int, int, int, int, int, int))0x194A490;

int (__fastcall *g_pfnCHudAccountBalance_Draw)(void *, int, float) = (int (__fastcall *)(void *, int, float))0x19410B0;
int (__fastcall *g_pfnCHudRoundTimer_Draw)(void *, int, float) = (int (__fastcall *)(void *, int, float))0x1949D90;

void (__fastcall *g_pfnWeaponsResource_SelectSlot)(void *, int, int iSlot, int fAdvance, int iDirection) = (void (__fastcall *)(void *, int, int, int, int))0x193EB90;
int (__fastcall *g_pfnCHudAmmo_DrawCrosshair)(void *, int, float flTime, int weaponid) = (int (__fastcall *)(void *, int, float, int))0x1940430;
int (__fastcall *g_pfnCHudAmmo_Draw)(void *, int, float flTime) = (int (__fastcall *)(void *, int, float))0x193FB60;
int *g_pgHUD_m_Menu_m_fMenuDisplayed = (int *)0x1A1B8A8;

void *g_pgHUD_m_Menu = (void *)0x1A1B894;
void (__fastcall *g_pfnCHudMenu_SelectMenuItem)(void *, int, int menu_item) = (void (__fastcall *)(void *, int, int))0x1957B90;

bool *g_bGunSmoke = (bool *)0x1A1C1A8;
vec3_t *g_pv_angles = (vec3_t *)0x1A34008;

void (*g_pfnEV_HLDM_CreateSmoke)(float *origin, float *dir, int speed, float scale, int r, int g, int b, int iSmokeType, float *base_velocity, bool bWind, int framerate) = (void (*)(float *, float *, int, float, int, int, int, int, float *, bool, int))0x19017D0;
void (*g_pfnEV_HLDM_DecalGunshot)(pmtrace_t *pTrace, int iBulletType, float scale, int r, int g, int b, bool bCreateSparks, char cTextureType) = (void (*)(pmtrace_t *, int, float, int, int, int, bool, char))0x19020B0;
void (*g_pfnEV_HLDM_FireBullets)(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration) = (void (*)(int, float *, float *, float *, int, float *, float *, float *, float, int, int, int *, int))0x1902460;
void (*g_pfnEV_VehiclePitchAdjust)(event_args_t *args) = (void (*)(event_args_t *))0x1904380;
void (*g_pfnEV_RemoveAllDecals)(struct event_args_s *args) = (void (*)(struct event_args_s *))0x190A290;
void (*g_pfnEV_CreateSmoke)(struct event_args_s *args) = (void (*)(struct event_args_s *))0x190A080;
void (*g_pfnEV_EjectBrass)(float *origin, float *velocity, float rotation, int model, int soundtype, int idx, int angle_velocity) = (void (*)(float *, float *, float, int, int, int, int))0x1945180;

float *(*g_pfnGetClientColor)(int clientIndex) = (float *(*)(int))0x1943360;

int (__fastcall *g_pfnCHudSpectator_Draw)(void *, int, float flTime) = (int (__fastcall *)(void *, int, float))0x1950450;
void(__fastcall *g_pfnCHudSayText_Draw)(void *, int, float) = (void(__fastcall *)(void *, int, float))0x1960A10;
int(__fastcall *g_pfnHudSniperScope_Draw)(void *, int, float flTime) = (int(__fastcall *)(void *, int, float))0x1961AD0;

void (__fastcall *g_pfnCCounterStrikeViewport_StartProgressBar)(void *pthis, int, const char *title, int numTicks, int startTicks, bool isTimeBased) = (void (__fastcall *)(void *, int, const char *, int, int, bool))0x1927370;
void (__fastcall *g_pfnCCounterStrikeViewport_UpdateProgressBar)(void *pthis, int, const char *statusText, int tick) = (void (__fastcall *)(void *, int, const char *, int))0x1927420;
void (__fastcall *g_pfnCCounterStrikeViewport_StopProgressBar)(void *pthis, int) = (void (__fastcall *)(void *, int))0x1927430;

class CStudioModelRenderer *g_pStudioRenderer = (CStudioModelRenderer *)0x1A20F90;
/*
BOOL g_bIsNewClient = FALSE;

int CL_IsNewClient(void)
{
	return g_bIsNewClient;
}

void CL_LinkNewClient(unsigned long dwBaseAddress)
{
	g_bIsNewClient = TRUE;

	gpViewPortInterface = (void *)(dwBaseAddress + 0x1439E8);
	g_PlayerExtraInfo = (extra_player_info_t *)(dwBaseAddress + 0x12E3C0);
	g_HostageInfo = (hostage_info_t *)(dwBaseAddress + 0xFFD98);

	g_pfnCHudHealth_DrawRadar = (void (__fastcall *)(void *, int, float))(dwBaseAddress + 0x4B990);
	g_pfnCHudHealth_Draw = (int (__fastcall *)(void *, int, float))(dwBaseAddress + 0x4A2B0);
	g_pfnCHudHealth_DrawRadarDot = (void (__fastcall *)(void *, int, int, int, float, int, int, int, int, int, int))(dwBaseAddress + 0x4B6A0);

	g_pfnWeaponsResource_SelectSlot = (void (__fastcall *)(void *, int, int, int, int))(dwBaseAddress + 0x3FD10);
	g_pfnCHudAmmo_DrawCrosshair = (int (__fastcall *)(void *, int, float, int))(dwBaseAddress + 0x41640);
	g_pfnCHudAmmo_Draw = (int (__fastcall *)(void *, int, float))(dwBaseAddress + 0x40D70);
	g_pgHUD_m_Menu_m_fMenuDisplayed = (int *)(dwBaseAddress + 0x124330);

	g_pfnCHudAccountBalance_Draw = (int (__fastcall *)(void *, int, float))(dwBaseAddress + 0x42310);
	g_pfnCHudRoundTimer_Draw = (int (__fastcall *)(void *, int, float))(dwBaseAddress + 0x4AFA0);

	g_pgHUD_m_Menu = (void *)(dwBaseAddress + 0x12431C);
	g_pfnCHudMenu_SelectMenuItem = (void (__fastcall *)(void *, int, int))(dwBaseAddress + 0x59CB0);

	g_bGunSmoke = (bool *)(dwBaseAddress + 0x124C30);
	g_pv_angles = (vec3_t *)(dwBaseAddress + 0x13CE20);

	g_pfnEV_HLDM_CreateSmoke = (void (*)(float *, float *, int, float, int, int, int, int, float *, bool, int))(dwBaseAddress + 0x17D0);
	g_pfnEV_HLDM_DecalGunshot = (void (*)(pmtrace_t *, int, float, int, int, int, bool, char))(dwBaseAddress + 0x20B0);
	g_pfnEV_HLDM_FireBullets = (void (*)(int, float *, float *, float *, int, float *, float *, float *, float, int, int, int *, int))(dwBaseAddress + 0x2460);
	g_pfnEV_VehiclePitchAdjust = (void (*)(event_args_t *))(dwBaseAddress + 0x4380);
	g_pfnEV_RemoveAllDecals = (void (*)(struct event_args_s *))(dwBaseAddress + 0xA290);
	g_pfnEV_CreateSmoke = (void (*)(struct event_args_s *))(dwBaseAddress + 0xA080);
	g_pfnEV_EjectBrass = (void (*)(float *, float *, float, int, int, int, int))(dwBaseAddress + 0x45F70);

	g_pfnGetClientColor = (float *(*)(int))(dwBaseAddress + 0x444B0);

	g_pfnCHudSpectator_Draw = (int (__fastcall *)(void *, int, float))(dwBaseAddress + 0x52700);

	g_pfnCCounterStrikeViewport_StartProgressBar = (void (__fastcall *)(void *, int, const char *, int, int, bool))(dwBaseAddress + 0x27080);
	g_pfnCCounterStrikeViewport_UpdateProgressBar = (void (__fastcall *)(void *, int, const char *, int))(dwBaseAddress + 0x27130);
	g_pfnCCounterStrikeViewport_StopProgressBar = (void (__fastcall *)(void *, int))(dwBaseAddress + 0x27140);

	g_pStudioRenderer = (CStudioModelRenderer *)(dwBaseAddress + 0x29A28);
}
*/