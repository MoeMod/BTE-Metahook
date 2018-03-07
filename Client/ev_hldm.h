typedef enum
{
	BULLET_NONE = 0,
	BULLET_PLAYER_9MM,
	BULLET_PLAYER_MP5,
	BULLET_PLAYER_357,
	BULLET_PLAYER_BUCKSHOT,
	BULLET_PLAYER_CROWBAR,

	BULLET_MONSTER_9MM,
	BULLET_MONSTER_MP5,
	BULLET_MONSTER_12MM,

	BULLET_PLAYER_45ACP,
	BULLET_PLAYER_338MAG,
	BULLET_PLAYER_762MM,
	BULLET_PLAYER_556MM,
	BULLET_PLAYER_50AE,
	BULLET_PLAYER_57MM,
	BULLET_PLAYER_357SIG,

	BULLET_PLAYER_46MM,
	BULLET_PLAYER_OIL,
	BULLET_PLAYER_SFSNIPER,
	BULLET_PLAYER_SFSNIPERM,
	BULLET_PLAYER_ZOMBIEGUN,
	BULLET_PLAYER_NNGAUSS_1,
	BULLET_PLAYER_NNGAUSS_2,
	BULLET_PLAYER_ETHEREAL,
	BULLET_PLAYER_RAILCANNON
}
Bullet;

extern int g_iShotsFired;

void EV_HLDM_CreateSmoke(float *origin, float *dir, int speed, float scale, int r, int g, int b, int iSmokeType, float *base_velocity, bool bWind, int framerate);
char EV_HLDM_PlayTextureSound(int idx, pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType);
char *EV_HLDM_DamageDecal(physent_t *pe);
void EV_HLDM_GunshotDecalTrace(pmtrace_t *pTrace, char *decalName, char chTextureType);
void EV_HLDM_DecalGunShot(pmtrace_t *pTrace, int iBulletType, float scale, int r, int g, int b, bool bStreakSplash, char chTextureType);
int EV_HLDM_CheckTracer(int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount);
void EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration, int iAttachment = 0, bool lefthand = false, float srcofs = 0.0);