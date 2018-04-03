extern void *gpViewPortInterface;

extern void (__fastcall *g_pfnCHudHealth_DrawRadar)(void *, int, float);
extern int (__fastcall *g_pfnCHudHealth_Draw)(void *, int, float);
extern void (__fastcall *g_pfnCHudHealth_DrawRadarDot)(void *, int, int, int, float, int, int, int, int, int, int);

extern int (__fastcall *g_pfnCHudAccountBalance_Draw)(void *, int, float);
extern int (__fastcall *g_pfnCHudRoundTimer_Draw)(void *, int, float);

extern void (__fastcall *g_pfnWeaponsResource_SelectSlot)(void *, int, int iSlot, int fAdvance, int iDirection);
extern int (__fastcall *g_pfnCHudAmmo_DrawCrosshair)(void *, int, float flTime, int weaponid);
extern int (__fastcall *g_pfnCHudAmmo_Draw)(void *, int, float flTime);
extern int *g_pgHUD_m_Menu_m_fMenuDisplayed;

extern void *g_pgHUD_m_Menu;
extern void (__fastcall *g_pfnCHudMenu_SelectMenuItem)(void *, int, int menu_item);

extern bool *g_bGunSmoke;
extern vec3_t *g_pv_angles;

extern void (*g_pfnEV_HLDM_CreateSmoke)(float *origin, float *dir, int speed, float scale, int r, int g, int b, int iSmokeType, float *base_velocity, bool bWind, int framerate);
extern void (*g_pfnEV_HLDM_DecalGunshot)(struct pmtrace_s *pTrace, int iBulletType, float scale, int r, int g, int b, bool bCreateSparks, char cTextureType);
extern void (*g_pfnEV_HLDM_FireBullets)(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration);
extern void (*g_pfnEV_VehiclePitchAdjust)(event_args_t *args);
extern void (*g_pfnEV_RemoveAllDecals)(struct event_args_s *args);
extern void (*g_pfnEV_CreateSmoke)(struct event_args_s *args);
extern void (*g_pfnEV_EjectBrass)(float *origin, float *velocity, float rotation, int model, int soundtype, int idx, int angle_velocity);

extern float *(*g_pfnGetClientColor)(int clientIndex);

extern int (__fastcall *g_pfnCHudSpectator_Draw)(void *, int, float flTime);
extern void(__fastcall *g_pfnCHudSayText_Draw)(void *, int, float);
extern int(__fastcall *g_pfnHudSniperScope_Draw)(void *, int, float flTime);

extern void (__fastcall *g_pfnCCounterStrikeViewport_StartProgressBar)(void *pthis, int, const char *title, int numTicks, int startTicks, bool isTimeBased);
extern void (__fastcall *g_pfnCCounterStrikeViewport_UpdateProgressBar)(void *pthis, int, const char *statusText, int tick);
extern void (__fastcall *g_pfnCCounterStrikeViewport_StopProgressBar)(void *pthis, int);

extern class CStudioModelRenderer *g_pStudioRenderer;