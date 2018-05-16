
#include <metahook.h>
#include "pm_defs.h"
#include <event_api.h>

#include <BTE_Const.h>
#include "hud.h"
#include "exportfuncs.h"
#include "eventscripts.h"
#include "ev_hldm.h"
#include "message.h"
#include "msghook.h"
#include "pm_materials.h"
#include "weapons.h"
#include "util.h"
#include "common.h"
#include "R.h"
#include "Client/WeaponManager.h"
#include "Client/PlayerClassManager.h"

#include "Client/HUD/ammo.h"
#include "Client/HUD/Statistics.h"

qboolean EV_IsPlayer(int idx);
TEMPENTITY *CL_DefaultSprite(const vec3_t pos, model_t *sprite, float framerate);

extern "C"
{
	void EV_FireUSP(struct event_args_s *args);
	void EV_FireMP5N(struct event_args_s *args);
	void EV_FireAK47(struct event_args_s *args);
	void EV_FireAug(struct event_args_s *args);
	void EV_FireDeagle(struct event_args_s *args);
	void EV_FireG3SG1(struct event_args_s *args);
	void EV_FireSG550(struct event_args_s *args);
	void EV_FireGlock18(struct event_args_s *args);
	void EV_FireM249(struct event_args_s *args);
	void EV_FireM3(struct event_args_s *args);
	void EV_FireM4A1(struct event_args_s *args);
	void EV_FireMac10(struct event_args_s *args);
	void EV_FireP90(struct event_args_s *args);
	void EV_FireP228(struct event_args_s *args);
	void EV_FireAWP(struct event_args_s *args);
	void EV_FireScout(struct event_args_s *args);
	void EV_FireSG552(struct event_args_s *args);
	void EV_FireTMP(struct event_args_s *args);
	void EV_FireFiveSeven(struct event_args_s *args);
	void EV_FireUMP45(struct event_args_s *args);
	void EV_FireXM1014(struct event_args_s *args);
	void EV_FireEliteLeft(struct event_args_s *args);
	void EV_FireEliteRight(struct event_args_s *args);
	void EV_Knife(struct event_args_s *args);
	void EV_FireGalil(struct event_args_s *args);
	void EV_FireFamas(struct event_args_s *args);
	void EV_FireSprifle(struct event_args_s *args);
	void EV_FireM1887XMAS(struct event_args_s *args);
	void EV_FireSkull4Left(struct event_args_s *args);
	void EV_FireSkull4Right(struct event_args_s *args);
	void EV_FireDMP7A1Left(struct event_args_s *args);
	void EV_FireDMP7A1Right(struct event_args_s *args);
	void EV_FireSkull3Left(struct event_args_s *args);
	void EV_FireSkull3Right(struct event_args_s *args);
	void EV_FireM134(struct event_args_s *args);
	void EV_FireSkull1(struct event_args_s *args);
	void EV_FireFlameThrower(struct event_args_s *args);
	void EV_FireSfSniper(struct event_args_s *args);
	void EV_FirePetrolBoomer(struct event_args_s *args);
	void EV_FireSfPistol(struct event_args_s *args);
	void EV_FireBalrog11(struct event_args_s *args);
	void EV_FireLightZg(struct event_args_s *args);
	void EV_FireHeavyZg(struct event_args_s *args);
	void EV_FireRailCannon(struct event_args_s *args);
	void EV_FireChainsaw(struct event_args_s *args);
	void EV_FireCannon(struct event_args_s *args);
	void EV_FireEthereal(struct event_args_s *args);
	void EV_FireGilboaEX(struct event_args_s *args);
	void EV_FireM1Garand(struct event_args_s *args);
	void EV_FireRPG7(struct event_args_s *args);
	void EV_FireDestroyer(struct event_args_s *args);
	void EV_FireAirburster(struct event_args_s *args);
	void EV_FireSpsmg(struct event_args_s *args);
	void EV_FireSGDrill(struct event_args_s *args);
	void EV_FireBlockAR(struct event_args_s *args);
	void EV_StormGiant(struct event_args_s *args);
	void EV_StormGiantEffect(struct event_args_s *args);
	void EV_FireBuffM4A1(struct event_args_s *args);
	void EV_FireBuffAK47(struct event_args_s *args);
	void EV_FireBloodHunter(struct event_args_s *args);
	void EV_FireBow(struct event_args_s *args);
	void EV_SpinGauss(struct event_args_s *args);
	void EV_FireGauss(struct event_args_s *args);
	void EV_FireBuffSG552(struct event_args_s *args);
	void EV_FireCannonEX(struct event_args_s *args);
	void EV_FireRuneblade(struct event_args_s *args);
	void EV_RunebladeEffect(struct event_args_s *args);
	void EV_FireThanatos9(struct event_args_s *args);
	void EV_FireJanusmk5(struct event_args_s *args);
	void EV_FireInfinity(struct event_args_s *args);
	void EV_FireBlockSMG(struct event_args_s *args);
	void EV_FireDesperado(struct event_args_s *args);
	void EV_FireDualSword(struct event_args_s *args);
	void EV_FireJanus11(struct event_args_s *args);
	void EV_FireGunkata(struct event_args_s *args);
	void EV_FireSgmissile(struct event_args_s *args);

	void EV_Explosion(struct event_args_s *args);
	void EV_TE(struct event_args_s *args);
	void EV_Balrog11FollowEntities(struct event_args_s *args);

}

int g_iShotsFired;
int g_iAnim = 0;

int sModelIndexFireball2, sModelIndexFireball3;
int sModelIndexSpark, sModelIndexSpark2, sModelIndexSpark3;

cvar_t *cl_gunbubbles;
cvar_t *cl_tracereffect;

void Game_HookEvents(void)
{
	gEngfuncs.pfnHookEvent("events/usp.sc", EV_FireUSP);
	gEngfuncs.pfnHookEvent("events/mp5n.sc", EV_FireMP5N);
	gEngfuncs.pfnHookEvent("events/ak47.sc", EV_FireAK47);
	gEngfuncs.pfnHookEvent("events/aug.sc", EV_FireAug);
	gEngfuncs.pfnHookEvent("events/deagle.sc", EV_FireDeagle);
	gEngfuncs.pfnHookEvent("events/g3sg1.sc", EV_FireG3SG1);
	gEngfuncs.pfnHookEvent("events/sg550.sc", EV_FireSG550);
	gEngfuncs.pfnHookEvent("events/glock18.sc", EV_FireGlock18);
	gEngfuncs.pfnHookEvent("events/m249.sc", EV_FireM249);
	gEngfuncs.pfnHookEvent("events/m3.sc", EV_FireM3);
	gEngfuncs.pfnHookEvent("events/m4a1.sc", EV_FireM4A1);
	gEngfuncs.pfnHookEvent("events/mac10.sc", EV_FireMac10);
	gEngfuncs.pfnHookEvent("events/p90.sc", EV_FireP90);
	gEngfuncs.pfnHookEvent("events/p228.sc", EV_FireP228);
	gEngfuncs.pfnHookEvent("events/awp.sc", EV_FireAWP);
	gEngfuncs.pfnHookEvent("events/scout.sc", EV_FireScout);
	gEngfuncs.pfnHookEvent("events/sg552.sc", EV_FireSG552);
	gEngfuncs.pfnHookEvent("events/tmp.sc", EV_FireTMP);
	gEngfuncs.pfnHookEvent("events/fiveseven.sc", EV_FireFiveSeven);
	gEngfuncs.pfnHookEvent("events/ump45.sc", EV_FireUMP45);
	gEngfuncs.pfnHookEvent("events/xm1014.sc", EV_FireXM1014);
	gEngfuncs.pfnHookEvent("events/elite_left.sc", EV_FireEliteLeft);
	gEngfuncs.pfnHookEvent("events/elite_right.sc", EV_FireEliteRight);
	gEngfuncs.pfnHookEvent("events/knife.sc", EV_Knife);
	gEngfuncs.pfnHookEvent("events/galil.sc", EV_FireGalil);
	gEngfuncs.pfnHookEvent("events/famas.sc", EV_FireFamas);

	gEngfuncs.pfnHookEvent("events/explosion.sc", EV_Explosion);
	gEngfuncs.pfnHookEvent("events/te.sc", EV_TE);

	gEngfuncs.pfnHookEvent("events/m134.sc", EV_FireM134);
	gEngfuncs.pfnHookEvent("events/sprifle.sc", EV_FireSprifle);
	gEngfuncs.pfnHookEvent("events/m1887xmas.sc", EV_FireM1887XMAS);
	gEngfuncs.pfnHookEvent("events/skull4_left.sc", EV_FireSkull4Left);
	gEngfuncs.pfnHookEvent("events/skull4_right.sc", EV_FireSkull4Right);
	gEngfuncs.pfnHookEvent("events/dmp7a1_left.sc", EV_FireDMP7A1Left);
	gEngfuncs.pfnHookEvent("events/dmp7a1_right.sc", EV_FireDMP7A1Right);
	gEngfuncs.pfnHookEvent("events/skull3_left.sc", EV_FireSkull3Left);
	gEngfuncs.pfnHookEvent("events/skull3_right.sc", EV_FireSkull3Right);
	gEngfuncs.pfnHookEvent("events/skull1.sc", EV_FireSkull1);
	gEngfuncs.pfnHookEvent("events/flamethrower.sc", EV_FireFlameThrower);
	gEngfuncs.pfnHookEvent("events/sfsniper.sc", EV_FireSfSniper);
	gEngfuncs.pfnHookEvent("events/petrolboomer.sc", EV_FirePetrolBoomer);
	gEngfuncs.pfnHookEvent("events/sfpistol.sc", EV_FireSfPistol);
	gEngfuncs.pfnHookEvent("events/balrog11.sc", EV_FireBalrog11);
	gEngfuncs.pfnHookEvent("events/lightzg.sc", EV_FireLightZg);
	gEngfuncs.pfnHookEvent("events/heavyzg.sc", EV_FireHeavyZg);
	gEngfuncs.pfnHookEvent("events/railcannon.sc", EV_FireRailCannon);
	gEngfuncs.pfnHookEvent("events/chainsaw.sc", EV_FireChainsaw);
	gEngfuncs.pfnHookEvent("events/cannon.sc", EV_FireCannon);
	gEngfuncs.pfnHookEvent("events/ethereal.sc", EV_FireEthereal);
	gEngfuncs.pfnHookEvent("events/gilboaex.sc", EV_FireGilboaEX);
	gEngfuncs.pfnHookEvent("events/m1garand.sc", EV_FireM1Garand);
	gEngfuncs.pfnHookEvent("events/rpg7.sc", EV_FireRPG7);

	gEngfuncs.pfnHookEvent("events/destroyer.sc", EV_FireDestroyer);
	gEngfuncs.pfnHookEvent("events/airburster.sc", EV_FireAirburster);
	gEngfuncs.pfnHookEvent("events/spsmg.sc", EV_FireSpsmg);
	gEngfuncs.pfnHookEvent("events/sgdrill.sc", EV_FireSGDrill);

	gEngfuncs.pfnHookEvent("events/balrog11follow.sc", EV_Balrog11FollowEntities);
	gEngfuncs.pfnHookEvent("events/blockar.sc", EV_FireBlockAR);

	gEngfuncs.pfnHookEvent("events/stormgiant.sc", EV_StormGiant);
	gEngfuncs.pfnHookEvent("events/stormgiant_effect.sc", EV_StormGiantEffect);

	gEngfuncs.pfnHookEvent("events/buffak47.sc", EV_FireBuffAK47);
	gEngfuncs.pfnHookEvent("events/buffm4a1.sc", EV_FireBuffM4A1);
	gEngfuncs.pfnHookEvent("events/bloodhunter.sc", EV_FireBloodHunter);
	gEngfuncs.pfnHookEvent("events/bow.sc", EV_FireBow);
	gEngfuncs.pfnHookEvent("events/gaussspin.sc", EV_SpinGauss);
	gEngfuncs.pfnHookEvent("events/gauss.sc", EV_FireGauss);

	gEngfuncs.pfnHookEvent("events/buffsg552.sc", EV_FireBuffSG552);
	gEngfuncs.pfnHookEvent("events/cannonex.sc", EV_FireCannonEX);
	gEngfuncs.pfnHookEvent("events/runeblade.sc", EV_FireRuneblade);
	gEngfuncs.pfnHookEvent("events/runeblade_effect.sc", EV_RunebladeEffect);
	gEngfuncs.pfnHookEvent("events/thanatos9.sc", EV_FireThanatos9);
	gEngfuncs.pfnHookEvent("events/janusmk5.sc", EV_FireJanusmk5);
	gEngfuncs.pfnHookEvent("events/infinity.sc", EV_FireInfinity);
	gEngfuncs.pfnHookEvent("events/blocksmg.sc", EV_FireBlockSMG);
	gEngfuncs.pfnHookEvent("events/desperado.sc", EV_FireDesperado);
	gEngfuncs.pfnHookEvent("events/dualsword.sc", EV_FireDualSword);
	gEngfuncs.pfnHookEvent("events/janus11.sc", EV_FireJanus11);
	gEngfuncs.pfnHookEvent("events/gunkata.sc", EV_FireGunkata);
	gEngfuncs.pfnHookEvent("events/sgmissile.sc", EV_FireSgmissile);
	
	cl_gunbubbles = gEngfuncs.pfnRegisterVariable("cl_gunbubbles", "2", FCVAR_ARCHIVE);
	cl_tracereffect = gEngfuncs.pfnRegisterVariable("cl_tracereffect", "0", FCVAR_ARCHIVE);
}

void Event_VidInit(void);

enum event_effect
{
	EVENT_NONE,
	EVENT_PLAYSOUND,
	EVENT_KNIFE,
	EVENT_BALROG_EXPLOSION,
	EVENT_SPEARGUN,
	EVENT_Z4,
	EVENT_Z42,
	EVENT_BUFFAKM4HIT,
	EVENT_GUILLOTINE,
	EVENT_THANATOS5,
	EVENT_CROW9
};

//void (*g_pfnEV_HLDM_FireBullets)(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration) = (void (*)(int, float *, float *, float *, int, float *, float *, float *, float, int, int, int *, int))0x1902460;
//void (*g_pfnEV_EjectBrass)(float *origin, float *velocity, float rotation, int model, int soundtype, int idx, int angle_velocity) = (void (*)(float *, float *, float, int, int, int, int))0x1945180;

//void (*g_pfnEV_HLDM_CreateSmoke)(float *origin, float *dir, int speed, float scale, int r, int g, int b, int iSmokeType, float *base_velocity, bool bWind, int framerate) = (void (*)(float *, float *, int, float, int, int, int, int, float *, bool, int))0x19017D0;

cl_entity_s *GetViewEntity(void)
{
	return gEngfuncs.GetViewModel();
}

void SmokeRise_Wind(tempent_s *pEntity, float frametime, float currenttime)
{
	static vec3_t velocity;
	static int iSmokeDirectionX = 1, iSmokeDirectionY = 1;	// Default = 1, not constant...
	if (pEntity->entity.curstate.frame > 7.0)
	{
		pEntity->entity.baseline.origin *= 0.97;
		pEntity->entity.baseline.origin.z += 0.7;

		if (pEntity->entity.baseline.origin.z > 70.0)
		{
			pEntity->entity.curstate.origin.z = 70.0;
		}
	}
	if (pEntity->entity.curstate.frame > 6.0)
	{
		velocity.x += 0.075;
		if (velocity.x > 5.0)
			velocity.x = 5.0;

		if (!iSmokeDirectionX)
			pEntity->entity.baseline.origin.x -= velocity.x;
		else
			pEntity->entity.baseline.origin.x += velocity.x;

		velocity.y += 0.075;
		if (velocity.y > 5.0)
			velocity.y = 5.0;

		if (!iSmokeDirectionY)
			pEntity->entity.baseline.origin.y -= velocity.y;
		else
			pEntity->entity.baseline.origin.y += velocity.y;

		if (!gEngfuncs.pfnRandomLong(0, 10))
		{
			if (velocity.y > 3.0)
			{
				velocity.y = 0.0;
				iSmokeDirectionY = 1 - iSmokeDirectionY;
			}
		}

		if (!gEngfuncs.pfnRandomLong(0, 10))
		{
			if (velocity.x > 3.0)
			{
				velocity.x = 0.0;
				iSmokeDirectionX = 1 - iSmokeDirectionX;
			}
		}
	}
}

void SmokeRise_Wind_Expand(tempent_s *pEntity, float frametime, float currenttime)
{
	if (pEntity->entity.curstate.renderamt >= 75)
		pEntity->entity.curstate.renderamt -= gEngfuncs.pfnRandomLong(0, 9) == 0;
	if (pEntity->entity.curstate.scale < pEntity->entity.baseline.fuser1)
		pEntity->entity.curstate.scale *= 1.025;
}

void SmokeRise(tempent_s *pEntity, float frametime, float currenttime)
{
	if (pEntity->entity.curstate.frame > 7.0)
	{
		pEntity->entity.baseline.origin *= 0.97;
		pEntity->entity.baseline.origin.z += 0.7;

		if (pEntity->entity.baseline.origin.z > 70.0)
		{
			pEntity->entity.curstate.origin.z = 70.0;
		}
	}
}

void HugWalls(tempent_s *pEntity, pmtrace_t *tr)
{
	float flLength = pEntity->entity.baseline.origin.Length();

	if (flLength > 2000.0)
		flLength = 2000.0;
	vec3_t vecResult = pEntity->entity.baseline.origin.Normalize();

	pEntity->entity.baseline.origin.x = ((tr->plane.normal.x * vecResult.z - tr->plane.normal.z * vecResult.x) * tr->plane.normal.z - (tr->plane.normal.y * vecResult.z - tr->plane.normal.x * vecResult.y) * tr->plane.normal.y) * flLength * 1.5;
	pEntity->entity.baseline.origin.y = ((tr->plane.normal.y * vecResult.z - tr->plane.normal.x * vecResult.y) * tr->plane.normal.x - (tr->plane.normal.z * vecResult.y - tr->plane.normal.y * vecResult.z) * tr->plane.normal.z) * flLength * 1.5;
	pEntity->entity.baseline.origin.z = ((tr->plane.normal.z * vecResult.y - tr->plane.normal.y * vecResult.z) * tr->plane.normal.y - (tr->plane.normal.x * vecResult.z - tr->plane.normal.z * vecResult.x) * tr->plane.normal.x) * flLength * 1.5;
}

void EV_HLDM_CreateSmoke(float *origin, float *dir, int speed, float scale, int r, int g, int b, int iSmokeType, float *base_velocity, bool bWind, int framerate)
{
	char model[256];
	int model_index;
	TEMPENTITY *te;
	switch (iSmokeType)
	{
	case 1:
		strcpy(model, "sprites/black_smoke1.spr");

		switch (gEngfuncs.pfnRandomLong(0, 3))
		{
		case 0:
			model[19] = '1';
			break;
		case 1:
			model[19] = '2';
			break;
		case 2:
			model[19] = '3';
			break;
		case 3:
			model[19] = '4';
			break;
		}

		model_index = gEngfuncs.pEventAPI->EV_FindModelIndex(model);

		if (gEngfuncs.pfnGetCvarFloat("fastsprites") > 1.0)
		{
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/fast_wallpuff1.spr");
		}

		if (!model_index)
			model_index = 293;
		break;

	case 2:
		switch (gEngfuncs.pfnRandomLong(0, 3))
		{
		case 0:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff1.spr");
			break;
		case 1:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff2.spr");
			break;
		case 2:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff3.spr");
			break;
		case 3:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/wall_puff4.spr");
			break;
		}
		break;

	case 3:
		if (gEngfuncs.pfnRandomLong(0, 1))
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/pistol_smoke1.spr");
		else
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/pistol_smoke2.spr");
		break;

	case 4:
		switch (gEngfuncs.pfnRandomLong(0, 2))
		{
		case 0:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke1.spr");
		case 1:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke2.spr");
		case 2:
			model_index = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/rifle_smoke3.spr");
		}
		break;
	}

	te = gEngfuncs.pEfxAPI->R_DefaultSprite(origin, model_index, framerate);

	if (te)
	{
		te->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEWORLD;
		te->entity.curstate.scale = scale + gEngfuncs.pfnRandomFloat(0.0, scale / 2.0);

		if (iSmokeType > 1)
		{
			te->callback = bWind ? SmokeRise_Wind : SmokeRise;
			te->entity.curstate.rendermode = kRenderTransAdd;
			te->entity.curstate.renderamt = 150;
			te->entity.curstate.renderfx = kRenderFxFadeSlow;
		}
		else
		{
			if (gEngfuncs.pfnGetCvarFloat("fastsprites") == 1.0)
			{
				te->entity.curstate.rendermode = kRenderTransAdd;
				te->entity.curstate.renderamt = 200;
			}
			else
			{
				if (gEngfuncs.pfnGetCvarFloat("fastsprites") > 1.0)
				{
					te->entity.curstate.rendermode = kRenderNormal;
				}
				else
				{
					te->entity.curstate.rendermode = kRenderTransAlpha;
					te->entity.curstate.renderamt = 200;
				}
			}

			te->callback = SmokeRise_Wind_Expand;
			te->entity.baseline.fuser1 = (iSmokeType == 1) ? 1.0 : 8.5;

			if (gEngfuncs.pfnGetCvarFloat("fastsprites") > 1.0)
				te->entity.baseline.fuser1 -= 3.0;
		}

		te->hitcallback = HugWalls;
		te->entity.curstate.rendercolor.r = r;
		te->entity.curstate.rendercolor.g = g;
		te->entity.curstate.rendercolor.b = b;

		if (speed)
		{
			speed += gEngfuncs.pfnRandomLong(0, 5);
		}

		if (origin[0] != dir[0] && origin[1] != dir[1] && origin[2] != dir[2])
		{
			te->entity.origin.x = origin[0];
			te->entity.origin.y = origin[1];
			te->entity.origin.z = origin[2];

			te->entity.baseline.origin.x = dir[0] * speed;
			te->entity.baseline.origin.y = dir[1] * speed;
			te->entity.baseline.origin.z = dir[2] * speed;

			if (base_velocity)
			{
				te->entity.baseline.origin.x = dir[0] * speed + 0.9 * base_velocity[0];
				te->entity.baseline.origin.y = dir[1] * speed + 0.9 * base_velocity[1];
				te->entity.baseline.origin.z = dir[2] * speed + 0.5 * base_velocity[2];
			}
		}
	}
}

void EV_CreateTracer(float *start, float *end)
{
	gEngfuncs.pEfxAPI->R_TracerEffect(start, end);
}

char(*PM_FindTextureType)(char *name) = (char(*)(char *))0x195A540;

char EV_HLDM_PlayTextureSound(int idx, pmtrace_t *ptr, float *vecSrc, float *vecEnd, int iBulletType)
{
	// hit the world, try to play sound based on texture material type
	char chTextureType = CHAR_TEX_CONCRETE;
	float fvol;
	char *rgsz[4];
	int cnt;
	float fattn = ATTN_NORM;
	int entity;
	char *pTextureName;
	char texname[64];
	char szbuffer[64];

	entity = gEngfuncs.pEventAPI->EV_IndexFromTrace(ptr);

	// FIXME check if playtexture sounds movevar is set
	//

	chTextureType = 0;

	// Player
	if (entity >= 1 && entity <= gEngfuncs.GetMaxClients())
	{
		// hit body
		chTextureType = CHAR_TEX_FLESH;
	}
	else if (entity == 0)
	{
		// get texture from entity or world (world is ent(0))
		pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture(ptr->ent, vecSrc, vecEnd);

		if (pTextureName)
		{
			strcpy(texname, pTextureName);
			pTextureName = texname;

			// strip leading '-0' or '+0~' or '{' or '!'
			if (*pTextureName == '-' || *pTextureName == '+')
			{
				pTextureName += 2;
			}

			if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
			{
				pTextureName++;
			}

			// '}}'
			strcpy(szbuffer, pTextureName);
			szbuffer[CBTEXTURENAMEMAX - 1] = 0;

			// get texture type
			chTextureType = PM_FindTextureType(szbuffer);
		}
	}

	switch (chTextureType)
	{
	default:
	case CHAR_TEX_CONCRETE: fvol = 0.9;
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		rgsz[0] = "player/pl_step1.wav";
		rgsz[1] = "player/pl_step2.wav";
		cnt = 2;
		break;
	case CHAR_TEX_METAL: fvol = 0.9;
		rgsz[0] = "player/pl_metal1.wav";
		rgsz[1] = "player/pl_metal2.wav";
		cnt = 2;
		break;
	case CHAR_TEX_DIRT:	fvol = 0.9;
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		rgsz[0] = "player/pl_dirt1.wav";
		rgsz[1] = "player/pl_dirt2.wav";
		rgsz[2] = "player/pl_dirt3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_VENT:	fvol = 0.5;
		rgsz[0] = "player/pl_duct1.wav";
		rgsz[1] = "player/pl_duct1.wav";
		cnt = 2;
		break;
	case CHAR_TEX_GRATE: fvol = 0.9;
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		rgsz[0] = "player/pl_grate1.wav";
		rgsz[1] = "player/pl_grate4.wav";
		cnt = 2;
		break;
	case CHAR_TEX_TILE:	fvol = 0.8;
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		rgsz[0] = "player/pl_tile1.wav";
		rgsz[1] = "player/pl_tile3.wav";
		rgsz[2] = "player/pl_tile2.wav";
		rgsz[3] = "player/pl_tile4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_SLOSH: fvol = 0.9;
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		rgsz[0] = "player/pl_slosh1.wav";
		rgsz[1] = "player/pl_slosh3.wav";
		rgsz[2] = "player/pl_slosh2.wav";
		rgsz[3] = "player/pl_slosh4.wav";
		cnt = 4;
		break;
	case CHAR_TEX_WOOD: fvol = 0.9;
		rgsz[0] = "debris/wood1.wav";
		rgsz[1] = "debris/wood2.wav";
		rgsz[2] = "debris/wood3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_GLASS:
	case CHAR_TEX_COMPUTER:
		fvol = 0.8;
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		rgsz[0] = "debris/glass1.wav";
		rgsz[1] = "debris/glass2.wav";
		rgsz[2] = "debris/glass3.wav";
		cnt = 3;
		break;
	case CHAR_TEX_FLESH:
		if (iBulletType == BULLET_PLAYER_CROWBAR)
			return 0.0; // crowbar already makes this sound
		if (iBulletType == BULLET_PLAYER_OIL)
			return 0.0;
		fvol = 1.0;
		rgsz[0] = "weapons/bullet_hit1.wav";
		rgsz[1] = "weapons/bullet_hit2.wav";
		fattn = 1.0;
		cnt = 2;
		break;
	}

	// play material hit sound
	gEngfuncs.pEventAPI->EV_PlaySound(0, ptr->endpos, CHAN_STATIC, rgsz[gEngfuncs.pfnRandomLong(0, cnt - 1)], fvol, fattn, 0, 96 + gEngfuncs.pfnRandomLong(0, 0xf));
	return chTextureType;
}

char *EV_HLDM_DamageDecal(physent_t *pe)
{
	static char decalname[32];
	int idx;

	if (pe->classnumber == 1)
	{
		idx = gEngfuncs.pfnRandomLong(0, 2);
		sprintf(decalname, "{break%i", idx + 1);
	}
	else if (pe->rendermode != kRenderNormal)
	{
		sprintf(decalname, "{bproof1");
	}
	else
	{
		idx = gEngfuncs.pfnRandomLong(0, 4);
		sprintf(decalname, "{shot%i", idx + 1);
	}
	return decalname;
}

void EV_HLDM_GunshotDecalTrace(pmtrace_t *pTrace, char *decalName, char chTextureType)
{
	int iRand;
	physent_t *pe;

	gEngfuncs.pEfxAPI->R_BulletImpactParticles(pTrace->endpos);

	if (decalName && *decalName)
	{
		iRand = gEngfuncs.pfnRandomLong(0, 0x7FFF);
		if (iRand < (0x7fff / 2))// not every bullet makes a sound.
		{
			if (chTextureType == CHAR_TEX_METAL || chTextureType == CHAR_TEX_VENT)
			{
				int a = iRand & 0x80000001;
				if (a < 0)
				{
					if (((a - 1) | 0xFFFFFFFE) == -1)
						gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_AUTO, "weapons/ric_metal-2.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
					else
						gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, CHAN_AUTO, "weapons/ric_metal-1.wav", VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
				}
			}
			else
			{
				switch (iRand % 6)
				{
				case 0:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				case 1:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				case 2:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric3.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				case 3:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric4.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				case 4:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric5.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				case 5:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric_conc-1.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				case 6:	gEngfuncs.pEventAPI->EV_PlaySound(-1, pTrace->endpos, 0, "weapons/ric_conc-2.wav", 1.0, ATTN_NORM, 0, PITCH_NORM); break;
				}
			}
		}
	}

	pe = gEngfuncs.pEventAPI->EV_GetPhysent(pTrace->ent);

	// Only decal brush models such as the world etc.
	if (decalName && decalName[0] && pe && (pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP))
	{
		if (gEngfuncs.pfnGetCvarFloat("r_decals"))
		{
			gEngfuncs.pEfxAPI->R_DecalShoot(
				gEngfuncs.pEfxAPI->Draw_DecalIndex(gEngfuncs.pEfxAPI->Draw_DecalIndexFromName(decalName)),
				gEngfuncs.pEventAPI->EV_IndexFromTrace(pTrace), 0, pTrace->endpos, 0);
		}
	}
}

void EV_HLDM_DecalGunShot(pmtrace_t *pTrace, int iBulletType, float scale, int r, int g, int b, bool bStreakSplash, char chTextureType)
{
	int iColorIndex;
	char *decalname;

	physent_t *pe = gEngfuncs.pEventAPI->EV_GetPhysent(pTrace->ent);

	if (pe && pe->solid == SOLID_BSP)
	{
		if (bStreakSplash)
		{
			int iStreakCount = gEngfuncs.pfnRandomLong(15, 30);
			
			switch (iBulletType)
			{
			case BULLET_PLAYER_SFSNIPER:
				iColorIndex = 12;
				break;
			case BULLET_PLAYER_SFSNIPERM:
				iColorIndex = 1;
				break;
			case BULLET_PLAYER_ZOMBIEGUN:
				iColorIndex = 3;
				break;
			case BULLET_PLAYER_ETHEREAL:
				iColorIndex = 0;
				break;
			case BULLET_PLAYER_NNGAUSS_1:
				iColorIndex = 0;
				iStreakCount = 30;
			case BULLET_PLAYER_NNGAUSS_2:
				iColorIndex = 0;
				iStreakCount = 150;
				break;
			default:
				iColorIndex = 30;
				break;
			}

			gEngfuncs.pEfxAPI->R_StreakSplash(pTrace->endpos, gEngfuncs.pfnRandomFloat(4.0, 10.0) * pTrace->plane.normal, iColorIndex, iStreakCount, gEngfuncs.pfnRandomFloat(4.0, 10.0) * pTrace->plane.normal[2], -75, 75);
		}

		switch (iBulletType)
		{
		case BULLET_PLAYER_SFSNIPER:
		case BULLET_PLAYER_SFSNIPERM:
			decalname = "{sfshot";
			break;
		case BULLET_PLAYER_OIL:
			decalname = NULL;
			break;
		default:
			decalname = EV_HLDM_DamageDecal(pe);
			break;
		}

		EV_HLDM_GunshotDecalTrace(pTrace, decalname, chTextureType);
		EV_HLDM_CreateSmoke(pTrace->endpos + pTrace->plane.normal * 5.0, pTrace->plane.normal, 25, scale, r, g, b, 2, NULL, true, 35);
	}
}

int EV_HLDM_CheckTracer(int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount)
{
	int tracer = 0;
	int i;
	qboolean player = idx >= 1 && idx <= gEngfuncs.GetMaxClients() ? true : false;

	if (iTracerFreq != 0 && ((*tracerCount)++ % iTracerFreq) == 0)
	{
		vec3_t vecTracerSrc;

		if (player)
		{
			vec3_t offset(0, 0, -4);

			// adjust tracer position for player
			for (i = 0; i < 3; i++)
			{
				vecTracerSrc[i] = vecSrc[i] + offset[i] + right[i] * 2 + forward[i] * 16;
			}
		}
		else
		{
			VectorCopy(vecSrc, vecTracerSrc);
		}

		if (iTracerFreq != 1)		// guns that always trace also always decal
			tracer = 1;

		EV_CreateTracer(vecTracerSrc, end);
	}

	return tracer;
}

void EV_HLDM_FireBullets(int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float *vecSpread, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, int iPenetration, int iAttachment, bool lefthand, float srcofs)
{
	int i;
	pmtrace_t tr;
	int tracer;
	int iPenetrationPower;
	float flPenetrationDistance;
	float flCurrentDistance;
	vec3_t vecStart;
	char chTextureType = CHAR_TEX_GRATE;
	float flScale = 0.4;
	float flTempDistance;
	int iSourcePenetration = iPenetration;
	int r = 40, g = 40, b = 40;
	BOOL bCreatedEffects = FALSE;

	switch (iBulletType)
	{
	case BULLET_PLAYER_9MM:
		iPenetrationPower = 21;
		flPenetrationDistance = 800.0;
		break;
	case BULLET_PLAYER_45ACP:
		iPenetrationPower = 15;
		flPenetrationDistance = 500.0;
		break;
	case BULLET_PLAYER_50AE:
		iPenetrationPower = 30;
		flPenetrationDistance = 1000.0;
		break;
	case BULLET_PLAYER_762MM:
		iPenetrationPower = 39;
		flPenetrationDistance = 5000.0;
		break;
	case BULLET_PLAYER_556MM:
		iPenetrationPower = 35;
		flPenetrationDistance = 4000.0;
		break;
	case BULLET_PLAYER_338MAG:
		iPenetrationPower = 45;
		flPenetrationDistance = 8000.0;
		break;
	case BULLET_PLAYER_57MM:
		iPenetrationPower = 30;
		flPenetrationDistance = 2000.0;
		break;
	case BULLET_PLAYER_357SIG:
		iPenetrationPower = 25;
		flPenetrationDistance = 800.0;
		break;
	case BULLET_PLAYER_BUCKSHOT:
		iPenetrationPower = 8;
		flPenetrationDistance = 500.0;
		break;
	case BULLET_PLAYER_NNGAUSS_1:
	case BULLET_PLAYER_NNGAUSS_2:
	case BULLET_PLAYER_46MM:
		iPenetrationPower = 25;
		flPenetrationDistance = 500.0;
		break;
	case BULLET_PLAYER_SFSNIPER:
		iPenetrationPower = 100;
		flPenetrationDistance = 10000.0;
		break;
	case BULLET_PLAYER_ETHEREAL:
		iPenetrationPower = 35;
		flPenetrationDistance = 4000.0;
	default:
		iPenetrationPower = 0;
		flPenetrationDistance = 0.0;
		break;
	}

	if (lefthand == 0)
	{
		vecSrc[0] -= right[0] * srcofs;
		vecSrc[1] -= right[1] * srcofs;
		vecSrc[2] -= right[2] * srcofs;
	}
	else
	{
		vecSrc[0] += right[0] * srcofs;
		vecSrc[1] += right[1] * srcofs;
		vecSrc[2] += right[2] * srcofs;
	}

	int iPower = iPenetrationPower * 2;
	bool bStreakSplash = false;

	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");

	if (cShots >= 1)
	{
		if (gEngfuncs.pEventAPI->EV_IsLocal(idx - 1) && iBulletType != BULLET_PLAYER_OIL)
		{
			HudStatistics().m_iShoots++;
		}
		
		vecStart = vecSrc;
		int iShots = cShots;
		float x, y, z;
		vec3_t vecDir, vecEnd;

		while (iShots > 0)
		{
			if (iBulletType == BULLET_PLAYER_BUCKSHOT || iBulletType == BULLET_PLAYER_46MM || iBulletType == BULLET_PLAYER_NNGAUSS_1 || iBulletType == BULLET_PLAYER_RAILCANNON || iBulletType == BULLET_PLAYER_JANUS11)
			{
				do
				{
					x = gEngfuncs.pfnRandomFloat(-0.5, 0.5) + gEngfuncs.pfnRandomFloat(-0.5, 0.5);
					y = gEngfuncs.pfnRandomFloat(-0.5, 0.5) + gEngfuncs.pfnRandomFloat(-0.5, 0.5);
					z = x*x + y*y;
				} while (z > 1);
				for (i = 0; i < 3; i++)
				{
					vecDir[i] = vecDirShooting[i] + x * vecSpread[0] * right[i] + y * vecSpread[1] * up[i];
					vecEnd[i] = vecSrc[i] + flDistance * vecDir[i];
				}
			}
			else
			{
				for (i = 0; i < 3; i++)
				{
					vecDir[i] = vecDirShooting[i] + vecSpread[0] * right[i] + vecSpread[1] * up[i];
					vecEnd[i] = vecSrc[i] + flDistance * vecDir[i];
				}
			}

			iPenetration = iSourcePenetration;
			bCreatedEffects = FALSE;

			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
			gEngfuncs.pEventAPI->EV_PushPMStates();
			gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			bStreakSplash = false;

			while (iPenetration > 0)
			{
				gEngfuncs.pEventAPI->EV_PlayerTrace(vecStart, vecEnd, iBulletType == BULLET_PLAYER_NNGAUSS_1 ? PM_STUDIO_IGNORE : PM_NORMAL, -1, &tr);

				tracer = EV_HLDM_CheckTracer(idx, vecStart, vecEnd, forward, right, iBulletType, iTracerFreq, tracerCount);
				flCurrentDistance = tr.fraction * flDistance;
				if (flCurrentDistance <= 0.0)
					break;

				if (!bCreatedEffects)
				{
					bCreatedEffects = TRUE;
					vec3_t source;
					if (EV_IsLocal(idx))
						source = GetViewEntity()->attachment[iAttachment];
					else
						source = gEngfuncs.GetEntityByIndex(idx)->attachment[0];
#if 0
					if (iBulletType == BULLET_PLAYER_ETHEREAL)
					{
						particle_t *pParticle = gEngfuncs.pEfxAPI->R_TracerParticles(source, (tr.endpos - source).Normalize() * 6900, (tr.endpos - source).Length() / 6900);
						if (pParticle)
						{
							pParticle->type = pt_clientcustom;
							pParticle->color = 12;
							pParticle->ramp = 0.9;
						}
					}
#else
					if (iBulletType == BULLET_PLAYER_ETHEREAL)
					{
						BEAM *pBeam;
						pBeam = gEngfuncs.pEfxAPI->R_BeamEntPoint(idx | 0x1000, tr.endpos, iBeamModelIndex, 0.2, 0.4, 0.0, 1.0, 0, 0, 0, 120 / 255.0, 180 / 255.0, 255 / 255.0);


						if (pBeam)
							pBeam->flags |= FBEAM_FADEIN;
#endif
					}

					if (iBulletType == BULLET_PLAYER_NNGAUSS_1 || iBulletType == BULLET_PLAYER_NNGAUSS_2)
					{
						BEAM *pBeam;
						pBeam = gEngfuncs.pEfxAPI->R_BeamEntPoint(idx | 0x1000, tr.endpos, iBeamModelIndex, 0.2, 0.4, 0.0, 1.0, 0, 0, 0, 120 / 255.0, 180 / 255.0, 255 / 255.0);

						int iRailModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/hotglow_railcannon.spr");

						if (pBeam)
							pBeam->flags |= FBEAM_FADEIN;

						if (iBulletType == BULLET_PLAYER_NNGAUSS_1)
						{
							gEngfuncs.pEfxAPI->R_TempSprite(tr.endpos, forward, 0.1 + 0.2 * lefthand / 12, iRailModelIndex, kRenderGlow, kRenderFxNoDissipation, 0.7 + 0.3 * lefthand / 12, 0.7 + 3.0 * lefthand / 12, FTENT_FADEOUT);
							gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, tr.endpos, tr.endpos, iRailModelIndex, 3 + 6 * lefthand / 12, 0.1, gEngfuncs.pfnRandomFloat(10, 15) / 100.0, 100, 200, 100);
							gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, tr.endpos, source, iRailModelIndex, 3, 0.1, gEngfuncs.pfnRandomFloat(7, 12) / 100.0, 100, 180, 100);
						}
						else
						{
							//gEngfuncs.pEfxAPI->R_TempSprite( tr.endpos, forward, 0.1, m_iGlow, kRenderGlow, kRenderFxNoDissipation, 0.7, 0.7, FTENT_FADEOUT );
							gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, tr.endpos, tr.endpos, iRailModelIndex, 1, 0.1, 0.1, 100, 150, 100);
						}
					}

					if (iBulletType == BULLET_PLAYER_RAILCANNON)
					{
						BEAM *pBeam;
						pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, tr.endpos, iBeamModelIndex, 0.5, 0.3, 0.0, 1.0, 0, 0, 0, 78 / 255.0, 37 / 255.0, 29 / 255.0);

						if (pBeam)
							pBeam->flags |= FBEAM_FADEIN;

					}

					if (iBulletType == BULLET_PLAYER_JANUS11)
					{
						BEAM *pBeam;
						pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, tr.endpos, iBeamModelIndex, 0.5, 0.3, 0.0, 1.0, 0, 0, 0, 78 / 255.0, 37 / 255.0, 29 / 255.0);

						if (pBeam)
							pBeam->flags |= FBEAM_FADEIN;

					}

					if (cl_tracereffect->value /*&& iBulletType != BULLET_PLAYER_ETHEREAL*/ && iBulletType != BULLET_PLAYER_NNGAUSS_1 && iBulletType != BULLET_PLAYER_NNGAUSS_2 && iBulletType != BULLET_PLAYER_OIL && iBulletType != BULLET_PLAYER_RAILCANNON)
						gEngfuncs.pEfxAPI->R_TracerParticles(source, (tr.endpos - source).Normalize() * 6000, (tr.endpos - source).Length() / 6000);
					if (tr.ent > 0 && EV_IsLocal(idx))
					{
						physent_t *ent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

						if (ent)
						{
							if (ent->info >= 1 && ent->info <= 32)
							{
								if ((vPlayer[ent->info].team != g_iTeam || g_iMod == MOD_DM))
									HudStatistics().m_iHit++;

								if (cl_crosshair_hit->value)
								if ((vPlayer[ent->info].team != g_iTeam || g_iMod == MOD_DM) && (g_iMod != MOD_GHOST || vPlayer[ent->info].team != 2))
									HudAmmo().Hit(cl.time);
							}
						}
					}
					if (cl_gunbubbles->value)
					{
						Vector end, dir;
						dir = vecDir;

						for (int i = 0; i < 3; i++)
						{
							end[i] = tr.endpos[i] - 5.0 * vecDir[i];
						}

						if (gEngfuncs.PM_PointContents(end, NULL) == CONTENTS_WATER)
						{
							int sModelIndexBubbles = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/bubble.spr");

							if (cl_gunbubbles->value == 2)
							{
								cl_entity_t *ent;
								vec3_t src = vecSrc;
								
								if (EV_IsLocal(idx))
								{
									ent = GetViewEntity();
									src = ent->attachment[iAttachment];
									dir = (end - src).Normalize();
								}

								vec3_t dir = (end - src).Normalize();

								float height, distance;

								if (gEngfuncs.PM_PointContents(src, NULL) == CONTENTS_WATER)
								{
									height = src.z - end.z;
									distance = (end - src).Length();

									if (height < 20)
										height = 20;
								}
								else
								{
									Vector test = end;

									while (gEngfuncs.PM_PointContents(test, NULL) == CONTENTS_WATER)
									{
										test.z += 2;
									}

									height = test.z - end.z - 5 - 2;
									distance = (end - src).Length();

									src = end - dir * (height / (src.z - end.z) * distance);
								}

								int count = (src - end).Length() / 8;

								if (count && height)
									gEngfuncs.pEfxAPI->R_BubbleTrail(src, end, height, sModelIndexBubbles, count, 2.0);
							}
							else
							{
								Vector mins, maxs;
								mins.x = tr.endpos[0] - 2.0;
								mins.y = tr.endpos[1] - 2.0;
								mins.z = tr.endpos[2] - 2.0;
								maxs.x = tr.endpos[0] + 15.0;
								maxs.y = tr.endpos[1] + 15.0;
								maxs.z = tr.endpos[2] + 15.0;

								gEngfuncs.pEfxAPI->R_Bubbles(mins, maxs, 20, sModelIndexBubbles, 10, 2.0);
							}
						}
					}
				}

				iPenetration--;
				if (flCurrentDistance > flPenetrationDistance)
					iPenetration = 0;
				flTempDistance = flCurrentDistance;
				switch (iBulletType)
				{
				case BULLET_PLAYER_9MM:
					if (!tracer)
						chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecStart, vecEnd, iBulletType);
					break;
				case BULLET_PLAYER_BUCKSHOT:
					break;
				case BULLET_PLAYER_MP5:
					if (!tracer)
						chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecStart, vecEnd, iBulletType);
					break;
				case BULLET_PLAYER_357:
					if (!tracer)
						chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecStart, vecEnd, iBulletType);
					break;
				case BULLET_PLAYER_NNGAUSS_1:
				case BULLET_PLAYER_NNGAUSS_2:
				case BULLET_PLAYER_RAILCANNON:
				case BULLET_PLAYER_46MM:
					chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecStart, vecEnd, iBulletType);
					break;
				default:
					chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecStart, vecEnd, iBulletType);
					break;
				}

				switch (chTextureType)
				{
				case CHAR_TEX_METAL:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.15;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_CONCRETE:
					bStreakSplash = true;
					r = g = b = 65;
					flCurrentDistance = iPower * 0.25;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_GRATE:
				case CHAR_TEX_VENT:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.5;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_TILE:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.65;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_COMPUTER:
					bStreakSplash = true;
					flCurrentDistance = iPower * 0.4;
					iPower = flCurrentDistance;
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_WOOD:
					bStreakSplash = false;
					r = 75;
					g = 42;
					b = 15;
					flScale = 0.5;
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				default:
					EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				}
				
				if (!iPenetration)
					break;
				flDistance = (flDistance - flTempDistance) * 0.5;
				vecStart = iPower * vecDir + tr.endpos - vecDir;
				vecEnd = vecDir * flDistance + vecStart;

				float flDepth = 1.0;

				if (iPower > 1)
				{
					while (1)
					{
						gEngfuncs.pEventAPI->EV_SetTraceHull(2);
						gEngfuncs.pEventAPI->EV_PlayerTrace(tr.endpos + vecDir * flDepth, tr.endpos + vecDir * flDepth * 2, PM_NORMAL, -1, &tr);
						if (tr.startsolid)
						{
							if (tr.inopen)
								break;
						}
						flDepth += 1.0;
						if (flDepth >= iPower)
							break;
					}
					if (flDepth < iPower)
						EV_HLDM_DecalGunShot(&tr, iBulletType, flScale, r, g, b, bStreakSplash, chTextureType);
				}
				continue;
			}
			gEngfuncs.pEventAPI->EV_PopPMStates();
			iShots--;
		}
	}
}

void EV_CustomEffect(struct event_args_s *args, int effect)
{	
	if (effect & (1 << CWeaponManager::EFFECT_SHAKE))
	{
		MSG_BeginWrite("ScreenShake");
		MSG_WriteShort((1<<12)*4);
		MSG_WriteShort((1<<12));
		MSG_WriteShort((1<<12)*4);
		MSG_EndWrite();
	}


	if (effect & (1 << CWeaponManager::EFFECT_TRACER_ETHEREAL))
	{		
		//gEngfuncs.pEfxAPI->R_UserTracerParticle(
	}
}

void EV_SPR_FOLLOW_ATTACHMENT ( struct tempent_s *ent, float frametime, float currenttime );
void EV_SPR ( struct tempent_s *ent, float frametime, float currenttime );
void EV_SPR2 ( struct tempent_s *ent, float frametime, float currenttime );
void EV_SPR3 ( struct tempent_s *ent, float frametime, float currenttime );
void EV_SPR4 ( struct tempent_s *ent, float frametime, float currenttime );
void EV_SPR5 ( struct tempent_s *ent, float frametime, float currenttime );

void EV_SPR_FOLLOW_ATTACHMENT ( struct tempent_s *ent, float frametime, float currenttime )
{
	cl_entity_s *viewent;
	int i = ent->entity.curstate.iuser1;
	viewent = gEngfuncs.GetViewModel();

	ent->entity.origin[0] = viewent->attachment[i][0];
	ent->entity.origin[1] = viewent->attachment[i][1];
	ent->entity.origin[2] = viewent->attachment[i][2];
}

void EV_SPR4 ( struct tempent_s *ent, float frametime, float currenttime )
{	
	if((ent->die - cl.time) < 1.0)
	{
		ent->entity.curstate.rendermode = kRenderTransAlpha;
		ent->entity.curstate.renderamt = 255.0 * (ent->die - cl.time) / 1.0;
		ent->entity.curstate.renderamt = ent->entity.curstate.renderamt < 0 ? 0 : ent->entity.curstate.renderamt;
	}
}

void EV_SPR ( struct tempent_s *ent, float frametime, float currenttime )
{	
	if((ent->die - cl.time) < 0.4)
	{		
		ent->entity.curstate.renderamt = 255.0 * (ent->die - cl.time) / 0.4;
		ent->entity.curstate.renderamt = ent->entity.curstate.renderamt < 0 ? 0 : ent->entity.curstate.renderamt;
	}
}

void EV_SPR2 ( struct tempent_s *ent, float frametime, float currenttime )
{
	if((ent->die - cl.time) < ent->entity.curstate.fuser2)
	{
		ent->entity.curstate.renderamt = 255.0 * (ent->die - cl.time);
	}
}

void EV_SPR3 ( struct tempent_s *ent, float frametime, float currenttime )
{
	if((ent->die - cl.time) < ent->entity.curstate.fuser2)
	{
		ent->entity.curstate.renderamt = 255.0 * (ent->die - cl.time) / ent->entity.curstate.fuser2;
	}
	
	ent->entity.origin[2] += 0.7;
	ent->entity.curstate.scale += 0.007;

	if((cl.time - ent->entity.curstate.fuser1) > (1.0 / ent->entity.curstate.framerate))
	{
		if(ent->entity.curstate.frame + 1 > ent->entity.curstate.iuser1)
			ent->entity.curstate.frame = 0;
		else
			ent->entity.curstate.frame += 1;

		ent->entity.curstate.fuser1 = cl.time;
	}
}

void EV_SPR5 ( struct tempent_s *ent, float frametime, float currenttime )
{
	if((ent->die - cl.time) < ent->entity.curstate.fuser2)
	{
		ent->entity.curstate.renderamt = 255.0 * (ent->die - cl.time) / ent->entity.curstate.fuser2;
	}	

	/*if((cl.time - ent->entity.curstate.fuser1) > (1.0 / ent->entity.curstate.framerate))
	{
		if(ent->entity.curstate.frame + 1 > ent->frameMax)
			ent->entity.curstate.frame = 0;
		else
			ent->entity.curstate.frame += 1;

		ent->entity.curstate.fuser1 = cl.time;
	}*/
}

void EV_SPR6(struct tempent_s *ent, float frametime, float currenttime)
{
	if ((ent->die - cl.time) < ent->entity.curstate.fuser2)
	{
		ent->entity.curstate.renderamt = 100.0 * (ent->die - cl.time) / ent->entity.curstate.fuser2;

		ent->entity.curstate.scale = (ent->entity.curstate.iuser1 ? 1.0 : 0.5)* (ent->die - cl.time) / ent->entity.curstate.fuser2;
	}
	
	if (!ent->entity.curstate.iuser1)
	{
		Vector origin = ent->entity.origin;
		origin.z = origin.z - 25.0;

		if (gEngfuncs.PM_PointContents(origin, NULL) != CONTENTS_SOLID)
			ent->entity.origin[2] -= ent->entity.curstate.fuser3;
	}
}

void DefaultScaleThink(tempent_s *ent, float frametime, float currenttime)
{
	if (ent->entity.baseline.fuser1 > ent->entity.curstate.scale)
		ent->entity.curstate.scale += 0.03;
}

void DefaultFramerateTouch(tempent_s *ent, pmtrace_t *tr)
{
	ent->entity.baseline.origin *= 0.01f;
	ent->entity.curstate.framerate = 60;
}

void EV_TE(struct event_args_s *args)
{
	struct model_s *pModel;
	TEMPENTITY *pEnt;
	
	switch (args->iparam1)
	{
		case 0: pModel = IEngineStudio.Mod_ForName("models/cso_bolt.mdl", 0); break;
		case 1: pModel = IEngineStudio.Mod_ForName("models/spear.mdl", 0); break;
	}

	pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(args->origin, pModel);
	VectorCopy(args->angles, pEnt->entity.angles);
	pEnt->die = cl.time + 5.0;

	pEnt->flags |= FTENT_CLIENTCUSTOM;

	if (args->iparam2)
	{
		pEnt->tentOffset[0] = pEnt->tentOffset[1] = pEnt->tentOffset[2] = 0.0;
		
		cl_entity_t *pEntity = NULL;
		pEntity = gEngfuncs.GetEntityByIndex(args->iparam2);
		if(pEntity)
		{
			pEnt->tentOffset[0] = args->origin[0] - pEntity->origin[0];
			pEnt->tentOffset[1] = args->origin[1] - pEntity->origin[1];
			pEnt->tentOffset[2] = args->origin[2] - pEntity->origin[2];
		}
		
		pEnt->clientIndex = args->iparam2;
		pEnt->flags |= FTENT_PLYRATTACHMENT;		
	}
	
	pEnt->callback = EV_SPR4;
}

void EV_Explosion(struct event_args_s *args)
{
	int idx;
	Vector origin;
	int type;

	idx = args->entindex;
	type = args->iparam1;

	gEngfuncs.CL_LoadModel("sprites/eexplo.spr", &sModelIndexFireball2);
	gEngfuncs.CL_LoadModel("sprites/fexplo.spr", &sModelIndexFireball3);

	switch (type)
	{
		case 0:
		{
			VectorCopy(args->origin, origin);
			origin[2] += 20.0;
			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 2.5, 30, TE_EXPLFLAG_NOSOUND);

			VectorCopy(args->origin, origin);
			origin[0] += gEngfuncs.pfnRandomFloat(-64.0, 64.4);
			origin[1] += gEngfuncs.pfnRandomFloat(-64.0, 64.0);
			origin[2] += gEngfuncs.pfnRandomFloat(-64.0, 64.0);
			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 3.0, 30, TE_EXPLFLAG_NONE);
			break;
		}
		case 1:
		{
			VectorCopy(args->origin, origin);
			origin[2] += 20.0;
			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 2.5, 30, TE_EXPLFLAG_NOSOUND);

			VectorCopy(args->origin, origin);
			origin[0] += gEngfuncs.pfnRandomFloat(-64.0, 64.4);
			origin[1] += gEngfuncs.pfnRandomFloat(-64.0, 64.0);
			origin[2] += gEngfuncs.pfnRandomFloat(-64.0, 64.0);
			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 3.0, 30, TE_EXPLFLAG_NOSOUND);
			
			VectorCopy(args->origin, origin);
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/janus1_exp.wav", 1.0, 0.3, 0, 100);

			break;
		}
		case 2:
		{
			float vColor[3][3];

			vColor[0][0] = vColor[0][1]= 255.0;
			vColor[0][2] = 1.0;
			vColor[1][0] = vColor[1][1] = vColor[1][2] = 100.0;
			vColor[2][0] = 50.0;
			vColor[2][1] = 200.0;
			vColor[2][2] = 50.0;

			for (int i = 0; i < 3; i++)
			{
				vec3_t vOrigin;
				
				VectorCopy(args->origin, origin);
				vOrigin[0] = origin[0] + gEngfuncs.pfnRandomFloat(-100.0, 100.0);
				vOrigin[1] = origin[1] + gEngfuncs.pfnRandomFloat(-100.0, 100.0);
				vOrigin[2] = origin[2] + gEngfuncs.pfnRandomFloat(20.0, 70.0);
				
				struct model_s *pModel;
				TEMPENTITY *pEnt;

				pModel = IEngineStudio.Mod_ForName("sprites/spark1.spr", 0);

				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(vOrigin, pModel);
				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 254;
				pEnt->entity.curstate.scale = 0.7;
				pEnt->entity.curstate.rendercolor.r = vColor[i][0];
				pEnt->entity.curstate.rendercolor.g = vColor[i][1];
				pEnt->entity.curstate.rendercolor.b = vColor[i][2];
				pEnt->flags |= FTENT_CLIENTCUSTOM;
				pEnt->callback = EV_SPR3;

				pEnt->entity.curstate.iuser1 = ModelFrameCount(pModel);
				pEnt->entity.curstate.framerate = 15.0;
				pEnt->entity.curstate.fuser1 = cl.time;
				pEnt->entity.curstate.fuser2 = 0.2;

				pEnt->die = cl.time + pEnt->entity.curstate.iuser1 / 15.0;
			}

			struct model_s *pModel;
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/spark1.spr", 0);

			VectorCopy(args->origin, origin);
			origin[2] += 10.0;

			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 255;
			pEnt->entity.curstate.scale = 0.7;
			pEnt->entity.curstate.fuser2 = 0.6;
			pEnt->flags |= FTENT_CLIENTCUSTOM;
			pEnt->callback = EV_SPR3;

			pEnt->entity.curstate.iuser1 = ModelFrameCount(pModel);
			pEnt->entity.curstate.framerate = 15.0;
			pEnt->entity.curstate.fuser1 = cl.time;
			pEnt->entity.curstate.fuser2 = 0.2;

			pEnt->die = cl.time + pEnt->entity.curstate.iuser1 / 15.0;

			VectorCopy(args->origin, origin);
			gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/firecracker_explode.wav", 1.0, 0.3, 0, 100);

			break;
		}
		case 3:
		{
			break;
		}
		case 4:
		{
			VectorCopy(args->origin, origin);
			origin[2] += 30.0;
			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 1.5, 30, TE_EXPLFLAG_NOSOUND);

			struct model_s *pModel;
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/flame3.spr", 0);

			origin[2] += 25.0;

			for (int i=0; i<8; i++)
			{
				VectorCopy(args->origin, origin);
				if (i < 1)
					origin[2] += 45.0;
				else
					origin[2] += 25.0;

				if (i >= 4)
				{
					origin[0] += gEngfuncs.pfnRandomFloat(-48.0, 48.0);
					origin[1] += gEngfuncs.pfnRandomFloat(-48.0, 48.0);
				}
				else if (i >= 1)
				{
					origin[0] += gEngfuncs.pfnRandomFloat(-28.0, 28.0);
					origin[1] += gEngfuncs.pfnRandomFloat(-28.0, 28.0);
				}

				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 255;
				if (i < 1)
					pEnt->entity.curstate.scale = 1.5;
				else
					pEnt->entity.curstate.scale = 1.0;

				pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PERSIST | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP/* | FTENT_GRAVITY | FTENT_COLLIDEALL*/;
				pEnt->callback = EV_SPR5;

				pEnt->frameMax = ModelFrameCount(pModel);
				pEnt->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pEnt->frameMax - 1);
				pEnt->entity.curstate.framerate = 15.0;				
				pEnt->entity.curstate.fuser1 = cl.time;
				pEnt->entity.curstate.fuser2 = 0.6;

				pEnt->die = cl.time + 6.0;
			}
			
			gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/petrolboomer_explosion.wav", 1.0, 0.3, 0, 100);

			break;
		}
		case 5:
		{
			VectorCopy(args->origin, origin);
			origin[2] += 30.0;
			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 3.5, 20, TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NODLIGHTS);

			struct model_s *pModel;
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/rpg7_exp.spr", 0);

			VectorCopy(args->origin, origin);
			origin[2] += 20.0;

			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 255;
			pEnt->entity.curstate.scale = 2.5;
			//pEnt->entity.curstate.fuser2 = 0.6;
			pEnt->flags |= FTENT_SPRANIMATE;
			pEnt->frameMax = ModelFrameCount(pModel);
			//pEnt->callback = EV_SPR3;*/

			//pEnt->entity.curstate.iuser1 = ModelFrameCount(pModel);
			pEnt->entity.curstate.framerate = 8.0;
			//pEnt->entity.curstate.fuser1 = cl.time;
			//pEnt->entity.curstate.fuser2 = 0.2;

			pEnt->die = cl.time + pEnt->frameMax / 8.0;

			VectorCopy(args->origin, origin);
			gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/rpg7_explosion.wav", 1.0, 0.1, 0, 100);

			dlight_t *dl = gEfxAPI.CL_AllocDlight(0);
			VectorCopy(origin, dl->origin);
			dl->radius = 400;
			dl->color.r = 255;
			dl->color.g = 190;
			dl->color.b = 40;
			dl->die = cl.time + 2.0;
			dl->decay = 200;

			break;
		}
		case 6:
		{
			VectorCopy(args->origin, origin);

			Vector forward = { 1, 0, 0 };
			Vector right = { 0, 1, 0 };
			Vector up = { 0, 0, 1 };
			
			float time = 15.0;
			float range = 100.0;
			float client_time = cl.time;
			
			
			//origin[2] += 30.0;

			dlight_t *dl = gEfxAPI.CL_AllocDlight(0);
			VectorCopy(origin, dl->origin);
			dl->radius = range * 2.5;
			dl->color.r = 255;
			dl->color.g = 190;
			dl->color.b = 40;
			dl->die = client_time + time;
			dl->decay = 0;

			gEngfuncs.pEfxAPI->R_Explosion(origin, sModelIndexFireball3, 1.5, 30, TE_EXPLFLAG_NOSOUND);

			struct model_s *pModel;
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/flame3.spr", 0);

			VectorCopy(args->origin, origin);
			origin[2] += 25.0;

			int num = 5;
			float f = range / num;
			for (int i = -num; i <= num; i++)
			{
				for (int j = -num; j <= num; j++)
				{
					Vector origin2 = origin;
					origin2 = origin2 + forward * i * f + right * j * f;

					if ((origin2 - origin).Length2D() > range)
						continue;

					origin2[2] += ((i == 0 && j == 0) ? 45.0 : 5.0);

					origin2[0] += gEngfuncs.pfnRandomFloat(-7.0, 7.0);
					origin2[1] += gEngfuncs.pfnRandomFloat(-7.0, 7.0);

					pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin2, pModel);

					if (!pEnt)
						return;

					pEnt->entity.curstate.rendermode = kRenderTransAdd;
					pEnt->entity.curstate.renderamt = 100;

					pEnt->entity.curstate.scale = ((i == 0 && j == 0) ? 1.0 : 0.5);

					pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP | FTENT_PERSIST;
					pEnt->callback = EV_SPR6;

					pEnt->frameMax = ModelFrameCount(pModel);
					pEnt->entity.curstate.frame = gEngfuncs.pfnRandomLong(0, pEnt->frameMax - 1);
					pEnt->entity.curstate.framerate = 15.5;
					pEnt->entity.curstate.fuser1 = client_time;
					pEnt->entity.curstate.fuser2 = 0.6;
					pEnt->entity.curstate.fuser3 = gEngfuncs.pfnRandomFloat(2.0, 3.0);
					pEnt->entity.curstate.iuser1 = (i == 0 && j == 0);

					pEnt->die = client_time + time;
				}
			}
			gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/molotov_fire.wav", 1.0, 1.3, 0, 100);

			break;
		}

	}	
}

enum usp_e
{
	USP_IDLE,
	USP_SHOOT1,
	USP_SHOOT2,
	USP_SHOOT3,
	USP_SHOOT_EMPTY,
	USP_RELOAD,
	USP_DRAW,
	USP_ATTACH_SILENCER,
	USP_UNSIL_IDLE,
	USP_UNSIL_SHOOT1,
	USP_UNSIL_SHOOT2,
	USP_UNSIL_SHOOT3,
	USP_UNSIL_SHOOT_EMPTY,
	USP_UNSIL_RELOAD,
	USP_UNSIL_DRAW,
	USP_DETACH_SILENCER
};

void EV_FireUSP(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	int silencer_on;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	vec3_t smoke_origin;
	float base_scale;
	

	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	silencer_on = args->bparam2;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);

		if (silencer_on)
		{
			if (!empty)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(USP_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(USP_SHOOT_EMPTY, 2);
		}
		else
		{
			EV_MuzzleFlash();

			if (!empty)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(USP_UNSIL_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(USP_UNSIL_SHOOT_EMPTY, 2);
		}

		VectorCopy(ent->attachment[0], smoke_origin);

		smoke_origin[0] -= forward[0] * 3;
		smoke_origin[1] -= forward[1] * 3;
		smoke_origin[2] -= forward[2] * 3;

		base_scale = gEngfuncs.pfnRandomFloat(0.1, 0.25);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, smoke_origin[2], 7, 7, 7, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 20, base_scale + 0.1, 10, 10, 10, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, base_scale, 13, 13, 13, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		if (lefthand == 0)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, -14.0, false);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 36.0, -14.0, 14.0, false);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.5, ShellVelocity);
		ShellVelocity[2] += 45.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	if (silencer_on)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	else
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSoundSilen, 1.0, 2.0, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 4096, BULLET_PLAYER_45ACP, 0, 0, 2);
}

enum mp5n_e
{
	MP5N_IDLE1,
	MP5N_RELOAD,
	MP5N_DRAW,
	MP5N_SHOOT1,
	MP5N_SHOOT2,
	MP5N_SHOOT3
};

void EV_FireMP5N(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(MP5N_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.25, 10, 10, 10, 3, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {34.0, -10.0, 11.0};
		
		//EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
		//
		if (WeaponData.bHaveShellVec)
		{
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
		}
		else
		{
			if (lefthand == 0)
				vecScale[2] = -vecScale[2];

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
			ShellVelocity[2] -= 50;
		}

		VectorCopy(ent->attachment[1], ShellOrigin);
		
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 13);
	/*float angle2[3];
	angle2[0] = angle2[1] = angle2[2] = 0;
	angle2[1] = angles[YAW];
	gEngfuncs.pEfxAPI->R_TempModel(ShellOrigin, ShellVelocity, angle2, 20.5, shell, TE_BOUNCE_SHELL);*/

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.64, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_9MM, 0, 0, 2);
}

enum ak47_e
{
	AK47_IDLE1,
	AK47_RELOAD,
	AK47_DRAW,
	AK47_SHOOT1,
	AK47_SHOOT2,
	AK47_SHOOT3
};

void EV_FireAK47(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(AK47_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -8.0, -13.0};

		if (lefthand == 0)
			vecScale[2] = -vecScale[2];

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 75;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);

	//gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, WeaponData.fSoundVolume, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;
	
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8196, BULLET_PLAYER_762MM, 0, 0, 2);
}

enum aug_e
{
	AUG_IDLE1,
	AUG_RELOAD,
	AUG_DRAW,
	AUG_SHOOT1,
	AUG_SHOOT2,
	AUG_SHOOT3
};

void EV_FireAug(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(AUG_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {17.0, -8.0, -14.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
				
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 122;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 8);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

enum skull1_e
{
	SKULL1_IDLE1,
	SKULL1_SHOOT1,
	SKULL1_SHOOT2,
	SKULL1_SHOOT3,
	SKULL1_RELOAD,
	SKULL1_DRAW,
	SKULL1_SP_SHHOT1,
	SKULL1_SP_SHHOT2
};

void EV_FireSkull1(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (g_iButton)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL1_SP_SHHOT1 + gEngfuncs.pfnRandomLong(0, 1), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL1_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
		
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {35.0, -11.0, -16.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] += 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2);
}

enum deagle_e
{
	DEAGLE_IDLE1,
	DEAGLE_SHOOT1,
	DEAGLE_SHOOT2,
	DEAGLE_SHOOT_EMPTY,
	DEAGLE_RELOAD,
	DEAGLE_DRAW
};

void EV_FireDeagle(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (empty)
		{
			if (WeaponData.iAnimEmpty)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(WeaponData.iAnimEmpty, 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(DEAGLE_SHOOT_EMPTY, 2);
		}
		else
		{			
			if (WeaponData.bHaveAnim)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(DEAGLE_SHOOT1 + gEngfuncs.pfnRandomLong(0, 1), 2);
		}

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}

	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {35.0, -11.0, -16.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] += 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2);
}

enum g3sg1_e
{
	G3SG1_IDLE,
	G3SG1_SHOOT,
	G3SG1_SHOOT2,
	G3SG1_RELOAD,
	G3SG1_DRAW
};

void EV_FireG3SG1(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(G3SG1_SHOOT + gEngfuncs.pfnRandomLong(0, 1), 2);
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.3, 35, 35, 35, 4, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 35, 0.35, 30, 30, 30, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 70, 0.3, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -8.0, -10.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
		
		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.5, ShellVelocity);
		ShellVelocity[2] -= 50.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 17);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_762MM, 0, 0, 2);
}

enum sg550_e
{
	SG550_IDLE,
	SG550_SHOOT,
	SG550_SHOOT2,
	SG550_RELOAD,
	SG550_DRAW
};

void EV_FireSG550(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
	
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		
		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SG550_SHOOT + gEngfuncs.pfnRandomLong(0, 1), 2);
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.3, 35, 35, 35, 4, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 35, 0.35, 30, 30, 30, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 70, 0.3, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -8.0, -10.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.5, ShellVelocity);
		ShellVelocity[2] -= 50.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 17);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

enum glock18_e
{
	GLOCK18_IDLE1,
	GLOCK18_IDLE2,
	GLOCK18_IDLE3,
	GLOCK18_SHOOT,
	GLOCK18_SHOOT2,
	GLOCK18_SHOOT3,
	GLOCK18_SHOOT_EMPTY,
	GLOCK18_RELOAD,
	GLOCK18_DRAW,
	GLOCK18_HOLSTER,
	GLOCK18_ADD_SILENCER,
	GLOCK18_DRAW2,
	GLOCK18_RELOAD2
};

void EV_FireGlock18(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	


	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (empty)
		{
			if (WeaponData.iAnimEmpty)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(WeaponData.iAnimEmpty, 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(GLOCK18_SHOOT_EMPTY, 2);
		}
		else
		{			
			if (WeaponData.bHaveAnim)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(GLOCK18_SHOOT3, 2);
		}
		
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.2, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.4, 20, 20, 20, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {36.0, -14.0, -14.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];		

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.65, ShellVelocity);
		ShellVelocity[2] += 25.0;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 4);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 4096, BULLET_PLAYER_9MM, 0, 0, 2);
}

void EV_FireM134(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);	

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);	

	if (EV_IsLocal(idx))
	{		
		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 20, 20, 20, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.5, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 80, 0.5, 10, 10, 10, 2, velocity, false, 35);
		}
		g_iShotsFired++;
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {-5.0, -10.0, -13.0};
		
		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.3, ShellVelocity);
		ShellVelocity[2] -= 105;		
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 10);

	int shell2 = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell762_m134.mdl");

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {-5.0, -10.0, 13.0};
			
		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
	
		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.3, ShellVelocity);
		ShellVelocity[2] -= 105;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell2, TE_BOUNCE_SHELL, idx, 10);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

enum m249_e
{
	M249_IDLE1,
	M249_SHOOT1,
	M249_SHOOT2,
	M249_RELOAD,
	M249_DRAW
};

void EV_FireM249(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);	

	if (EV_IsLocal(idx))
	{		
		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(M249_SHOOT1 + gEngfuncs.pfnRandomLong(0, 1), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);

		g_iShotsFired++;
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -10.0, -13.0};

		if (WeaponData.bHaveShellVec)
		{
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
		}
		else
		{
			if (lefthand == 0)
				vecScale[2] = 0 - vecScale[2];

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
			ShellVelocity[2] -= 75;
		}

		VectorCopy(ent->attachment[1], ShellOrigin);		
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 10);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

enum m3_e
{
	M3_IDLE,
	M3_FIRE1,
	M3_FIRE2,
	M3_RELOAD,
	M3_PUMP,
	M3_START_RELOAD,
	M3_DRAW,
	M3_HOLSTER
};

void EV_FireM3(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int shell;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = 0;
	
	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;
	
		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(M3_FIRE1 + gEngfuncs.pfnRandomLong(0, 1), 2);
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.5, 20, 20, 20, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.45, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 75, 0.35, 7, 7, 7, 2, velocity, false, 35);
		}
	}
	
	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {22.0, -9.0, -11.0};
	
		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
	
		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);
	
	if (shell)
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_NULL, idx, 3);
	
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = 0.0675;
	vSpread[1] = 0.0675;
	vSpread[2] = 0;

	if (args->fparam1)
		vSpread[0] = vSpread[1] = args->fparam1 / 1000;
	
	int shots = args->fparam2 ? args->fparam2 : 9;

	EV_HLDM_FireBullets(idx, forward, right, up, shots, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_BUCKSHOT, 0, 0, 1);
}

enum m4a1_e
{
	M4A1_IDLE,
	M4A1_SHOOT1,
	M4A1_SHOOT2,
	M4A1_SHOOT3,
	M4A1_RELOAD,
	M4A1_DRAW,
	M4A1_ATTACH_SILENCER,
	M4A1_UNSIL_IDLE,
	M4A1_UNSIL_SHOOT1,
	M4A1_UNSIL_SHOOT2,
	M4A1_UNSIL_SHOOT3,
	M4A1_UNSIL_RELOAD,
	M4A1_UNSIL_DRAW,
	M4A1_DETACH_SILENCER
};

void EV_FireM4A1(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int silencer_on;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	silencer_on = args->bparam1;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (silencer_on)
		{
			if (WeaponData.bHaveAnim)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(M4A1_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
		}
		else
		{			
			if (WeaponData.bHaveAnim)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(M4A1_UNSIL_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
		}

		if (silencer_on)
			if (Hud().m_bCreateSmoke)
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.3, 16, 16, 16, 3, velocity, false, 35);
		else
			if (Hud().m_bCreateSmoke)
				EV_HLDM_CreateSmoke(ent->attachment[2], forward, 3, 0.2, 16, 16, 16, 4, velocity, false, 35);
		
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -8.0, -10.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 45;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 10);

	if (silencer_on)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, 1.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	else
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSoundSilen, 1.0, 1.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	
	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

enum mac10_e
{
	MAC10_IDLE1,
	MAC10_RELOAD,
	MAC10_DRAW,
	MAC10_SHOOT1,
	MAC10_SHOOT2,
	MAC10_SHOOT3
};

void EV_FireMac10(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(MAC10_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 2, 0.25, 8, 8, 8, 3, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {32.0, -9.0, -11.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 65;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 7);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.72, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_9MM, 0, 0, 2);
}

enum p90_e
{
	P90_IDLE1,
	P90_RELOAD,
	P90_DRAW,
	P90_SHOOT1,
	P90_SHOOT2,
	P90_SHOOT3
};

void EV_FireP90(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(P90_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.16, 7, 7, 7, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {35.0, -16.0, -22.0};

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 10);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.64, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_57MM, 0, 0, 2);
}

enum p228_e
{
	P228_IDLE,
	P228_SHOOT1,
	P228_SHOOT2,
	P228_SHOOT3,
	P228_SHOOT_EMPTY,
	P228_RELOAD,
	P228_DRAW
};

void EV_FireP228(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (empty)
		{
			if (WeaponData.iAnimEmpty)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(WeaponData.iAnimEmpty, 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(P228_SHOOT_EMPTY, 2);
		}
		else
		{			
			if (WeaponData.bHaveAnim)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(P228_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
		}

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 20, 20, 20, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {36.0, -14.0, -14.0};

		if (WeaponData.bHaveShellVec)
		{
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
		}
		else
		{
			if (lefthand == 0)
				vecScale[2] = -vecScale[2];

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
			VectorScale(ShellVelocity, 0.75, ShellVelocity);
			ShellVelocity[2] += 35;
		}
		
		VectorCopy(ent->attachment[1], ShellOrigin);
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 4);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_357SIG, 0, 0, 2);
}

enum awp_e
{
	AWP_IDLE,
	AWP_SHOOT1,
	AWP_SHOOT2,
	AWP_SHOOT3,
	AWP_RELOAD,
	AWP_DRAW
};

void EV_FireAWP(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(AWP_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 20, 20, 20, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.5, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 80, 0.5, 10, 10, 10, 2, velocity, false, 35);
		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.28, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	VectorCopy(forward, vecAiming);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_338MAG, 0, 0, 3);
}

enum scout_e
{
	SCOUT_IDLE,
	SCOUT_SHOOT,
	SCOUT_SHOOT2,
	SCOUT_RELOAD,
	SCOUT_DRAW
};

void EV_FireScout(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		
		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SCOUT_SHOOT + gEngfuncs.pfnRandomLong(0, 1), 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 35, 0.45, 20, 20, 20, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 70, 0.45, 15, 15, 15, 2, velocity, false, 35);
		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 1.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	vSpread[0] = args->fparam1 / 1000.0;
	vSpread[1] = args->fparam2 / 1000.0;
	vSpread[2] = 0;

	VectorCopy(forward, vecAiming);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_762MM, 0, 0, 3);
}

enum sg552_e
{
	SG552_IDLE1,
	SG552_RELOAD,
	SG552_DRAW,
	SG552_SHOOT1,
	SG552_SHOOT2,
	SG552_SHOOT3
};

void EV_FireSG552(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		
		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SG552_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.2, 18, 18, 18, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -8.0, -10.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.65, ShellVelocity);
		ShellVelocity[2] -= 120;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 15);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

enum tmp_e
{
	TMP_IDLE1,
	TMP_RELOAD,
	TMP_DRAW,
	TMP_SHOOT1,
	TMP_SHOOT2,
	TMP_SHOOT3
};

void EV_FireTMP(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(TMP_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.2, 10, 10, 10, 3, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {32.0, -6.0, -11.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.2, ShellVelocity);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 15);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 1.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_9MM, 0, 0, 2);
}

enum fiveseven_e
{
	FIVESEVEN_IDLE,
	FIVESEVEN_SHOOT1,
	FIVESEVEN_SHOOT2,
	FIVESEVEN_SHOOT_EMPTY,
	FIVESEVEN_RELOAD,
	FIVESEVEN_DRAW
};

void EV_FireFiveSeven(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (empty)
		{
			if (WeaponData.iAnimEmpty)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(WeaponData.iAnimEmpty, 2);
			else			
				gEngfuncs.pEventAPI->EV_WeaponAnimation(FIVESEVEN_SHOOT_EMPTY, 2);
		}
		else
		{			
			if (WeaponData.bHaveAnim)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(FIVESEVEN_SHOOT1 + gEngfuncs.pfnRandomLong(0, 1), 2);
		}

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 20, 20, 20, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = { 36.0, -14.0, -14.0 };

		if (WeaponData.bHaveShellVec)
		{
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
		}
		else
		{
			if (lefthand == 0)
				vecScale[2] = -vecScale[2];

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
			VectorScale(ShellVelocity, 0.75, ShellVelocity);
			ShellVelocity[2] -= 35.0;
		}

		VectorCopy(ent->attachment[1], ShellOrigin);		
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 4);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_57MM, 0, 0, 2);
}

enum ump45_e
{
	UMP45_IDLE1,
	UMP45_RELOAD,
	UMP45_DRAW,
	UMP45_SHOOT1,
	UMP45_SHOOT2,
	UMP45_SHOOT3
};

void EV_FireUMP45(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(UMP45_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.25, 10, 10, 10, 3, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {34.0, -10.0, -11.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 13);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.64, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_45ACP, 0, 0, 2);
}

enum xm1014_e
{
	XM1014_IDLE,
	XM1014_FIRE1,
	XM1014_FIRE2,
	XM1014_RELOAD,
	XM1014_PUMP,
	XM1014_START_RELOAD,
	XM1014_DRAW
};

void EV_FireXM1014(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shotgunshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(XM1014_FIRE1 + gEngfuncs.pfnRandomLong(0, 1), 2);
		
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 15, 15, 15, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.35, 9, 9, 9, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {22.0, -9.0, -11.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 50;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHOTSHELL, idx, 3);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = 0.0725;
	vSpread[1] = 0.0725;
	vSpread[2] = 0;

	if (args->fparam1)
		vSpread[0] = vSpread[1] = args->fparam1 / 1000;

	int shots = args->fparam2 ? args->fparam2 : 6;

	EV_HLDM_FireBullets(idx, forward, right, up, shots, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_BUCKSHOT, 0, 0, 1);
}

enum elite_e
{
	ELITE_IDLE,
	ELITE_IDLE_LEFTEMPTY,
	ELITE_SHOOTLEFT1,
	ELITE_SHOOTLEFT2,
	ELITE_SHOOTLEFT3,
	ELITE_SHOOTLEFT4,
	ELITE_SHOOTLEFT5,
	ELITE_SHOOTLEFTLAST,
	ELITE_SHOOTRIGHT1,
	ELITE_SHOOTRIGHT2,
	ELITE_SHOOTRIGHT3,
	ELITE_SHOOTRIGHT4,
	ELITE_SHOOTRIGHT5,
	ELITE_SHOOTRIGHTLAST,
	ELITE_RELOAD,
	ELITE_DRAW
};

void EV_FireEliteLeft(struct event_args_s *args)
{
	args->bparam1 = true;

	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int lefthand;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = args->bparam1;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;

	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (lefthand)
		{
			if (bullets_left <= 1)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFTLAST, 2);
			else if (time_diff >= 0.5)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT5, 2);
			else if (time_diff >= 0.4)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT4, 2);
			else if (time_diff >= 0.3)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT3, 2);
			else if (time_diff >= 0.2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT2, 2);
			else if (time_diff >= 0.0)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT1, 2);

			if (Hud().m_bCreateSmoke)
			{
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
			}
		}
		else
		{
			if (bullets_left <= 1)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHTLAST, 2);
			else if (time_diff >= 0.5)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT5, 2);
			else if (time_diff >= 0.4)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT4, 2);
			else if (time_diff >= 0.3)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT3, 2);
			else if (time_diff >= 0.2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT2, 2);
			else if (time_diff >= 0.0)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT1, 2);

			if (Hud().m_bCreateSmoke)
			{
				EV_HLDM_CreateSmoke(ent->attachment[1], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[1], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[1], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
			}
		}
	}

	if (EV_IsLocal(idx))
	{
		if (lefthand)
		{
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0, false);
			VectorCopy(ent->attachment[2], ShellOrigin);
		}
		else
		{
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);
			VectorCopy(ent->attachment[3], ShellOrigin);
		}

		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam2;
	vSpread[1] = args->fparam1 / 100.0;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, lefthand ? 0 : 1, lefthand, WeaponData.flSrcOfs);
}

void EV_FireEliteRight(struct event_args_s *args)
{
	args->bparam1 = false;

	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (bullets_left <= 1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHTLAST, 2);
		else if (time_diff >= 0.5)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT5, 2);
		else if (time_diff >= 0.4)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT4, 2);
		else if (time_diff >= 0.3)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT3, 2);
		else if (time_diff >= 0.2)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT2, 2);
		else if (time_diff >= 0.0)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT1, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);

		VectorCopy(ent->attachment[3], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam2;
	vSpread[1] = args->fparam1 / 100.0;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, 1, false, WeaponData.flSrcOfs);
}

void EV_Balrog9ExplosionCallBack ( struct tempent_s *ent, float frametime, float currenttime )
{
	ent->entity.curstate.scale += 0.007;
	if(ent->die <= cl.time + 0.3)
		ent->entity.curstate.renderamt -= 35;
}

void EV_Balrog7ExplosionCallBack ( struct tempent_s *ent, float frametime, float currenttime )
{
	ent->entity.curstate.scale += 0.0013;
	if(ent->die <= cl.time + 0.4)
		ent->entity.curstate.renderamt -= 15;

	if(ent->entity.curstate.renderamt < 0)
		ent->entity.curstate.renderamt = 0;
}

void EV_SpearCallBack ( struct tempent_s *ent, float frametime, float currenttime )
{
	/*ent->entity.origin[0] += ent->entity.angles[0] / 360 * 2;
	ent->entity.origin[1] += ent->entity.angles[1] / 360 * 2;
	ent->entity.origin[2] += ent->entity.angles[2] / 360 * 7;*/
}

void EV_Z4StunCallBack ( struct tempent_s *ent, float frametime, float currenttime )
{
	if((cl.time - ent->entity.curstate.fuser1) >1.0 / ent->entity.curstate.framerate)
	{
		if(ent->entity.curstate.frame + 1 > ent->entity.curstate.iuser1)
			ent->entity.curstate.frame = 0;
		else
			ent->entity.curstate.frame += 1;

		ent->entity.curstate.fuser1 = cl.time;
	}
}

void EV_ModelAnim ( struct tempent_s *ent, float frametime, float currenttime )
{
	if((cl.time - ent->entity.curstate.fuser1) >1.0 / ent->entity.curstate.framerate)
	{
		if(ent->entity.curstate.frame + 1 > ent->entity.curstate.iuser1)
			ent->die = cl.time + 0.01;
		else
			ent->entity.curstate.frame += 1;

		ent->entity.curstate.fuser1 = cl.time;
	}
}

void EV_CreateBloodhunterEffect();

void EV_Knife(struct event_args_s *args)
{
	int idx;
	
	int effect = args->iparam1;
	vec3_t origin;
	
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, args->iparam2);

	VectorCopy(args->origin, origin);

	if(effect & (1<<EVENT_PLAYSOUND))
	{
		if (args->fparam2)
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSoundSilen2:WeaponData.szSoundSilen, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		else
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		
		if (EV_IsLocal(idx) && args->bparam2)
			g_iShotsFired++;
	}

	if(effect & (1<<EVENT_BALROG_EXPLOSION))
	{
		int type = args->iparam2;
		
		if (!type)
		{
			struct model_s *pModel;		
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/balrogcritical.spr", 0);

			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 255;
			pEnt->entity.curstate.scale = 0.1;
			pEnt->die = cl.time +0.6f;
			pEnt->flags |= FTENT_CLIENTCUSTOM;
			pEnt->callback = EV_Balrog9ExplosionCallBack;

			int id;
			gEngfuncs.CL_LoadModel("sprites/fexplo.spr", &id);
			origin[2] -= 15.0;
			gEngfuncs.pEfxAPI->R_Explosion(origin, id, 0.45, 60, TE_EXPLFLAG_NOSOUND);
		}
		else
		{
			struct model_s *pModel;		
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/balrogcritical.spr", 0);

			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 255;
			pEnt->entity.curstate.scale = 0.5;
			pEnt->die = cl.time +0.6f;
			pEnt->flags |= FTENT_CLIENTCUSTOM;
			pEnt->callback = EV_Balrog7ExplosionCallBack;

			int id;
			gEngfuncs.CL_LoadModel("sprites/fexplo.spr", &id);
			origin[2] -= 15.0;
			gEngfuncs.pEfxAPI->R_Explosion(origin, id, 0.45, 30, TE_EXPLFLAG_NONE);
			
			gEngfuncs.pEfxAPI->R_TracerEffect(gEngfuncs.GetEntityByIndex(idx)->origin, origin);
			if (EV_IsLocal(idx))
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
	}

	if(effect & (1<<EVENT_Z4))
	{		
		struct model_s *pModel;		
		TEMPENTITY *pEnt;

		pModel = IEngineStudio.Mod_ForName("sprites/z4_stun.spr", 0);

		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
		pEnt->entity.curstate.rendermode = kRenderTransAdd;
		pEnt->entity.curstate.renderamt = 255;
		pEnt->entity.curstate.frame = 0;
		pEnt->entity.curstate.framerate = 20;
		pEnt->entity.curstate.scale = 0.4;		
		pEnt->entity.curstate.iuser1 = 4;
		pEnt->entity.curstate.fuser1 = cl.time;
		pEnt->die = cl.time +5.0f;

		pEnt->tentOffset[0] = pEnt->tentOffset[1] = 0.0;
		pEnt->tentOffset[2] = 5.0;
		pEnt->clientIndex = args->iparam2;
		pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PLYRATTACHMENT;
		pEnt->callback = EV_Z4StunCallBack;
	}

	if(effect & (1<<EVENT_Z42))
	{		
		switch (args->iparam2)
		{
		case 0:
			{
				struct model_s *pModel;
				TEMPENTITY *pEnt;

				pModel = IEngineStudio.Mod_ForName("models/ef_scorpion_hole3.mdl", false);

				origin[2] -= 32.0;
				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 1.0;
				//pEnt->entity.curstate.scale = 0.01;
				/*pEnt->entity.curstate.iuser1 = 20; // max frame bad
				pEnt->entity.curstate.fuser1 = cl.time;
				pEnt->die = cl.time + 21.0f / 30.0f;

				pEnt->flags |= FTENT_CLIENTCUSTOM;
				pEnt->callback = EV_ModelAnim;*/

				break;
			}

		case 1:
			{
				struct model_s *pModel;
				TEMPENTITY *pEnt;

				pModel = IEngineStudio.Mod_ForName("sprites/z4_skull.spr", false);

				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 0;
				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 254;
				pEnt->entity.curstate.scale = 0.2;
				pEnt->die = cl.time + args->fparam1;
				
				pEnt->tentOffset[0] = pEnt->tentOffset[1] = 0.0;
				pEnt->tentOffset[2] = 40.0;

				pEnt->clientIndex = args->entindex;
				pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PLYRATTACHMENT;
				pEnt->callback = EV_SPR;
				break;
			}

		}
		
		/*struct model_s *pModel;
		TEMPENTITY *pEnt;

		pModel = IEngineStudio.Mod_ForName("sprites/z4_stun.spr", 0);

		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
		pEnt->entity.curstate.rendermode = kRenderTransAdd;
		pEnt->entity.curstate.renderamt = 255;
		pEnt->entity.curstate.frame = 0;
		pEnt->entity.curstate.framerate = 20;
		pEnt->entity.curstate.scale = 0.4;		
		pEnt->entity.curstate.iuser1 = 5;
		pEnt->entity.curstate.fuser1 = cl.time;
		pEnt->die = cl.time +5.0f;

		pEnt->tentOffset[0] = pEnt->tentOffset[1] = 0.0;
		pEnt->tentOffset[2] = 5.0;
		pEnt->clientIndex = args->iparam2;
		pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PLYRATTACHMENT;
		pEnt->callback = EV_Z4StunCallBack;*/
	}

	if(effect & (1<<EVENT_SPEARGUN))
	{
		/*struct model_s *pModel;
		pModel = IEngineStudio.Mod_ForName("models/spear2.mdl", 0);*/

		/*for (int i=0;i<10;i++)
		{
			struct model_s *pModel;		
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("models/spear2.mdl", 0);

			pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->die = cl.time +10.0f;
			pEnt->entity.angles[0] = gEngfuncs.pfnRandomFloat(-360, 360);
			pEnt->entity.angles[1] = gEngfuncs.pfnRandomFloat(-360, 360);
			pEnt->entity.angles[2] = gEngfuncs.pfnRandomFloat(-360, 360);
			pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_GRAVITY;
			pEnt->callback = EV_SpearCallBack;
		}*/
#if 0
		int id;

		float a[3], b[3];
		a[0] = a[1] = 10;
		a[2] = 200;
		b[0] = b[1] = b[2] = 0;
		gEngfuncs.CL_LoadModel("models/spear2.mdl", &id);
		//gEngfuncs.pEfxAPI->R_BreakModel(origin, b, a, 100, 0, 10, id, 0);

		gEngfuncs.CL_LoadModel("sprites/fexplo.spr", &id);
		origin[2] += 20;
		gEngfuncs.pEfxAPI->R_Explosion(origin, id, 1.5, 30, TE_EXPLFLAG_NONE);
		//gEngfuncs.pEfxAPI->R_ParticleBurst(origin, 200, 100, 1);
		origin[2] -= 20;
		origin[0] += gEngfuncs.pfnRandomFloat(-32, 32);
		origin[1] += gEngfuncs.pfnRandomFloat(-32, 32);
		origin[2] += gEngfuncs.pfnRandomFloat(30, 35);
		gEngfuncs.CL_LoadModel("sprites/eexplo.spr", &id);
		gEngfuncs.pEfxAPI->R_Explosion(origin, id, 2, 30, TE_EXPLFLAG_NOSOUND);	

		/*struct model_s *pModel;		
		TEMPENTITY *pEnt;

		pModel = IEngineStudio.Mod_ForName("models/spear2.mdl", 0);

		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
		pEnt->die = cl.time +10.0f;
		pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_GRAVITY;*/
#else
		gEngfuncs.pEfxAPI->R_Explosion(args->origin, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/flame_puff01.spr"), (2 - int(3 * 39.37 * -0.3)) / 10.0f, 150.0f, 0);
		gEngfuncs.pEfxAPI->R_TempSphereModel(args->origin, 400.0f, 0.3f, 30, gEventAPI.EV_FindModelIndex("models/spear2.mdl"));
#endif
	}

	if (effect & (1 << EVENT_GUILLOTINE))
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, args->origin, CHAN_WEAPON, "weapons/guillotine_explode.wav", 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
		gEngfuncs.pEfxAPI->R_Explosion(args->origin, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/guillotine_lost.spr"), 1, 30, TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES);
		//gEngfuncs.pEfxAPI->R_TempSphereModel(args->origin, 400.0f, 0.3f, 30, gEventAPI.EV_FindModelIndex("models/spear2.mdl"));
		
		gEngfuncs.pEfxAPI->R_BreakModel(args->origin, Vector(64.0, 64.0, 64.0), Vector(RANDOM_FLOAT(-64.0, 64.0), RANDOM_FLOAT(-64.0, 64.0), 25.0), 10, 25, 32, gEventAPI.EV_FindModelIndex("models/gibs_guilotine.mdl"), BREAK_GLASS);
	}

	if (effect & (1 << EVENT_THANATOS5))
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, args->origin, CHAN_WEAPON, va("weapons/thanatos5_explode%d.wav", gEngfuncs.pfnRandomLong(1, 3)), 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
		gEngfuncs.pEfxAPI->R_Explosion(args->origin, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/thanatos5_explode.spr"), args->iparam2 ? 0.5 : 1, 30, TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES);
		gEngfuncs.pEfxAPI->R_Explosion(args->origin, gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/thanatos5_explode2.spr"), args->iparam2 ? 0.5:1, 30, TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NOPARTICLES);
	}

	if (effect & (1 << EVENT_CROW9))
	{
		vec3_t forward, right, up;
		
		gEngfuncs.pfnAngleVectors(args->angles, forward, right, up);
		TEMPENTITY *pEnt;
		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin + forward * 30, IEngineStudio.Mod_ForName("models/crow9_wind.mdl", false));
		pEnt->entity.curstate.frame = 0;
		pEnt->entity.curstate.framerate = 1;
		pEnt->die = cl.time + 0.5;
	}

	if(effect & (1<<EVENT_KNIFE))
	{
		auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 3);

		/*struct model_s *pModel;
		TEMPENTITY *pEnt;
		pModel = IEngineStudio.Mod_ForName("models/ef_dragonsword.mdl", 0);

		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
		pEnt->entity.curstate.rendermode = kRenderTransAdd;
		pEnt->entity.curstate.renderamt = 254;
		pEnt->entity.curstate.scale = 1.0;

		Vector vecForw;
		gEngfuncs.pfnAngleVectors(g_vecAngles,vecForw,NULL,NULL);
		pEnt->entity.origin[0] = (g_vecEye[0] + vecForw[0]*5.0f);
		pEnt->entity.origin[1] = (g_vecEye[1] + vecForw[1]*5.0f);
		pEnt->entity.origin[2] = (g_vecEye[2] + vecForw[2]*5.0f);

		Engfunc_Call_AddVisibleEntity( &(pEnt->entity));*/		
		
		/*int iHitResult;
		bool bStab = args->bparam1;		

		if(!bStab)
		{			
			g_iAnim = (g_iAnim ++) % (WeaponData.iAnimSwing[1] - WeaponData.iAnimSwing[0] + 1);
			gEngfuncs.pEventAPI->EV_WeaponAnimation(g_iAnim + WeaponData.iAnimSwing[0] - 1, 2);
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "sound/weapons/knife_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		}
		else if(!args->bparam2)
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStab[0], WeaponData.iAnimStab[1] +1), 2);
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "sound/weapons/knife_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		}
		else			
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStabHit[0], WeaponData.iAnimStabHit[1]), 2);
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "sound/weapons/knife_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		}*/

		int type = args->iparam2;
		int i = args->fparam2;
		int random;

		switch (type)
		{
		case 1:
			{
				random = WeaponData.bRandomSound ? gEngfuncs.pfnRandomLong(0, WeaponData.iSlash) : i;
				if (random > WeaponData.iSlash) random = gEngfuncs.pfnRandomLong(0, WeaponData.iSlash) - 1;
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSlash[random], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
				break;
			}
		case 2:
			{
				random = WeaponData.bRandomSound ? gEngfuncs.pfnRandomLong(0, WeaponData.iSlashHit) : i;
				if (random > WeaponData.iSlashHit) random = gEngfuncs.pfnRandomLong(0, WeaponData.iSlashHit);
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSlashHit[random], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
				break;
			}
		case 3:
			{
				random = WeaponData.bRandomSound ? gEngfuncs.pfnRandomLong(0, WeaponData.iWall) : i;
				if (random > WeaponData.iWall) random = gEngfuncs.pfnRandomLong(0, WeaponData.iWall);
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szWall[random], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
				break;
			}
		case 4:
			{
				random = WeaponData.bRandomSound ? gEngfuncs.pfnRandomLong(0, WeaponData.iStab) : i;
				if (random > WeaponData.iStab) random = gEngfuncs.pfnRandomLong(0, WeaponData.iStab);
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szStab[random], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
				break;
			}
		case 5:
			{
				random = WeaponData.bRandomSound ? gEngfuncs.pfnRandomLong(0, WeaponData.iStabHit) : i;
				if (random > WeaponData.iStabHit) random = gEngfuncs.pfnRandomLong(0, WeaponData.iStabHit);
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szStabHit[random], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
				break;
			}
		case 6:
			{
				if(WeaponData.szDraw[0])
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szDraw, 1.0, ATTN_NORM, 0, 100);
				break;
			}
		}
		
		if ((type == 2 || type == 5) && EV_IsLocal(idx) && cl_crosshair_hit->value)
		{
			vec3_t angles, forward, right, up;
			vec3_t vecSrc, vecEnd;
			
			EV_GetGunPosition(args, vecSrc, origin);

			VectorCopy(args->angles, angles);
			gEngfuncs.pfnAngleVectors(angles, forward, right, up);

			for (int i = 0; i < 3; i++)
				vecEnd[i] = vecSrc[i] + 8196 * forward[i];
					
			pmtrace_t tr;
			
			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, false);
			gEngfuncs.pEventAPI->EV_PushPMStates();
			gEngfuncs.pEventAPI->EV_SetSolidPlayers (idx - 1);
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_IGNORE, -1, &tr);
			
			if (tr.ent > 0)
			{
				physent_t *ent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);
				
				if (ent->info >= 1 && ent->info <= 32)
				{
					if (vPlayer[ent->info].team != g_iTeam || g_iMod == MOD_DM)
						HudAmmo().Hit(cl.time);
				}
			}

			gEngfuncs.pEventAPI->EV_PopPMStates();
		}		
		
	}
	
	if (effect & (1 << EVENT_BUFFAKM4HIT))
	{
		if (g_iBTEWeapon == WPN_BLOODHUNTER)
		{
			if (g_iWeaponStat - 50 < 3)
			{
				EV_CreateBloodhunterEffect();
			}
		}
		HudAmmo().HitForBuff(cl.time);
	}


/*	if (EV_IsLocal(idx) && !effect)
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam1, 2);*/
}

enum galil_e
{
	GALIL_IDLE1,
	GALIL_RELOAD,
	GALIL_DRAW,
	GALIL_SHOOT1,
	GALIL_SHOOT2,
	GALIL_SHOOT3
};

void EV_FireGalil(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 10000000.0;
	angles[1] += args->iparam2 / 10000000.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(GALIL_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {20.0, -8.0, -10.0};

		if (WeaponData.bHaveShellVec)
		{
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
		}
		else
		{
			if (lefthand == 0)
				vecScale[2] = -vecScale[2];

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
			VectorScale(ShellVelocity, 1.25, ShellVelocity);
			ShellVelocity[2] -= 122;
		}

		VectorCopy(ent->attachment[1], ShellOrigin);		
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_762MM, 0, 0, 2);
}

enum famas_e
{
	FAMAS_IDLE1,
	FAMAS_RELOAD,
	FAMAS_DRAW,
	FAMAS_SHOOT1,
	FAMAS_SHOOT2,
	FAMAS_SHOOT3
};

void EV_FireFamas(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 10000000.0;
	angles[1] += args->iparam2 / 10000000.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(FAMAS_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {17.0, -8.0, -14.0};

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 122;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 8);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
}

void EV_FireSprifle(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);

		VectorScale(up, 0.4, up);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[1], up, 80, 0.3, 120, 120, 120, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], up, 40, 0.2, 90, 90, 90, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], up, 30, 0.2, 60, 60, 60, 3, velocity, false, 35);
		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.28, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	VectorCopy(forward, vecAiming);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_338MAG, 0, 0, 3);
}

void EV_FireM1887XMAS(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);
	
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);
	
	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;
	
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();

		if(g_iWeaponData[g_iWeapon].iClip > 1)		
			g_iAnim = gEngfuncs.pfnRandomLong(WeaponData.iAnimStart + 8, WeaponData.iAnimEnd);
		else
			g_iAnim = gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimStart + 7);

		gEngfuncs.pEventAPI->EV_WeaponAnimation(g_iAnim, 2);
	
		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.5, 20, 20, 20, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.45, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 75, 0.35, 7, 7, 7, 2, velocity, false, 35);
		}
	}
	


	char sound[64];
	int iAnim = g_iAnim - WeaponData.iAnimStart;
	int iSound;
	if (iAnim >= 0)
		iSound = 1;
	if (iAnim >= 7)
		iSound = 2;
	if (iAnim >= 15)
		iSound = 3;

	if (EV_IsLocal(idx))
		sprintf(sound, "sound/weapons/%s-%d.wav", WeaponData.szName, iSound);
	else
		sprintf(sound, "%s",WeaponData.szSound);		
	
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, sound, 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	
	EV_GetGunPosition(args, vecSrc, origin);
	
	VectorCopy(forward, vecAiming);
	
	vSpread[0] = 0.0675;
	vSpread[1] = 0.0675;
	vSpread[2] = 0;
	
	if (WeaponData.bHaveShotGunSpread)
	{
		vSpread[0] = WeaponData.fShotGunSpread[0];
		vSpread[1] = WeaponData.fShotGunSpread[1];
		vSpread[2] = WeaponData.fShotGunSpread[2];
	}
	
	int shots = 9;
	if (WeaponData.iShotGunShots)
		shots = WeaponData.iShotGunShots;
	
	EV_HLDM_FireBullets(idx, forward, right, up, shots, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_BUCKSHOT, 0, 0, 1);
}

enum skull4_e
{
	SKULL4_IDLE,
	SKULL4_IDLE_LEFT,
	SKULL4_LEFT1,
	SKULL4_LEFT2,
	SKULL4_LEFT_LAST,
	SKULL4_RIGHT1,
	SKULL4_RIGHT2,
	SKULL4_RIGHT_LAST,
	SKULL4_RELOAD,
	SKULL4_DRAW
};

void EV_FireSkull4Left(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int lefthand;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = args->bparam1;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;

	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();
			
		if (bullets_left <= 1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL4_LEFT_LAST, 2);
		else if (gEngfuncs.pfnRandomLong(0,1))
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL4_LEFT1, 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL4_LEFT2, 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0, false);
		VectorCopy(ent->attachment[2], ShellOrigin);

		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2, 0, true, WeaponData.flSrcOfs);
}

void EV_FireSkull4Right(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int lefthand;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = args->bparam1;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;

	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();
			
		if (bullets_left <= 1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL4_RIGHT_LAST, 2);
		else if (gEngfuncs.pfnRandomLong(0,1))
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL4_RIGHT1, 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL4_RIGHT2, 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);
		VectorCopy(ent->attachment[3], ShellOrigin);

		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2, 1, false, WeaponData.flSrcOfs);
}

void EV_FireDMP7A1Left(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int lefthand;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = args->bparam1;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;

	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (bullets_left <= 1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFTLAST, 2);
		else if (time_diff >= 0.5)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT5, 2);
		else if (time_diff >= 0.4)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT4, 2);
		else if (time_diff >= 0.3)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT3, 2);
		else if (time_diff >= 0.2)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT2, 2);
		else if (time_diff >= 0.0)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTLEFT1, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0, false);
		VectorCopy(ent->attachment[2], ShellOrigin);

		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	if (EV_IsLocal(idx))
		vecSrc = ent->attachment[0];

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	vecSrc[0] -= right[0] * 2;
	vecSrc[1] -= right[1] * 2;
	vecSrc[2] -= right[2] * 2;
	
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, 0, true, WeaponData.flSrcOfs);
}

void EV_FireDMP7A1Right(struct event_args_s *args)
{
	args->bparam1 = false;

	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if(WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (bullets_left <= 1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHTLAST, 2);
		else if (time_diff >= 0.5)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT5, 2);
		else if (time_diff >= 0.4)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT4, 2);
		else if (time_diff >= 0.3)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT3, 2);
		else if (time_diff >= 0.2)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT2, 2);
		else if (time_diff >= 0.0)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(ELITE_SHOOTRIGHT1, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);

		VectorCopy(ent->attachment[3], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	vecSrc[0] += right[0] * 2;
	vecSrc[1] += right[1] * 2;
	vecSrc[2] += right[2] * 2;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, 1, false, WeaponData.flSrcOfs);
}

enum skull3_e
{
	SKULL3_IDLE,
	SKULL3_IDLE_LEFT,
	SKULL3_LEFT1,
	SKULL3_LEFT2,
	SKULL3_LEFT_LAST,
	SKULL3_RIGHT1,
	SKULL3_RIGHT2,
	SKULL3_RIGHT_LAST,
	SKULL3_RELOAD,
	SKULL3_DRAW,
	SKULL3_CHANGE_TO_B
};

void EV_FireSkull3Left(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;

	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (!args->bparam1)
		{
			if (bullets_left <= 1)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL3_LEFT_LAST, 2);
			else if (gEngfuncs.pfnRandomLong(0, 1))
				gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL3_LEFT1, 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL3_LEFT2, 2);
		}
		else
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(3, 5), 2);
		}

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[1], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);
		VectorCopy(ent->attachment[0], ShellOrigin);

		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, 1, true, WeaponData.flSrcOfs);
}

void EV_FireSkull3Right(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (bullets_left <= 1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL3_RIGHT_LAST, 2);
		else if (gEngfuncs.pfnRandomLong(0, 1))
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL3_RIGHT1, 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(SKULL3_RIGHT2, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[2], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[2], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[2], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);

		VectorCopy(ent->attachment[3], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, 3, false, WeaponData.flSrcOfs);
}

#if 0
void VectorMA (const float *veca, float scale, const float *vecb, float *vecc)
{
	vecc[0] = veca[0] + scale*vecb[0];
	vecc[1] = veca[1] + scale*vecb[1];
	vecc[2] = veca[2] + scale*vecb[2];
}
#else
extern void VectorMA(const Vector& start, float scale, const Vector& direction, Vector& dest);
#endif

void VectorScale (float *veca, float scale)
{
	veca[0] *= scale;
	veca[1] *= scale;
	veca[2] *= scale;
}

void VectorAngles( const float *forward, float *angles )
{
	float	tmp, yaw, pitch;
	
	if (forward[1] == 0 && forward[0] == 0)
	{
		yaw = 0;
		if (forward[2] > 0)
			pitch = 90;
		else
			pitch = 270;
	}
	else
	{
		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
		if (yaw < 0)
			yaw += 360;

		tmp = sqrt (forward[0]*forward[0] + forward[1]*forward[1]);
		pitch = (atan2(forward[2], tmp) * 180 / M_PI);
		if (pitch < 0)
			pitch += 360;
	}
	
	angles[0] = pitch;
	angles[1] = yaw;
	angles[2] = 0;
}

void EV_FlameCallback ( struct tempent_s *ent, float frametime, float currenttime )
{
	ent->entity.angles.x = gEngfuncs.pfnRandomFloat(-512.0, 511.0);
	ent->entity.angles.y = gEngfuncs.pfnRandomFloat(-256.0, 255.0);
	ent->entity.angles.z = gEngfuncs.pfnRandomFloat(-256.0, 255.0);

	ent->entity.curstate.scale += ent->entity.curstate.fuser2;

	if (ent->entity.curstate.scale > ent->entity.curstate.vuser3[0])
		ent->entity.curstate.scale = ent->entity.curstate.vuser3[0];

	if (ent->entity.curstate.frame > ent->frameMax / 3)
		ent->entity.curstate.fuser2 = ent->entity.curstate.vuser4[0];

//	gEngfuncs.Con_Printf("%f\n", ent->entity.curstate.scale);

	vec3_t origin;
	vec3_t forward;
	VectorCopy(ent->entity.curstate.vuser1, origin);
	VectorCopy(ent->entity.curstate.velocity, forward);

	if (VectorLength(forward))
	{
		VectorScale(forward, (currenttime - ent->entity.curstate.fuser3) / (ent->frameMax / ent->entity.curstate.framerate) * ent->entity.curstate.fuser4);
		VectorAdd(origin, forward, ent->entity.origin);
	}
}

void EV_FlameHitCallback ( struct tempent_s *ent, struct pmtrace_s *ptr )
{
	physent_t *pe;
	pe = gEngfuncs.pEventAPI->EV_GetPhysent(ptr->ent);
	
	if (!pe)
		return;

	if (!EV_IsLocal(pe->info))
	{	
		ent->entity.curstate.fuser2 = ent->entity.curstate.vuser4[1];
		ent->entity.curstate.framerate = ent->entity.curstate.vuser4[2];

		VectorClear(ent->entity.curstate.velocity);
	}
}

void EV_CreateFlameThrowerFlame(vec3_t vecSrc, vec3_t vecDir, vec3_t base_velocity, int idx)
{
	model_t *model;
	TEMPENTITY *pEntity;

	model = IEngineStudio.Mod_ForName("sprites/flame_puff01.spr", FALSE);

	pEntity = CL_DefaultSprite(vecSrc, model, 10);

	if (pEntity)
	{
		pEntity->flags |= FTENT_COLLIDEALL | FTENT_CLIENTCUSTOM;
		pEntity->entity.angles.x = RANDOM_FLOAT(-512, 511);
		pEntity->entity.angles.y = RANDOM_FLOAT(-256, 255);
		pEntity->entity.angles.z = RANDOM_FLOAT(-256, 255);
		pEntity->flags |= FTENT_ROTATE;
		pEntity->entity.curstate.scale = 0.15;
		pEntity->entity.baseline.fuser1 = 2.5;
		pEntity->callback = DefaultScaleThink;
		pEntity->hitcallback = DefaultFramerateTouch;
		pEntity->entity.curstate.rendermode = kRenderTransAdd;
		pEntity->entity.curstate.renderamt = 150;
		pEntity->entity.curstate.renderfx = kRenderFxFadeSlow;
		pEntity->entity.origin = vecSrc;
		pEntity->entity.baseline.origin = vecDir * (400 + RANDOM_LONG(0, 5));
		if (base_velocity.Length())
		{
			pEntity->entity.baseline.origin.x += 0.9 * base_velocity.x;
			pEntity->entity.baseline.origin.y += 0.9 * base_velocity.y;
			pEntity->entity.baseline.origin.z += 0.5 * base_velocity.z;
		}
		pEntity->clientIndex = idx;
	}
}

void EV_FireFlameThrower(struct event_args_s *args)
{
	int idx;
	vec3_t forward, right, up;
	vec3_t vecSrc;
	cl_entity_t *ent;

	ent = GetViewEntity();
	idx = args->entindex;

	if (EV_IsLocal(idx))
		AngleVectors(v_angles, forward, right, up);
	else
		AngleVectors(args->angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, args->origin);

	if (EV_IsLocal(idx))
		EV_CreateFlameThrowerFlame(ent->attachment[0] + forward * 7, forward, args->velocity, idx);
	else
		EV_CreateFlameThrowerFlame(vecSrc, forward, args->velocity, idx);

	if (args->bparam1)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, args->origin, CHAN_WEAPON, "weapons/flamethrower-1.wav", 1.0, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 15));
}

void EV_FireSfSniper(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t vecSrc, vecEnd, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;
		
		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	VectorCopy(forward, vecAiming);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_SFSNIPER, 0, 0, 3);

	EV_GetGunPosition(args, vecSrc, origin);
	//VectorMA(vecSrc, 8196, forward, vecEnd);

	vec3_t vecDir;

	for (int i = 0; i < 3; i++)
	{
		vecDir[i] = vecAiming[i] + vSpread[0] * right[i] + vSpread[1] * up[i];
		vecEnd[i] = vecSrc[i] + 8196.0 * vecDir[i];
	}

	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/laserbeam.spr");

	BEAM *pBeam;

	if (EV_IsLocal(idx))
	{
		if (Hud().m_FOV == 90.0)
		{
			pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(ent->attachment[0], vecEnd, iBeamModelIndex, 1.0f, 3.0f, 0.0f, 128.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		}
		else
		{
			pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(origin, vecEnd, iBeamModelIndex, 1.0f, 3.0f, 0.0f, 128.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
		}
	}
	else
	{
		pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(origin, vecEnd, iBeamModelIndex, 1.0f, 3.0f, 0.0f, 128.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	if (pBeam) pBeam->flags |= FBEAM_FADEIN | FBEAM_FADEOUT | FBEAM_SHADEIN;

	if (pBeam)
		pBeam->flags |= FBEAM_FADEIN | FBEAM_FADEOUT | FBEAM_SHADEIN;
}
void EV_FirePetrolBoomer(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;
				
		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.5, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.5, 20, 20, 20, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.45, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 75, 0.35, 7, 7, 7, 2, velocity, false, 35);
		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
}

void EV_FireBalrog11(struct event_args_s *args)
{
	if (!args->bparam2)
	{
		EV_FireXM1014(args);
		return;
	}

	int idx;
	vec3_t origin;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	

	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	int lefthand;

	lefthand = cl_righthand->value;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);
	
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell_bcs.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.35, 20, 20, 20, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 80, 0.5, 15, 15, 15, 3, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = {22.0, -9.0, -11.0};
		
		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];
		
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[2], ShellOrigin);
		//VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 50;

		gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHOTSHELL, idx, 3);
	
	vec3_t sub;
	float distance = 0;

	if (EV_IsLocal(idx))
	{
		VectorSubtract(ent->attachment[0], origin, sub);
		distance = VectorLength(sub);
	}
	args->fparam2 = 25;

	float v_angle1= v_angles[1];

	for (int j = 2; j<=8; j++)
	{

		//v_angles[1] = v_angle1;
		//v_angles[1] -= args->fparam2 / 2.0f;

		for (int i=0; i<=4; i++)
		{
			//v_angles[1] += args->fparam2 / 5.0f;

			v_angles[1] = v_angle1 + 10 * (i - 2);

			gEngfuncs.pfnAngleVectors(v_angles, forward, right, up);
			
			struct model_s *pModel;
			TEMPENTITY *pEnt;

			pModel = IEngineStudio.Mod_ForName("sprites/flame_puff01.spr", 0);

			if (EV_IsLocal(idx))
				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(ent->attachment[0], pModel);
			else
				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);

			pEnt->entity.curstate.rendermode = kRenderTransAdd;
			pEnt->entity.curstate.renderamt = 10 + j*12;
			pEnt->entity.curstate.rendercolor.b = 200;
			pEnt->entity.curstate.rendercolor.r = pEnt->entity.curstate.rendercolor.g = 255;
			pEnt->entity.curstate.scale = 0.4 - j*0.48;
			pEnt->entity.curstate.fuser2 = 0.1 - j*0.012;
			pEnt->entity.curstate.fuser4 = (/*args->fparam1*/600 - distance) * (j+1) / 8;
			pEnt->entity.curstate.vuser3[0] = 0.2 - j * 0.24;
			pEnt->entity.curstate.vuser4[0] = 0.005;
			pEnt->entity.curstate.vuser4[1] = 0.05;
			pEnt->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 1) ? 359.9 - gEngfuncs.pfnRandomLong(0, 15) : gEngfuncs.pfnRandomLong(0, 15);
			pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEALL | FTENT_SPRANIMATE;
			pEnt->frameMax = ModelFrameCount(pModel) - 1;
			pEnt->entity.curstate.vuser4[2] = pEnt->frameMax / 0.55 * 3.0;
			pEnt->entity.curstate.framerate = pEnt->frameMax / 0.55;
			pEnt->entity.curstate.fuser3 = cl.time;
			
			pEnt->die = cl.time + 60.0f;

			VectorCopy(pEnt->entity.origin, pEnt->entity.curstate.vuser1);
			
			
			VectorCopy(forward, pEnt->entity.curstate.velocity);

			pEnt->callback = EV_FlameCallback;
			pEnt->hitcallback = EV_FlameHitCallback;
		}
	}
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSoundSilen, 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
}

void EV_FireLightZg(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 10000000.0;
	angles[1] += args->iparam2 / 10000000.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(GALIL_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = { 20.0, -8.0, -10.0 };

		if (WeaponData.bHaveShellVec)
		{
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
		}
		else
		{
			if (lefthand == 0)
				vecScale[2] = -vecScale[2];

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
			VectorScale(ShellVelocity, 1.25, ShellVelocity);
			ShellVelocity[2] -= 122;
		}

		VectorCopy(ent->attachment[1], ShellOrigin);
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_ZOMBIEGUN, 0, 0, 2);
}

void EV_FireHeavyZg(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (WeaponData.bHaveAnim)
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(AK47_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = { 20.0, -8.0, -13.0 };

		if (lefthand == 0)
			vecScale[2] = -vecScale[2];

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		ShellVelocity[2] -= 75;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);

	//gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, WeaponData.fSoundVolume, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8196, BULLET_PLAYER_ZOMBIEGUN, 0, 0, 2);
}

BEAM *pBeam;
BEAM *pBeam2;
TEMPENTITY *pEntSpPistol;

void KillBeam(void);

void EV_FireSfPistolStop(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	
	
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);
	VectorCopy ( args->origin, origin );

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sfpistol_shoot1.wav", 1.0, ATTN_NORM, (1<<5), 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	if (args->bparam2)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sfpistol_shoot_end.wav", 1.0, 1.0, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));		
	
	if (EV_IsLocal(idx)) 
	{
		KillBeam();

		if (pEntSpPistol)
		{
			pEntSpPistol->die = 0.0;
			pEntSpPistol = NULL;
		}
	}
}

void KillBeam()
{
	if (pBeam)
	{
		pBeam->die = 0.0;
		pBeam = NULL;
	}
	
	if (pBeam2)
	{
		pBeam2->die = 0.0;
		pBeam2 = NULL;
	}
}

void EV_FireSfPistol(struct event_args_s *args)
{
	if (args->bparam1)
	{
		EV_FireSfPistolStop(args);

		return;
	}

	int idx;	
		
	cl_entity_s *ent;
	vec3_t vecSrc, vecEnd, origin, angles, forward, right, up;

	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);

	if (args->iparam1 == 1)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sfpistol_shoot1.wav", 1.0, 1.0, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	else if (args->iparam1 == 2)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sfpistol_shoot_start.wav", 1.0, 1.0, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));		

	if (EV_IsLocal(idx))
	{
		if (!pEntSpPistol)
		{
			struct model_s *pModel;
			pModel = IEngineStudio.Mod_ForName("sprites/muzzleflash27.spr", 0);

			pEntSpPistol = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(ent->attachment[0], pModel);
			pEntSpPistol->entity.curstate.rendermode = kRenderTransAdd;
			pEntSpPistol->entity.curstate.renderamt = 200;
			pEntSpPistol->entity.curstate.scale = 0.06;
			pEntSpPistol->die = cl.time + 9999.0f;
			pEntSpPistol->flags |= FTENT_PERSIST | FTENT_CLIENTCUSTOM;
			pEntSpPistol->callback = EV_SPR_FOLLOW_ATTACHMENT;
			pEntSpPistol->entity.curstate.iuser1 = 0;
		}
		/*int sModelMuzzleflash;
		gEngfuncs.CL_LoadModel("sprites/muzzleflash27.spr", &sModelMuzzleflash);
		gEngfuncs.pEfxAPI->R_Explosion(ent->attachment[0], sModelMuzzleflash, 25, 0, TE_EXPLFLAG_NOSOUND);*/
	}
		
	
	pmtrace_t tr;

	if (EV_IsLocal(idx) && g_bAlive)
		VectorCopy( v_angles, angles );
	
	AngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		VectorCopy(ent->attachment[0], vecSrc);
	}
	else
	{
		EV_GetGunPosition(args, vecSrc, origin);
	}


	VectorMA(vecSrc, 8196, forward, vecEnd);

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );	
		
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
	gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr);
	
	if (tr.ent > 0 && EV_IsLocal(idx))
	{
		physent_t *ent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

		if (tr.ent)
		{
			physent_t *ent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);
		
			if (ent->info >= 1 && ent->info <= 32)
			{
				if ((vPlayer[ent->info].team != g_iTeam || g_iMod == MOD_DM))
					HudStatistics().m_iHit++;

				if (cl_crosshair_hit->value)
					if ((vPlayer[ent->info].team != g_iTeam || g_iMod == MOD_DM) && (g_iMod != MOD_GHOST || vPlayer[ent->info].team != 2))
						HudAmmo().Hit(cl.time);	
			}
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();

	int sModelMuzzleflash;
	gEngfuncs.CL_LoadModel("sprites/plasmabomb.spr", &sModelMuzzleflash);
	gEngfuncs.pEfxAPI->R_Explosion(tr.endpos, sModelMuzzleflash, 0.10, 30, TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES);

	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/smoke.spr" );
	if (EV_IsLocal(idx))
	{
		if (!pBeam)
			pBeam = gEngfuncs.pEfxAPI->R_BeamEntPoint(idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 1.0, 0.0, 1.7, 55, 0, 0, 50, 228, 0);

		if (!pBeam2)
			pBeam2 = gEngfuncs.pEfxAPI->R_BeamEntPoint(idx | 0x1000, tr.endpos, iBeamModelIndex, 99999, 1.5, 0.0, 0.7, 25, 0, 0, 0, 178, 0);
	}
	else
	{
		BEAM *pBeam3 = gEngfuncs.pEfxAPI->R_BeamEntPoint(idx | 0x1000, tr.endpos, iBeamModelIndex, 0.08, 1.0, 0.0, 1.7, 55, 0, 0, 50, 228, 0);
		BEAM *pBeam4 = gEngfuncs.pEfxAPI->R_BeamEntPoint(idx | 0x1000, tr.endpos, iBeamModelIndex, 0.08, 1.5, 0.0, 0.7, 25, 0, 0, 0, 178, 0);
	}	
}

void UpdateBeams()
{
	if (pBeam || pBeam2)
	{	
		vec3_t forward, vecSrc, vecEnd, origin, angles, right, up;
		vec3_t view_ofs;
		pmtrace_t tr;
		cl_entity_t *pthisplayer = gEngfuncs.GetLocalPlayer();
		int idx = pthisplayer->index;
		cl_entity_s *ent;
		ent = GetViewEntity();

		// Get our exact viewangles from engine
		gEngfuncs.GetViewAngles( (float *)angles );

		// Determine our last predicted origin

		if (EV_IsLocal(idx))
		{
			VectorCopy(ent->attachment[0], origin);
		}
		else
		{
			VectorCopy(g_vecOrigin, origin);
		}

		
		/*HUD_GetLastOrg( (float *)&origin );*/

		AngleVectors( angles, forward, right, up );

		VectorCopy( origin, vecSrc );
		
		VectorMA( vecSrc, 8196, forward, vecEnd );

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );	
							
		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();
						
		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers ( idx - 1 );	

		gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );
		gEngfuncs.pEventAPI->EV_PlayerTrace( vecSrc, vecEnd, PM_STUDIO_BOX, -1, &tr );

		gEngfuncs.pEventAPI->EV_PopPMStates();

		if ( pBeam )
		{
			VectorCopy( tr.endpos, pBeam->target );		
			pBeam->die	  = cl.time + 0.1; // We keep it alive just a little bit forward in the future, just in case.
		}
			
		if ( pBeam2 )
		{
			VectorCopy( tr.endpos, pBeam2->target );
			pBeam2->die	   = cl.time + 0.1; // We keep it alive just a little bit forward in the future, just in case.
		}
	}
}

void Event_VidInit()
{
	KillBeam();

	if ( pEntSpPistol )
	{
		pEntSpPistol->die = 0.0;
		pEntSpPistol = NULL;
	}

}

void Event_KilledReset()
{
	for (int id = 1; id <= gEngfuncs.GetMaxClients(); id++)
	{
		if (gEngfuncs.pEventAPI->EV_IsLocal(id))
			gEngfuncs.pEventAPI->EV_StopSound(id, CHAN_WEAPON, "weapons/sfpistol_shoot1.wav");
	}
}

void EV_FireRailCannon(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	int charge = 0;
	float flDistance = 4096.0;
	if (args->bparam1)
		charge = args->fparam1;

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();
		if (!args->bparam1)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(4, 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(5, 2);
	}

	if (!args->bparam1)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/railcanon-1.wav", 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	else
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/railcanon-2.wav", 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	//gEngfuncs.pfnClientCmd("stopsound");

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = vSpread[1] = vSpread[2] = 0;

	int shots;

	if (!args->bparam1)
	{
		vSpread[0] = vSpread[1] = 0.0715;
		shots = 7;
		flDistance = 3048.0;
		EV_HLDM_FireBullets(idx, forward, right, up, shots, vecSrc, forward, vSpread, flDistance, BULLET_PLAYER_BUCKSHOT, 0, 0, args->bparam1 ? 2 : 1, 0, charge);
	}
	else
	{
		if (charge == 1)
		{
			vSpread[0] = vSpread[1] = 0.0625;
			shots = 7;
			flDistance = 4128.0;
		}
		else if (charge == 2)
		{
			vSpread[0] = vSpread[1] = 0.045;
			shots = 8;
			flDistance = 6096.0;
		}
		else if (charge == 3)
		{
			vSpread[0] = vSpread[1] = 0.01;
			shots = 8;
			flDistance = 8048.0;
		}
		EV_HLDM_FireBullets(idx, forward, right, up, shots, vecSrc, forward, vSpread, flDistance, BULLET_PLAYER_RAILCANNON, 0, 0, args->bparam1 ? 2 : 1, 0, charge);
	}
}

void EV_FireChainsaw(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;

	idx = args->entindex;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	// Of all conditions, fparam1 means the distance to trace to, iparam2 means if the attack is mode 2 or 1
	// When attack with mode 1
	// bparam2 means if the player clicked left button just now
	// fparam2 means the shake power from server
	// iparam1 means a condition to choose which sound should we play
	// When attack with mode 2
	// fparam2 means if we should play the hit sound or just a slash sound
	// iparam1 means which condition of the animation we need to play
	// bparam1 means if the player hitted a player or an entity that we can hurt it
	// bparam2 means if the weapon has clip (m_iClip > 0)

	if (args->iparam2 == 2)
	{
		if (!EV_IsLocal(idx))
		{
			gEngfuncs.pfnPlaySoundByNameAtLocation("weapons/chainsaw_hit2.wav", gEngfuncs.pfnRandomFloat(0.8f, 0.9f), origin);
		}
		else
		{
			if (args->bparam2)
			{
				gEventAPI.EV_WeaponAnimation(args->iparam1 ? 7 : 8, 2);
				gEventAPI.EV_PlaySound(idx, origin, CHAN_ITEM, args->bparam1 ? (args->fparam2 ? "weapons/chainsaw_hit2.wav" : (!args->iparam1 ? (args->bparam2 ? "weapons/chainsaw_slash1.wav" : "weapons/chainsaw_slash3.wav") : (args->bparam2 ? "weapons/chainsaw_slash2.wav" : "weapons/chainsaw_slash4.wav"))) : (args->iparam1 ? (args->bparam2 ? "weapons/chainsaw_slash1.wav" : "weapons/chainsaw_slash3.wav") : (args->bparam2 ? "weapons/chainsaw_slash2.wav" : "weapons/chainsaw_slash4.wav")), 1.0f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else
			{
				gEventAPI.EV_WeaponAnimation(args->iparam1 ? 9 : 10, 2);
				gEventAPI.EV_PlaySound(idx, origin, CHAN_ITEM, args->iparam1 ? (args->bparam2 ? "weapons/chainsaw_slash1.wav" : "weapons/chainsaw_slash3.wav") : (!args->bparam2 ? "weapons/chainsaw_slash2.wav" : "weapons/chainsaw_slash4.wav"), 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
		}
	}
	else if (args->iparam2 == 1)
	{
		int iPlayerBits = args->iparam1;
		if (iPlayerBits)
		{
			for (int iPlayer = 1; iPlayer < 33; iPlayer++)
			{
				if (iPlayerBits & (1 << iPlayer))
				{
					cl_entity_t *gPlayer = gEngfuncs.GetEntityByIndex(iPlayer);
					if (!gPlayer)
						continue;
					gEventAPI.EV_PlaySound(iPlayer, gPlayer->origin, CHAN_AUTO, args->bparam2 ? "weapons/chainsaw_hit2.wav" : (args->bparam1 ? "weapons/chainsaw_hit4.wav" : "weapons/chainsaw_hit3.wav"), 1.0, ATTN_NORM, 0, gEngfuncs.pfnRandomLong(0, 15) + 94);
				}
			}
		}
	}
	else
	{
		if (!args->bparam2)
		{
			if (EV_IsLocal(idx))
				gEventAPI.EV_WeaponAnimation(3, 2);
			gEventAPI.EV_PlaySound(idx, origin, CHAN_ITEM, "weapons/chainsaw_attack1_start.wav", 1.0f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
		}
		else
		{
			if (EV_IsLocal(idx))
			{
				gEventAPI.EV_WeaponAnimation(4, 2);

				MESSAGE_BEGIN("ScreenShake");
				WRITE_SHORT((1 << 12) * 5);
				WRITE_SHORT(1);
				WRITE_SHORT((1 << 12) * 5);
				MESSAGE_END();


			}
			if (args->bparam1)
			{
				//gEventAPI.EV_PlaySound(idx, origin, CHAN_ITEM, "weapons/chainsaw_attack1_loop.wav", 1.0f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				//if (EV_IsLocal(idx))
				//	gEngfuncs.pfnPlaySoundByNameAtLocation(args->iparam1 ? "weapons/chainsaw_hit1.wav" : "weapons/chainsaw_attack1_loop.wav", 1.0f, origin);
				gEventAPI.EV_PlaySound(idx, origin, CHAN_ITEM, "weapons/chainsaw_hit1.wav", 1.0f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else
			{
				gEventAPI.EV_PlaySound(idx, origin, CHAN_ITEM, "weapons/chainsaw_attack1_loop.wav", 1.0f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
		}
	}

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, Vector(0, 0, 0), args->fparam1, BULLET_PLAYER_OIL, 0, 0, 8);

}

void EV_CannonFireEffect(vec3_t vecSrc, vec3_t vecForward, vec3_t vecVelocity, int idx, int iModel)
{
	if (!iModel)
		iModel = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/flame_puff01.spr");
	TEMPENTITY *ent;
	int i;
	int ran;
	float c, s;
	for (i = 0; i<12; i++)
	{
		ent = gEngfuncs.pEfxAPI->R_DefaultSprite(vecSrc, iModel, 20);
		if (!ent)
			break;
		ent->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEALL;
		ent->entity.angles.x = gEngfuncs.pfnRandomFloat(-512.0, 511.0);
		ent->entity.angles.y = gEngfuncs.pfnRandomFloat(-256.0, 255.0);
		ent->entity.angles.z = gEngfuncs.pfnRandomFloat(-256.0, 255.0);
		ent->flags |= FTENT_ROTATE;
		ent->entity.curstate.scale = gEngfuncs.pfnRandomFloat(0.3, 0.8);
		ent->entity.baseline.fuser1 = gEngfuncs.pfnRandomFloat(1.5, 2.0);	// CSO: baseline.fuser2
		ent->callback = DefaultScaleThink;
		ent->hitcallback = DefaultFramerateTouch;
		ent->entity.curstate.rendermode = kRenderTransAdd;
		ent->entity.curstate.renderamt = 150;
		ent->entity.curstate.renderfx = kRenderFxFadeSlow;
		ran = 10 * gEngfuncs.pfnRandomLong(0, 4) - 2;
		c = cos(ran * M_PI / 180);
		s = sin(ran * M_PI / 180);
		ent->entity.origin = vecSrc;
		ent->entity.baseline.origin.x = (c * vecForward.x - s * vecForward.y) * gEngfuncs.pfnRandomLong(130, 400);
		ent->entity.baseline.origin.y = (s * vecForward.x + c * vecForward.y) * gEngfuncs.pfnRandomLong(130, 400);
		ent->entity.baseline.origin.z = vecForward.z * gEngfuncs.pfnRandomLong(30, 200);

		if (vecVelocity)
		{
			ent->entity.baseline.origin.x += vecVelocity.x * 0.9;
			ent->entity.baseline.origin.y += vecVelocity.y * 0.9;
			ent->entity.baseline.origin.z += vecVelocity.z * 0.3;
		}

		ent->clientIndex = idx;
	}
}

void EV_FireCannon(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	

	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int lefthand;

	lefthand = cl_righthand->value;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles.y -= 18;
	
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/cannon-1.wav", 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	if (EV_IsLocal(idx))
	{
		//g_iShotsFired++;

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.35, 20, 20, 20, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 80, 0.5, 15, 15, 15, 3, velocity, false, 35);
		}
		gEngfuncs.pEventAPI->EV_WeaponAnimation(1, 2);
	}

	vec3_t vecSrc;
	EV_GetGunPosition(args, vecSrc, origin);

	if (EV_IsLocal(idx))
		EV_CannonFireEffect(ent->attachment[0] + 7.0 * forward, forward, velocity, idx, 0);
	else
		EV_CannonFireEffect(vecSrc + 40.0 * forward, forward, velocity, idx, 0);
}

void EV_FireEthereal(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
		
	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(AK47_SHOOT1 + gEngfuncs.pfnRandomLong(0, 2), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8196, BULLET_PLAYER_ETHEREAL, 0, 0, 2);
}

void EV_FireGilboaEX(struct event_args_s *args)
{
	if (!args->bparam2)
	{
		EV_FireGalil(args);

		return;
	}

	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 10000000.0;
	angles[1] += args->iparam2 / 10000000.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(5, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[2], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		VectorCopy(ent->attachment[1], ShellOrigin);

		float vec[4];
		for (int i = 0; i < 4; i++)
			vec[i] = WeaponData.fShellVec[i];

		EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, vec, lefthand == 0);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);

		for (int i = 0; i < 2; i++)
			vec[i] = WeaponData.fShellVec[i] - 30;

		for (int i = 2; i < 4; i++)
			vec[i] = WeaponData.fShellVec[i] - 10;

		EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, vec, lefthand == 0);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);
	}
	else
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);
	}


	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound2, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	VectorCopy(forward, vecAiming);

	float flSpread = args->fparam1;
	float shared_rand = args->fparam2;
	float x = UTIL_SharedRandomFloat(shared_rand, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 1, -0.5, 0.5);
	float y = UTIL_SharedRandomFloat(shared_rand + 2, -0.5, 0.5) + UTIL_SharedRandomFloat(shared_rand + 3, -0.5, 0.5);

	vSpread[0] = x * flSpread;
	vSpread[1] = y * flSpread;
	vSpread[2] = 0;

	EV_GetGunPosition(args, vecSrc, origin);
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2, 0, true, 1.0);
	EV_GetGunPosition(args, vecSrc, origin);
	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2, 2, false, 1.0);
}

void EV_FireM1Garand(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
	bool aiming, empty;

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	aiming = args->bparam1;
	empty = args->bparam2;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(aiming ? (empty ? 14 : 13) : (empty ? 12 : 11), 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);
	}

	if (EV_IsLocal(idx))
	{
		if (aiming)
		{
			float vec[4] = { 50.0, 70.0, 60.0, 90.0 };
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, vec, lefthand);
		}
		else
		{
			float vec[4] = { 50.0, 70.0, 60.0, 90.0 };
			EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, vec, lefthand);
		}

		VectorCopy(ent->attachment[1], ShellOrigin);
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, empty ? WeaponData.szSound2 : WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8196, BULLET_PLAYER_762MM, 0, 0, 2);
}

void EV_FireRPG7(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	
	bool aiming, empty;

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	aiming = args->bparam1;
	empty = args->bparam2;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		gEngfuncs.pEventAPI->EV_WeaponAnimation(aiming ? 5 : 4, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], up, 2.0, 0.5, 25, 25, 25, 3, velocity, false, 10);
			EV_HLDM_CreateSmoke(ent->attachment[0], up, 2.0, 0.5, 20, 20, 20, 2, velocity, false, 10);
			EV_HLDM_CreateSmoke(ent->attachment[0], up, 1.5, 0.45, 15, 15, 15, 2, velocity, false, 10);
			EV_HLDM_CreateSmoke(ent->attachment[0], up, 1.5, 0.35, 7, 7, 7, 2, velocity, false, 10);

			EV_HLDM_CreateSmoke(ent->attachment[1], up, 2.0, 0.5, 70, 70, 70, 3, velocity, false, 10);
			EV_HLDM_CreateSmoke(ent->attachment[1], up, 2.0, 0.5, 70, 70, 70, 2, velocity, false, 10);
			EV_HLDM_CreateSmoke(ent->attachment[1], up, 1.5, 0.45, 70, 70, 70, 2, velocity, false, 10);
			EV_HLDM_CreateSmoke(ent->attachment[1], up, 1.5, 0.35, 70, 70, 70, 2, velocity, false, 10);
		}
	}
	else
	{
		cl_entity_t *player = gEngfuncs.GetEntityByIndex(idx);

		Vector up, right, forward;
		gEngfuncs.pfnAngleVectors(player->angles, forward, right, up);

		Vector origin[2], player_origin;
		player_origin = player->origin;
		origin[0] = player_origin + forward * 30 + up * 10;
		origin[1] = player_origin + forward * -30 + up * 22 + right * 7;

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(origin[0], up, 3, 1.0, 100, 100, 100, 3, velocity, false, 15);
			EV_HLDM_CreateSmoke(origin[0], up, 3, 1.0, 50, 50, 50, 2, velocity, false, 15);
			EV_HLDM_CreateSmoke(origin[0], up, 3, 0.9, 50, 50, 50, 2, velocity, false, 15);
			EV_HLDM_CreateSmoke(origin[0], up, 3, 0.7, 50, 50, 50, 2, velocity, false, 15);

			EV_HLDM_CreateSmoke(origin[1], up, 10, 1.5, 100, 100, 100, 3, velocity, false, 10);
			EV_HLDM_CreateSmoke(origin[1], up, 10, 1.5, 100, 100, 100, 2, velocity, false, 10);
			EV_HLDM_CreateSmoke(origin[1], up, 10, 1.2, 100, 100, 100, 2, velocity, false, 10);
			EV_HLDM_CreateSmoke(origin[1], up, 10, 1.0, 100, 100, 100, 2, velocity, false, 10);

		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	if (EV_IsLocal(idx))
	{
		dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
		VectorCopy(ent->attachment[1], dl->origin);

		dl->radius = 300;
		dl->color.r = 255;
		dl->color.g = 249;
		dl->color.b = 146;
		dl->decay = 300;
		dl->die = cl.time + 1.0;
	}
	else
	{
		dlight_t *dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
		VectorCopy(args->origin, dl->origin);

		dl->radius = 300;
		dl->color.r = 255;
		dl->color.g = 249;
		dl->color.b = 146;
		dl->decay = 300;
		dl->die = cl.time + 1.0;
	}
}

void EV_FireDestroyer(struct event_args_s *args)
{
	int idx, iShell, lefthand;
	vec3_t origin, angles, velocity, forward, right, up, vecSrc, ShellOrigin, ShellVelocity, vecSpread;
	cl_entity_t *ent;
	idx = args->entindex;
	origin = args->origin;
	angles = args->angles;
	velocity = args->velocity;

	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;

	AngleVectors(angles, forward, right, up);

	ent = GetViewEntity();

	lefthand = cl_righthand->value < 1.0;

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(1, 2);

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.3, 35, 35, 35, 4, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 35, 0.35, 30, 30, 30, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 70, 0.3, 25, 25, 25, 2, velocity, false, 35);
		}
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/destroyer-1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
	EV_GetGunPosition(args, vecSrc, origin);
	vecSpread.x = args->fparam1;
	vecSpread.y = args->fparam2;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vecSpread, 8192.0, BULLET_PLAYER_338MAG, 0, 0, 5);

	iShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell_destroyer.mdl");

	if (EV_IsLocal(idx))
	{
		if (lefthand)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0, false);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 17.0, -8.0, 10.0, false);

		ShellVelocity.x *= 2.0;
		ShellVelocity.y *= 2.0;
		ShellOrigin = ent->attachment[1];
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);
	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], iShell, TE_BOUNCE_SHELL, idx, 17);
}

TEMPENTITY *CL_DefaultSprite(const vec3_t pos, model_t *sprite, float framerate)
{
	if (!sprite || sprite->type != mod_sprite)
		return NULL;
	TEMPENTITY	*pTemp;
	int		frameCount;

	frameCount = ModelFrameCount(sprite);

	pTemp = gEngfuncs.pEfxAPI->CL_TempEntAlloc((vec3_t)pos, sprite);
	if (!pTemp)
		return NULL;

	pTemp->frameMax = frameCount - 1;
	pTemp->entity.curstate.scale = 1.0f;
	pTemp->flags |= FTENT_SPRANIMATE;
	if (framerate == 0)
		framerate = 10;

	pTemp->entity.curstate.framerate = framerate;
	pTemp->die = cl.time + (float)frameCount / framerate;
	pTemp->entity.curstate.frame = 0;

	return pTemp;
}

void CL_Explosion(vec3_t origin, model_t *pModel, float flScale, float flFramerate, int iFlags)
{
	if (!pModel)
		return;
	if (flScale <= 0.0)
		return;
	TEMPENTITY *temp;
	temp = CL_DefaultSprite(origin, pModel, flFramerate);
	gEngfuncs.pEfxAPI->R_Sprite_Explode(temp, flScale, iFlags);

	if (!(iFlags & TE_EXPLFLAG_NODLIGHTS))
	{
		dlight_t *dl;
		dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
		dl->origin = origin;
		dl->radius = 200;
		dl->color.r = dl->color.g = 250;
		dl->color.b = 150;
		dl->die = cl.time + 0.25f;
		dl->decay = 800;

		dl = gEngfuncs.pEfxAPI->CL_AllocDlight(0);
		dl->origin = origin;
		dl->radius = 150;
		dl->color.r = 255;
		dl->color.g = 190;
		dl->color.b = 40;
		dl->die = cl.time + 1.0f;
		dl->decay = 200;
	}

	if (!(iFlags & TE_EXPLFLAG_NOPARTICLES))
		gEngfuncs.pEfxAPI->R_FlickerParticles(origin);

	if (iFlags & TE_EXPLFLAG_NOSOUND)
		return;

	gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_AUTO, va("weapons/explode%d.wav", gEngfuncs.pfnRandomLong(3, 5)), VOL_NORM, ATTN_NORM, 0, PITCH_NORM);
}

void EV_ExtractCompressedAir(vec3_t vecSrc, vec3_t vecDirection, vec3_t vecBaseVelocity, int idx)
{
	model_t *model = IEngineStudio.Mod_ForName("sprites/ef_aircyclone.spr", FALSE);
	TEMPENTITY *te = CL_DefaultSprite(vecSrc, model, 15);
	if (!te)
		return;
	float speed = gEngfuncs.pfnRandomLong(0, 5) + 400;
	te->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEWORLD;
	te->entity.angles.x = gEngfuncs.pfnRandomFloat(-512, 511);
	te->entity.angles.y = gEngfuncs.pfnRandomFloat(-256, 255);
	te->entity.angles.z = gEngfuncs.pfnRandomFloat(-256, 255);
	te->flags |= FTENT_ROTATE;
	te->entity.curstate.scale = 0.15;
	te->entity.baseline.fuser1 = 1.5;
	te->callback = DefaultScaleThink;
	te->hitcallback = DefaultFramerateTouch;
	te->entity.curstate.rendermode = kRenderTransAdd;
	te->entity.curstate.renderamt = 150;
	te->entity.curstate.renderfx = kRenderFxNone;
	te->entity.origin = vecSrc;
	te->entity.baseline.origin = vecDirection * speed;

	if (vecBaseVelocity)
	{
		te->entity.baseline.origin.x += 0.9 * vecBaseVelocity.x;
		te->entity.baseline.origin.y += 0.9 * vecBaseVelocity.y;
		te->entity.baseline.origin.z += 0.5 * vecBaseVelocity.z;
	}

	te->clientIndex = idx;
}

void EV_FireAirburster(event_args_s *args)
{
	vec3_t origin, vecSrc;
	vec3_t forward, right, up;
	int idx = args->entindex;
	int iType = args->bparam1;
	cl_entity_t *ent = gEngfuncs.GetViewModel();
	origin = args->origin;

	AngleVectors(args->angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, origin);

	if (!iType)
	{
		vec3_t vecStart;
		if (EV_IsLocal(idx))
		{
			EV_MuzzleFlash();
			gEngfuncs.pEventAPI->EV_WeaponAnimation(1, 2);
			vecStart = ent->attachment[0] + 7 * forward;
		}
		else
			vecStart = vecSrc;

		EV_ExtractCompressedAir(vecStart, forward, args->velocity, idx);

		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_ITEM, "weapons/airburster_shoot.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
	}
	else
	{
		if (EV_IsLocal(idx))
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(5, 2);
			EV_MuzzleFlash();
		}
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_ITEM, "weapons/airburster_shoot2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		CL_Explosion(vecSrc + forward * 50, IEngineStudio.Mod_ForName("sprites/ef_airexplosion.spr", 0), 1.5, 50.0, TE_EXPLFLAG_NOSOUND | TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES);
	}
}

void EV_FireSpsmg(event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;

	ent = gEngfuncs.GetViewModel();
	idx = args->entindex;
	lefthand = gEngfuncs.pfnGetCvarFloat("cl_righthand") < 1.0;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	if (!args->bparam1 && !args->bparam2)
	{
		angles.x += args->iparam1 / 100.0;
		angles.y += args->iparam2 / 100.0;
	}

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (!args->bparam1 && !args->bparam2)
	{
		if (EV_IsLocal(idx))
		{
			EV_MuzzleFlash();

			gEngfuncs.pEventAPI->EV_WeaponAnimation(3, 2);

			if (Hud().m_bCreateSmoke)
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.16, 7, 7, 7, 4, velocity, false, 35);
		}

		if (EV_IsLocal(idx))
		{
			float vecScale[3] = { 35.0, -16.0, -22.0 };

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

			VectorCopy(ent->attachment[1], ShellOrigin);
			VectorScale(ShellVelocity, 1.25, ShellVelocity);
			ShellVelocity[2] -= 50;
		}
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 10);

		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/spsmg-1.wav", 1.0, 0.64, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

		EV_GetGunPosition(args, vecSrc, origin);

		VectorCopy(forward, vecAiming);

		vSpread.x = args->fparam1;
		vSpread.y = args->fparam2;
		vSpread.z = 0;

		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_57MM, 0, 0, 2);
	}
	else
	{
		if (EV_IsLocal(idx))
		{
			EV_MuzzleFlash();
			gEngfuncs.pEventAPI->EV_WeaponAnimation(4, 2);

			if (Hud().m_bCreateSmoke)
			{
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 15, 15, 15, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.35, 9, 9, 9, 2, velocity, 0, 35);
			}

			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.2, 55, 50, 50, 2, velocity + up * 50, 0, 30);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.3, 25, 20, 20, 2, velocity + up * 50, 0, 25);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.4, 30, 25, 25, 3, velocity + up * 50, 0, 25);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.5, 30, 25, 25, 3, velocity + up * 80, 0, 35);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.4, 65, 65, 65, 3, velocity + up * 80, 0, 35);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.4, 20, 15, 15, 2, velocity + up * 80, 0, 25);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.1, 50, 50, 50, 3, velocity, 0, 30);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.12, 35, 30, 35, 2, velocity, 0, 30);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.15, 30, 30, 25, 3, velocity, 0, 30);
			EV_HLDM_CreateSmoke(ent->attachment[2], up, 0, 0.18, 35, 40, 40, 2, velocity, 0, 30);
		}

		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/spsmg-2.wav", 1.0, 0.64, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

		EV_GetGunPosition(args, vecSrc, origin);

		VectorCopy(forward, vecAiming);

		vSpread.x = args->fparam1;
		vSpread.y = args->fparam2;
		vSpread.z = 0.0;

		EV_HLDM_FireBullets(idx, forward, right, up, args->iparam1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_46MM, 0, 0, 2);
	}
}

void EV_FireSGDrill(event_args_s *args)
{
	vec3_t origin, angles, velocity;
	vec3_t forward, right, up;
	vec3_t ShellOrigin, ShellVelocity;
	vec3_t vecDirShooting;
	vec3_t vecSrc, vecSpread;
	origin = args->origin;
	velocity = args->velocity;
	angles = args->angles;

	cl_entity_t *ent = GetViewEntity();

	int cShots;
	int idx = args->entindex;
	int lefthand = gEngfuncs.pfnGetCvarFloat("cl_righthand") < 1.0;

	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;

	AngleVectors(angles, forward, right, up);

	int iShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell_sgdrill.mdl");

	if (args->bparam1)
	{
		if (args->bparam1 == 1)
		{
			if (EV_IsLocal(idx))
			{
				g_iShotsFired++;
				gEngfuncs.pEventAPI->EV_WeaponAnimation(2, 2);
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sgdrill_slash.wav", VOL_NORM, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sgdrill_Pslash.wav", VOL_NORM, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
		}
	}
	else
	{
		if (EV_IsLocal(idx))
		{
			EV_MuzzleFlash();
			gEngfuncs.pEventAPI->EV_WeaponAnimation(1, 2);
			if (gEngfuncs.pfnGetCvarFloat("cl_gunsmoke"))
			{
				EV_HLDM_CreateSmoke(
					ent->attachment[0],
					forward,
					3,
					0.45,
					15,
					15,
					15,
					3,
					velocity,
					0,
					35);
				EV_HLDM_CreateSmoke(
					ent->attachment[0],
					forward,
					40,
					0.35,
					9,
					9,
					9,
					2,
					velocity,
					0,
					35);
			}

			g_iShotsFired++;

			if (lefthand)
				EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				22,
				-9,
				-11,
				false);
			else
				EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				22,
				-9,
				11,
				false);

			ShellVelocity *= 1.25;
			ShellOrigin = ent->attachment[1];
		}
		else
		{
			EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				20,
				-12,
				4,
				false);
		}

		EV_EjectBrass(
			ShellOrigin,
			ShellVelocity,
			angles[YAW],
			iShell,
			TE_BOUNCE_SHOTSHELL,
			idx,
			3);

		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/sgdrill-1.wav", VOL_NORM, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));

		EV_GetGunPosition(args, vecSrc, origin);
		vecDirShooting = forward;
		if (args->fparam1)
		{
			vecSpread.x = args->fparam1 / 1000.0;
			vecSpread.y = args->fparam1 / 1000.0;
		}
		else
		{
			vecSpread.x = 0.03;
			vecSpread.y = 0.03;
		}

		vecSpread.z = 0.0;

		if (args->fparam2)
			cShots = args->fparam2;
		else
			cShots = 8;

		EV_HLDM_FireBullets(
			idx,
			forward,
			right,
			up,
			cShots,
			vecSrc,
			vecDirShooting,
			vecSpread,
			8192,
			BULLET_PLAYER_BUCKSHOT,
			0,
			0,
			1);
	}
}

void FollowThink(tempent_s *ent, float frametime, float currenttime);

void CreateBalrog11CannonSingleProjectile(TEMPENTITY *prev, vec3_t origin, int iAimingEntity)
{
	cl_entity_t *ent = gEngfuncs.GetEntityByIndex(iAimingEntity);
	if (!ent)
		return;
	if (prev && (prev->entity.origin - origin).Length() < 5)	// Why not other numbers??
		return;
	static int iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/flame_puff01.spr");
	TEMPENTITY *te = gEngfuncs.pEfxAPI->R_DefaultSprite(origin, iModelIndex, 48);
	if (te)
	{
		te->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEKILL;
		te->entity.angles.y = gEngfuncs.pfnRandomFloat(-512, 511);
		te->entity.angles.x = gEngfuncs.pfnRandomFloat(-256, 255);
		te->entity.angles.z = gEngfuncs.pfnRandomFloat(-256, 255);
		te->flags |= FTENT_ROTATE;
		te->entity.curstate.scale = 0.01;
		te->entity.baseline.fuser1 = 0.3;

		te->callback = FollowThink;
		te->hitcallback = DefaultFramerateTouch;

		te->entity.curstate.rendermode = kRenderTransAdd;
		te->entity.curstate.renderamt = 40.0f;
		te->entity.curstate.renderfx = kRenderFxNone;
		
		te->entity.baseline.origin = Vector(0, 0, 0);
		te->entity.baseline.iuser1 = ent->index;
		te->entity.curstate.iuser1 = 0;
		te->clientIndex = ent->curstate.owner;
	}
}

void FollowThink(tempent_s *ent, float frametime, float currenttime)
{
	DefaultScaleThink(ent, frametime, currenttime);
	
	static cl_entity_t *pEntity;
	pEntity = gEngfuncs.GetEntityByIndex(ent->entity.baseline.iuser1);

	if (!pEntity)
	{
		ent->die = currenttime;
		return;
	}

	if (ent->die - currenttime <= frametime * 100 && !ent->entity.curstate.iuser1)
	{
		CreateBalrog11CannonSingleProjectile(ent, pEntity->origin, pEntity->index);
		ent->entity.curstate.iuser1 = 1;
	}
}

void EV_Balrog11FollowEntities(event_args_s *args)
{
	CreateBalrog11CannonSingleProjectile(NULL, args->origin, args->entindex);
}

void EV_FireBlockAR(struct event_args_s *args)
{
	vec3_t origin, angles, velocity;
	vec3_t forward, right, up;
	vec3_t ShellOrigin, ShellVelocity;
	vec3_t vecDirShooting;
	vec3_t vecSrc, vecSpread;
	origin = args->origin;
	velocity = args->velocity;
	angles = args->angles;

	cl_entity_t *ent = GetViewEntity();
	int idx = args->entindex;
	int iShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/block_shell.mdl");

	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;

	AngleVectors(angles, forward, right, up);

	if (args->bparam1)
	{
		if (EV_IsLocal(idx))
			gEngfuncs.pEfxAPI->R_BreakModel(ent->attachment[0] + forward * 35, Vector(10.0, 10.0, 10.0), Vector(0, 0, 0), 100, 3, 10, iShell, BREAK_CONCRETE);
	}
	else
	{
		if (EV_IsLocal(idx))
		{
			g_iShotsFired++;
			EV_MuzzleFlash();
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(0, 2) + 1, 2);
			if (Hud().m_bCreateSmoke)
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);

			if (gEngfuncs.pfnGetCvarFloat("cl_righthand") < 1.0)
				EV_GetDefaultShellInfo(
					args, 
					origin, 
					velocity, 
					ShellVelocity, 
					ShellOrigin, 
					forward, 
					right, 
					up, 
					20.0, 
					-8.0, 
					-10.0, 
					false);
			else
				EV_GetDefaultShellInfo(
					args, 
					origin,
					velocity,
					ShellVelocity, 
					ShellOrigin, 
					forward, 
					right, 
					up, 
					20.0, 
					-8.0, 
					10.0, 
					false);
			
			ShellOrigin = ent->attachment[1];
			ShellVelocity.z -= 75;
		}
		else
		{
			EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				20.0,
				-12.0,
				4.0,
				false);
		}

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], iShell, TE_BOUNCE_SHELL, idx, 9);
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/blockar1-1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
	
		EV_GetGunPosition(args, vecSrc, origin);
		vecSpread = Vector(args->fparam1, args->fparam2, 0);
		vecDirShooting = forward;

		EV_HLDM_FireBullets(
			idx,
			forward,
			right,
			up,
			1,
			vecSrc,
			vecDirShooting,
			vecSpread,
			8192.0,
			BULLET_PLAYER_762MM,
			0,
			0,
			2
			);
	}
}

void EV_StormGiant(struct event_args_s *args)
{
	int idx = args->entindex;
	vec3_t origin = args->origin, angles = args->angles, forward, vecSrc;

	AngleVectors(angles, forward, NULL, NULL);

	EV_GetGunPosition(args, vecSrc, origin);
	vec3_t vecEnd = vecSrc + forward * args->fparam1;

	if (EV_IsLocal(idx))
	{
		HudStatistics().m_iShoots++;
	}

	switch (args->iparam1)
	{
	case 0:	// RESULT_HIT_NONE
	{
			switch (args->iparam2)
			{
			case 0:
			{
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_draw_attack.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
					break;
			}
			case 1:
			case 2:
			{
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_midslash1_fail.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
					break;
			}
			case 3:
			{
				      gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_midslash2_fail.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				      break;
			}
			}

			break;
	}
	case 2:	// RESULT_HIT_WORLD
	{
			static pmtrace_t tr;
			model_t *model = IEngineStudio.Mod_ForName("sprites/ef_coilmg.spr", FALSE);
			
			gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
			gEngfuncs.pEventAPI->EV_PushPMStates();
			gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
			gEngfuncs.pEventAPI->EV_SetTraceHull(2);
			gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, -1, &tr);

			char chTextureType, *pTextureName, texname[64], szbuffer[64];
			
			if (tr.fraction < 1.0)
			{
				pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture(tr.ent, vecSrc, vecEnd);

				if (pTextureName)
				{
					strcpy(texname, pTextureName);
					pTextureName = texname;

					// strip leading '-0' or '+0~' or '{' or '!'
					if (*pTextureName == '-' || *pTextureName == '+')
					{
						pTextureName += 2;
					}

					if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
					{
						pTextureName++;
					}

					// '}}'
					strcpy(szbuffer, pTextureName);
					szbuffer[CBTEXTURENAMEMAX - 1] = 0;

					// get texture type
					chTextureType = PM_FindTextureType(szbuffer);
				}
				
				TEMPENTITY *te = CL_DefaultSprite(tr.endpos, model, 10);
				if (te)
				{
					te->entity.curstate.rendermode = kRenderTransAdd;
					te->entity.curstate.renderamt = 200;
					te->entity.curstate.scale = 0.3;
				}
			}

			gEngfuncs.pEventAPI->EV_PopPMStates();

			switch (chTextureType)
			{
			case CHAR_TEX_METAL:
			case CHAR_TEX_VENT:
			case CHAR_TEX_COMPUTER:
				gEngfuncs.pEventAPI->EV_PlaySound(-1, tr.endpos, CHAN_VOICE, gEngfuncs.pfnRandomLong(0, 1) ? "weapons/stormgiant_metal1.wav" : "weapons/stormgiant_metal2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			case CHAR_TEX_WOOD:
				gEngfuncs.pEventAPI->EV_PlaySound(-1, tr.endpos, CHAN_VOICE, gEngfuncs.pfnRandomLong(0, 1) ? "weapons/stormgiant_wood1.wav" : "weapons/stormgiant_wood2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			default:
				gEngfuncs.pEventAPI->EV_PlaySound(-1, tr.endpos, CHAN_VOICE, gEngfuncs.pfnRandomLong(0, 1) ? "weapons/stormgiant_stone1.wav" : "weapons/stormgiant_stone2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			}

			switch (args->iparam2)
			{
			case 1:
			{
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_attack_hit2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
					break;
			}
			case 2:
			{
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_midslash1_hit.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
					break;
			}
			case 3:
			{
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_midslash2_hit.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
					break;
			}
			default:
					break;
			}

		      break;
	}
	case 1:	// RESULT_HIT_PLAYER
	{
			if (EV_IsLocal(idx))
			{
				HudStatistics().m_iHit++;

				pmtrace_t tr;

				gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, false);
				gEngfuncs.pEventAPI->EV_PushPMStates();
				gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
				gEngfuncs.pEventAPI->EV_SetTraceHull(2);
				gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_STUDIO_IGNORE, -1, &tr);

				if (tr.ent > 0)
				{
					physent_t *ent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);

					if (ent->info >= 1 && ent->info <= 32)
					{
						if (vPlayer[ent->info].team != g_iTeam || g_iMod == MOD_DM)
							HudAmmo().Hit(cl.time);
					}
				}

				gEngfuncs.pEventAPI->EV_PopPMStates();
			}
			
			switch (args->iparam2)
			{
			case 0:
			{
				if (!gEngfuncs.pfnRandomLong(0, 1))
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				else
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			}
			case 1:
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_attack_hit2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				if (!gEngfuncs.pfnRandomLong(0, 1))
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				else
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			}
			case 2:
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_midslash1_hit.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				if (!gEngfuncs.pfnRandomLong(0, 1))
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				else
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			}
			case 3:
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/stormgiant_midslash2_hit.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				if (!gEngfuncs.pfnRandomLong(0, 1))
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				else
					gEngfuncs.pEventAPI->EV_PlaySound(-1, origin, CHAN_WEAPON, "weapons/stormgiant_hit2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				break;
			}
			}

		      break;
	}
	}
}

void EV_StormGiantEffect(struct event_args_s *args)
{
	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(args->entindex);
	model_t *model = IEngineStudio.Mod_ForName("sprites/ef_coilmg.spr", FALSE);

	if (model)
	{
		if (pEntity)
		{
			TEMPENTITY *te = CL_DefaultSprite(pEntity->origin, model, 10);
			if (te)
			{
				te->entity.curstate.rendermode = kRenderTransAdd;
				te->entity.curstate.renderamt = 200;
				te->entity.curstate.scale = 0.3;
			}
		}
	}
}

void EV_FireBuffM4A1(struct event_args_s *args)
{
	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/smoke.spr");
	int idx = args->entindex;
	int iShell;
	vec3_t origin, angles, velocity, forward, right, up, vecSrc, vecSpread, vecBeamStart, vecEnd, ShellOrigin, ShellAngles, ShellVelocity;
	cl_entity_t *ent = gEngfuncs.GetViewModel();
	origin = args->origin;
	angles = args->angles;
	velocity = args->velocity;
	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;
	AngleVectors(angles, forward, right, up);
	EV_GetGunPosition(args, vecSrc, origin);
	vecSpread = Vector(args->fparam1, args->fparam2, 0);
	vecBeamStart = vecSrc + vecSpread.x * right + vecSpread.y * up;
	vecEnd = vecSrc + 8196.0f * forward + right * vecSpread.x + up * vecSpread.y;

	if (args->bparam1)
	{
		BEAM *pBeam = gEngfuncs.pEfxAPI->R_BeamCirclePoints(0, vecBeamStart, vecEnd, iBeamModelIndex, 1.0f, 0.75f, 0.0f, 40.0f / 255.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f);
		if (pBeam)
			pBeam->flags |= FBEAM_FADEIN;
	}

	iShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(3 + gEngfuncs.pfnRandomLong(0, 2), 2);
		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);

		Vector vecScale = Vector(20.0, -8.0, 10.0);

		if (gEngfuncs.pfnGetCvarFloat("cl_righthand") >= 1.0)
			vecScale.z = 0 - vecScale.z;
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale.x, vecScale.y, vecScale.z, true);
		ShellVelocity.z -= 45.0f;
		ShellOrigin = ent->attachment[2];
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);
	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], iShell, TE_BOUNCE_SHELL, idx, 10);
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? "weapons/m4a1buff-2.wav" : "weapons/m4a1buff-1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vecSpread, 8192, BULLET_PLAYER_762MM, 0, 0, 2);
}

void EV_FireBuffAK47(struct event_args_s *args)
{
	vec3_t origin, velocity, angles, forward, right, up, vecSrc, vecAiming, ShellVelocity, ShellOrigin, vecSpread;
	cl_entity_t *ent = GetViewEntity();
	int lefthand = gEngfuncs.pfnGetCvarFloat("cl_righthand") < 1.0;
	int idx = args->entindex;
	origin = args->origin;
	velocity = args->velocity;
	angles = args->angles;
	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;
	AngleVectors(angles, forward, right, up);
	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");
	if (EV_IsLocal(idx))
	{
		EV_MuzzleFlash();
		gEngfuncs.pEventAPI->EV_WeaponAnimation(3 + gEngfuncs.pfnRandomLong(0, 2), 2);
		if (gEngfuncs.pfnGetCvarFloat("cl_gunsmoke")) EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);

		if (lefthand)
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, -10.0, false);
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -8.0, 10.0, false);
		ShellOrigin = ent->attachment[1];
	}
	else
	{
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);
	}

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, 1, idx, 9);

	if (args->bparam1)
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/ak47buff-2.wav", 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
	else
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/ak47buff-1.wav", 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
		EV_GetGunPosition(args, vecSrc, origin);
		vecSpread = Vector(args->fparam1, args->fparam2, 0);
		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, forward, vecSpread, 8192.0, BULLET_PLAYER_556MM, 0, 0, 2);
	}
}

void EV_FireBow(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;

	cl_entity_s *ent;
	ent = gEngfuncs.GetViewModel();

	idx = args->entindex;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	EV_HLDM_FireBullets(idx, forward, right, up, 1, EV_IsLocal(idx) ? ent->attachment[0] : vecSrc, vecAiming, Vector(0, 0, 0), 8192, BULLET_PLAYER_338MAG, 0, 0, 8);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->iparam1 ? "sound/weapons/bow_charge-2.wav" : "sound/weapons/bow_charge-1_empty.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));

	int iBeamModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/smoke.spr");

	vec3_t vecDir, vecEnd;

	vecDir = vecAiming;
	vecEnd = vecSrc + 8196.0f * vecDir;

	BEAM *pBeam;
	if (EV_IsLocal(idx))
		pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(ent->attachment[0], vecEnd, iBeamModelIndex, 1.0, 3.0, 0.0, 128.0, 0, 0, 0, 0.78, 0.37, 0.29);
	else
		pBeam = gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc, vecEnd, iBeamModelIndex, 1.0, 3.0, 0.0, 128.0, 0, 0, 0, 0.78, 0.37, 0.29);

	if (pBeam)
		pBeam->flags |= FBEAM_FADEIN | FBEAM_FADEOUT | FBEAM_SHADEIN;
}

#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch
#define	GAUSS_PRIMARY_CHARGE_VOLUME	256// how loud gauss is while charging
#define GAUSS_PRIMARY_FIRE_VOLUME	450// how loud gauss is when discharged

void EV_SpinGauss(event_args_t *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int iSoundState = 0;

	int pitch;

	idx = args->entindex;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	pitch = args->iparam1;

	iSoundState = args->bparam1 ? SND_CHANGE_PITCH : 0;

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "ambience/pulsemachine.wav", 1.0, ATTN_NORM, iSoundState, pitch);
}

/*
==============================
EV_StopPreviousGauss

==============================
*/
void EV_StopPreviousGauss(int idx)
{
	// Make sure we don't have a gauss spin event in the queue for this guy
	gEngfuncs.pEventAPI->EV_KillEvents(idx, "events/gaussspin.sc");
	gEngfuncs.pEventAPI->EV_StopSound(idx, CHAN_WEAPON, "ambience/pulsemachine.wav");
}

enum gauss_e {
	GAUSS_IDLE = 0,
	GAUSS_IDLE2,
	GAUSS_FIDGET,
	GAUSS_SPINUP,
	GAUSS_SPIN,
	GAUSS_FIRE,
	GAUSS_FIRE2,
	GAUSS_HOLSTER,
	GAUSS_DRAW
};

extern float g_flApplyVel;

void EV_FireGauss(event_args_t *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float flDamage = args->fparam1;
	int primaryfire = args->bparam1;

	int m_fPrimaryFire = args->bparam1;
	int m_iWeaponVolume = GAUSS_PRIMARY_FIRE_VOLUME;
	vec3_t vecSrc;
	vec3_t vecDest;
	edict_t		*pentIgnore;
	pmtrace_t tr, beam_tr;
	float flMaxFrac = 1.0;
	int	nTotal = 0;
	int fHasPunched = 0;
	int fFirstBeam = 1;
	int	nMaxHits = 10;
	physent_t *pEntity;
	int m_iBeam, m_iGlow, m_iBalls;
	vec3_t up, right, forward;

	idx = args->entindex;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	if (args->bparam2)
	{
		EV_StopPreviousGauss(idx);
		return;
	}

	//	Con_Printf( "Firing gauss with %f\n", flDamage );
	EV_GetGunPosition(args, vecSrc, origin);

	m_iBeam = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/smoke.spr");
	m_iBalls = m_iGlow = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/hotglow.spr");

	AngleVectors(angles, forward, right, up);

	VectorMA(vecSrc, 8192, forward, vecDest);

	if (EV_IsLocal(idx))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(GAUSS_FIRE2, 2);

		g_flApplyVel = flDamage;
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/gauss2.wav", 0.5 + flDamage * (1.0 / 400.0), ATTN_NORM, 0, 85 + gEngfuncs.pfnRandomLong(0, 0x1f));

	while (flDamage > 10 && nMaxHits > 0)
	{
		nMaxHits--;

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);

		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();

		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);

		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecDest, PM_STUDIO_BOX, -1, &tr);

		gEngfuncs.pEventAPI->EV_PopPMStates();

		if (tr.allsolid)
			break;

		if (fFirstBeam)
		{
			if (EV_IsLocal(idx))
			{
				// Add muzzle flash to current weapon model
				EV_MuzzleFlash();
			}
			fFirstBeam = 0;

			gEngfuncs.pEfxAPI->R_BeamEntPoint(
				idx | 0x1000,
				tr.endpos,
				m_iBeam,
				0.1,
				m_fPrimaryFire ? 1.0 : 2.5,
				0.0,
				m_fPrimaryFire ? 128.0 : flDamage,
				0,
				0,
				0,
				m_fPrimaryFire ? 255 : 255,
				m_fPrimaryFire ? 128 : 255,
				m_fPrimaryFire ? 0 : 255
				);
		}
		else
		{
			gEngfuncs.pEfxAPI->R_BeamPoints(vecSrc,
				tr.endpos,
				m_iBeam,
				0.1,
				m_fPrimaryFire ? 1.0 : 2.5,
				0.0,
				m_fPrimaryFire ? 128.0 : flDamage,
				0,
				0,
				0,
				m_fPrimaryFire ? 255 : 255,
				m_fPrimaryFire ? 128 : 255,
				m_fPrimaryFire ? 0 : 255
				);
		}

		pEntity = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);
		if (pEntity == NULL)
			break;

		if (pEntity->solid == SOLID_BSP)
		{
			float n;

			pentIgnore = NULL;

			n = -DotProduct(tr.plane.normal, forward);

			if (n < 0.5) // 60 degrees	
			{
				// ALERT( at_console, "reflect %f\n", n );
				// reflect
				vec3_t r;

				VectorMA(forward, 2.0 * n, tr.plane.normal, r);

				flMaxFrac = flMaxFrac - tr.fraction;

				VectorCopy(r, forward);

				VectorMA(tr.endpos, 8.0, forward, vecSrc);
				VectorMA(vecSrc, 8192.0, forward, vecDest);

				gEngfuncs.pEfxAPI->R_TempSprite(tr.endpos, Vector(0, 0, 0), 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage * n / 255.0, flDamage * n * 0.5 * 0.1, FTENT_FADEOUT);

				vec3_t fwd;
				VectorAdd(tr.endpos, tr.plane.normal, fwd);

				gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 3, 0.1, gEngfuncs.pfnRandomFloat(10, 20) / 100.0, 100,
					255, 100);

				// lose energy
				if (n == 0)
				{
					n = 0.1;
				}

				flDamage = flDamage * (1 - n);

			}
			else
			{
				// tunnel
				//EV_HLDM_DecalGunshot(&tr, BULLET_MONSTER_12MM);

				char chTextureType = EV_HLDM_PlayTextureSound(idx, &tr, vecSrc, vecDest, BULLET_MONSTER_12MM);
				int r = 40, g = 40, b = 40;
				bool bStreakSplash = false;
				float flScale = 0.4;

				switch (chTextureType)
				{
				case CHAR_TEX_METAL:
					bStreakSplash = true;
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_CONCRETE:
					bStreakSplash = true;
					r = g = b = 65;
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_GRATE:
				case CHAR_TEX_VENT:
					bStreakSplash = true;
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_TILE:
					bStreakSplash = true;
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_COMPUTER:
					bStreakSplash = true;
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				case CHAR_TEX_WOOD:
					bStreakSplash = false;
					r = 75;
					g = 42;
					b = 15;
					flScale = 0.5;
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				default:
					EV_HLDM_DecalGunShot(&tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
					break;
				}

				gEngfuncs.pEfxAPI->R_TempSprite(tr.endpos, Vector(0, 0, 0), 1.0, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT);

				// limit it to one hole punch
				if (fHasPunched)
				{
					break;
				}
				fHasPunched = 1;

				// try punching through wall if secondary attack (primary is incapable of breaking through)
				if (!m_fPrimaryFire)
				{
					vec3_t start;

					VectorMA(tr.endpos, 8.0, forward, start);

					// Store off the old count
					gEngfuncs.pEventAPI->EV_PushPMStates();

					// Now add in all of the players.
					gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);

					gEngfuncs.pEventAPI->EV_SetTraceHull(2);
					gEngfuncs.pEventAPI->EV_PlayerTrace(start, vecDest, PM_STUDIO_BOX, -1, &beam_tr);

					if (!beam_tr.allsolid)
					{
						vec3_t delta;
						float n;

						// trace backwards to find exit point

						gEngfuncs.pEventAPI->EV_PlayerTrace(beam_tr.endpos, tr.endpos, PM_STUDIO_BOX, -1, &beam_tr);

						VectorSubtract(beam_tr.endpos, tr.endpos, delta);

						n = delta.Length();

						if (n < flDamage)
						{
							if (n == 0)
								n = 1;
							flDamage -= n;

							// absorption balls
							{
								vec3_t fwd;
								VectorSubtract(tr.endpos, forward, fwd);
								gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 3, 0.1, gEngfuncs.pfnRandomFloat(10, 20) / 100.0, 100,
									255, 100);
							}

							//////////////////////////////////// WHAT TO DO HERE
							// CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );

							//EV_HLDM_DecalGunshot(&beam_tr, BULLET_MONSTER_12MM);

							char chTextureType = EV_HLDM_PlayTextureSound(idx, &beam_tr, vecSrc, vecDest, BULLET_MONSTER_12MM);
							int r = 40, g = 40, b = 40;
							bool bStreakSplash = false;
							float flScale = 0.4;

							switch (chTextureType)
							{
							case CHAR_TEX_METAL:
								bStreakSplash = true;
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							case CHAR_TEX_CONCRETE:
								bStreakSplash = true;
								r = g = b = 65;
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							case CHAR_TEX_GRATE:
							case CHAR_TEX_VENT:
								bStreakSplash = true;
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							case CHAR_TEX_TILE:
								bStreakSplash = true;
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							case CHAR_TEX_COMPUTER:
								bStreakSplash = true;
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							case CHAR_TEX_WOOD:
								bStreakSplash = false;
								r = 75;
								g = 42;
								b = 15;
								flScale = 0.5;
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							default:
								EV_HLDM_DecalGunShot(&beam_tr, BULLET_MONSTER_12MM, flScale, r, g, b, bStreakSplash, chTextureType);
								break;
							}

							gEngfuncs.pEfxAPI->R_TempSprite(beam_tr.endpos, Vector(0, 0, 0), 0.1, m_iGlow, kRenderGlow, kRenderFxNoDissipation, flDamage / 255.0, 6.0, FTENT_FADEOUT);

							// balls
							{
								vec3_t fwd;
								VectorSubtract(beam_tr.endpos, forward, fwd);
								gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, beam_tr.endpos, fwd, m_iBalls, (int)(flDamage * 0.3), 0.1, gEngfuncs.pfnRandomFloat(10, 20) / 100.0, 200,
									255, 40);
							}

							VectorAdd(beam_tr.endpos, forward, vecSrc);
						}
					}
					else
					{
						flDamage = 0;
					}

					gEngfuncs.pEventAPI->EV_PopPMStates();
				}
				else
				{
					if (m_fPrimaryFire)
					{
						// slug doesn't punch through ever with primary 
						// fire, so leave a little glowy bit and make some balls
						gEngfuncs.pEfxAPI->R_TempSprite(tr.endpos, Vector(0, 0, 0), 0.2, m_iGlow, kRenderGlow, kRenderFxNoDissipation, 200.0 / 255.0, 0.3, FTENT_FADEOUT);

						{
							vec3_t fwd;
							VectorAdd(tr.endpos, tr.plane.normal, fwd);
							gEngfuncs.pEfxAPI->R_Sprite_Trail(TE_SPRITETRAIL, tr.endpos, fwd, m_iBalls, 8, 0.6, gEngfuncs.pfnRandomFloat(10, 20) / 100.0, 100,
								255, 200);
						}
					}

					flDamage = 0;
				}
			}
		}
		else
		{
			VectorAdd(tr.endpos, forward, vecSrc);
		}
	}
}

void EV_FireBuffSG552(struct event_args_s *args)
{
	vec3_t origin, angles, velocity, forward, right, up;
	vec3_t ShellOrigin, ShellVelocity;
	vec3_t vecSrc, vecSpread;
	cl_entity_t *ent;
	int idx, lefthand, shell;

	ent = GetViewEntity();

	if (cl_righthand->value)
		lefthand = 0;
	else
		lefthand = 1;

	idx = args->entindex;

	origin = args->origin;
	angles = args->angles;

	AngleVectors(angles, forward, right, up);	// ??

	angles.x += args->iparam1 / 100.0;	// ??
	angles.y += args->iparam2 / 100.0;	// ??

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash( );

		gEngfuncs.pEventAPI->EV_WeaponAnimation( 3 + RANDOM_LONG(0, 2), 2 );

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(
							ent->attachment[0],
							forward,
							3,
							0.3,
							35,
							35,
							35,
							4,
							velocity,
							false,
							35
							);
			EV_HLDM_CreateSmoke(
							ent->attachment[0],
							forward,
							35,
							0.35,
							30,
							30,
							30,
							2,
							velocity,
							false,
							35
							);
			EV_HLDM_CreateSmoke(
							ent->attachment[0],
							forward,
							70,
							0.3,
							25,
							25,
							25,
							2,
							velocity, 
							false,
							35
							);
		}
	}

	if ( EV_IsLocal( idx ) )
	{
		if ( lefthand )
		{
			EV_GetDefaultShellInfo(
							args,
							origin,
							velocity,
							ShellVelocity,
							ShellOrigin,
							forward,
							right,
							up,
							20.0,
							-8.0,
							-10.0,
							false
							);
		}
		else
		{
			EV_GetDefaultShellInfo(
							args,
							origin,
							velocity,
							ShellVelocity,
							ShellOrigin,
							forward,
							right,
							up,
							17.0,
							-8.0,
							10.0,
							false
							);

			ShellVelocity *= 1.5;
			ShellVelocity.z -= 50.0;

			ShellOrigin = ent->attachment[1];
		}
	}
	else
		EV_GetDefaultShellInfo(
						args,
						origin,
						velocity,
						ShellVelocity,
						ShellOrigin,
						forward,
						right,
						up,
						20.0,
						-12.0,
						4.0,
						false
						);

	EV_EjectBrass(
			ShellOrigin, 
			ShellVelocity, 
			angles[ YAW ], 
			shell, 
			TE_BOUNCE_SHELL, 
			idx, 
			17
			);

	gEngfuncs.pEventAPI->EV_PlaySound(
							idx,
							origin,
							1,
							"weapons/buffsg552-1.wav",
							VOL_NORM,
							0.4,	// ??
							0,
							RANDOM_LONG(0, 15) + 94
							);

	EV_GetGunPosition(args, vecSrc, origin);

	vecSpread = Vector(
				args->fparam1,
				args->fparam2,
				0.0
				);

	EV_HLDM_FireBullets(
				idx,
				forward,
				right,
				up,
				1,
				vecSrc,
				forward,
				vecSpread,
				8192.0,
				BULLET_PLAYER_556MM,
				0,
				NULL,
				2
				);
}

void EV_FireCannonEX(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	

	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int lefthand;

	lefthand = cl_righthand->value;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles.y -= 18;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);


	// iparam1 : fire1 = 0,fire2,fire_return,surround,fire_spec,surrund_attack
	if (args->iparam1 == 0 || args->iparam1 == 1 || args->iparam1 == 2)
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/cannonex_shoota.wav", 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

		if (EV_IsLocal(idx))
		{
			//g_iShotsFired++;

			if (Hud().m_bCreateSmoke)
			{
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 25, 25, 25, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.35, 20, 20, 20, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 80, 0.5, 15, 15, 15, 3, velocity, false, 35);
			}
			if (args->iparam1 == 0)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(4, 2);
			else if (args->iparam1 == 1)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(5, 2);
			else if (args->iparam1 == 2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(7, 2);
		}

		vec3_t vecSrc;
		EV_GetGunPosition(args, vecSrc, origin);

		if (EV_IsLocal(idx))
			EV_CannonFireEffect(ent->attachment[0] + 7.0 * forward, forward, velocity, idx, 0);
		else
			EV_CannonFireEffect(vecSrc + 40.0 * forward, forward, velocity, idx, 0);
	}
	else if (args->iparam1 == 3)
	{
		if (EV_IsLocal(idx))
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 2);
		}

		struct model_s *pModel;
		TEMPENTITY *pEnt;

		pModel = IEngineStudio.Mod_ForName("models/p_cannonexdragonfx.mdl", false);

		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
		pEnt->entity.curstate.frame = 0;
		pEnt->entity.curstate.framerate = 1;
		pEnt->tentOffset = Vector(0,0,0);
		pEnt->die = cl.time + 3.0;

		pEnt->clientIndex = idx;
		pEnt->flags |= FTENT_PLYRATTACHMENT;
	}
	else if (args->iparam1 == 4)
	{
		vec3_t vecSrc;
		EV_GetGunPosition(args, vecSrc, origin);

		if (EV_IsLocal(idx))
			AngleVectors(v_angles, forward, right, up);
		else
			AngleVectors(args->angles, forward, right, up);

		EV_GetGunPosition(args, vecSrc, args->origin);

		float vecVelocity[3];
		VectorCopy(args->velocity, vecVelocity);
		vecVelocity[2] -= 45.0;

		if (EV_IsLocal(idx))
			EV_CreateFlameThrowerFlame(ent->attachment[0] + forward * 7 + up * 20, forward, vecVelocity, idx);
		else
			EV_CreateFlameThrowerFlame(vecSrc + up * 20, forward, vecVelocity, idx);

		gEngfuncs.pEventAPI->EV_PlaySound(idx, args->origin, CHAN_WEAPON, "weapons/flamethrower-1.wav", 1.0, ATTN_NORM, 0, 94 + RANDOM_LONG(0, 15));
	}
	else if (args->iparam1 == 5)
	{
		//if (EV_IsLocal(idx))
		//{
			//gEngfuncs.pEventAPI->EV_WeaponAnimation(1, 2);
		//}
		//vec3_t vecSrc;
		//EV_GetGunPosition(args, vecSrc, origin);
		//gEngfuncs.pEfxAPI->R_Explosion(ent->attachment[0], gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/ef_cannonex.spr"), 10, 15, TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND);
		CL_Explosion(
			ent->attachment[0],
			IEngineStudio.Mod_ForName("sprites/ef_cannonex.spr", FALSE),
			10.0,
			15.0,
			TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND
			);
	}
}

void EV_FireRuneblade(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	

	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int lefthand;

	lefthand = cl_righthand->value;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles.y -= 18;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	Vector vecSrc;
	EV_GetGunPosition(args, vecSrc, origin);

	if (args->iparam1 == 0)
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSlash[gEngfuncs.pfnRandomLong(0, WeaponData.iSlash)], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
	}
	else if (args->iparam1 == 1)
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSlashHit[gEngfuncs.pfnRandomLong(0, WeaponData.iSlashHit)], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));
	}
	else if (args->iparam1 == 2)
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szWall[gEngfuncs.pfnRandomLong(0, WeaponData.iWall)], 1.0, ATTN_NORM, 0, 98 + gEngfuncs.pfnRandomLong(0, 3));


		Vector vecEnd = vecSrc + forward * args->fparam1;
		static pmtrace_t tr;
		model_t *model = IEngineStudio.Mod_ForName("sprites/runeblade_ef02.spr", FALSE);

		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(idx - 1);
		gEngfuncs.pEventAPI->EV_SetTraceHull(2);
		gEngfuncs.pEventAPI->EV_PlayerTrace(vecSrc, vecEnd, PM_NORMAL, -1, &tr);

		char chTextureType, *pTextureName, texname[64], szbuffer[64];

		if (tr.fraction < 1.0)
		{
			pTextureName = (char *)gEngfuncs.pEventAPI->EV_TraceTexture(tr.ent, vecSrc, vecEnd);

			if (pTextureName)
			{
				strcpy(texname, pTextureName);
				pTextureName = texname;

				// strip leading '-0' or '+0~' or '{' or '!'
				if (*pTextureName == '-' || *pTextureName == '+')
				{
					pTextureName += 2;
				}

				if (*pTextureName == '{' || *pTextureName == '!' || *pTextureName == '~' || *pTextureName == ' ')
				{
					pTextureName++;
				}

				// '}}'
				strcpy(szbuffer, pTextureName);
				szbuffer[CBTEXTURENAMEMAX - 1] = 0;

				// get texture type
				chTextureType = PM_FindTextureType(szbuffer);
			}

			TEMPENTITY *te = CL_DefaultSprite(tr.endpos, model, 10);
			if (te)
			{
				te->entity.curstate.rendermode = kRenderTransAdd;
				te->entity.curstate.renderamt = 200;
				te->entity.curstate.scale = 0.3;
				te->entity.curstate.framerate = 45;
			}
		}

		gEngfuncs.pEventAPI->EV_PopPMStates();
	}


	if (args->bparam1)
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/laserminigun_exp2.wav", 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		TEMPENTITY *pEnt;
		pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin + forward * 30, IEngineStudio.Mod_ForName("models/ef_runeblade_swing.mdl", false));
		pEnt->entity.curstate.frame = 0;
		pEnt->entity.curstate.framerate = 1;
		pEnt->die = cl.time + 0.5;
	
		CL_Explosion(
			vecSrc + forward * 50,
			IEngineStudio.Mod_ForName("sprites/runeblade_ef.spr", FALSE),
			1.0,
			50.0,
			TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND
			);

		CL_Explosion(
			vecSrc + forward * 50,
			IEngineStudio.Mod_ForName("sprites/runeblade_ef02.spr", FALSE),
			1.0,
			50.0,
			TE_EXPLFLAG_NODLIGHTS | TE_EXPLFLAG_NOPARTICLES | TE_EXPLFLAG_NOSOUND
			);
	}
}

void EV_RunebladeEffect(struct event_args_s *args)
{
	cl_entity_t *pEntity = gEngfuncs.GetEntityByIndex(args->entindex);
	model_t *model = IEngineStudio.Mod_ForName("sprites/runeblade_ef02.spr", FALSE);

	if (model)
	{
		if (pEntity)
		{
			TEMPENTITY *te = CL_DefaultSprite(pEntity->origin, model, 10);
			if (te)
			{
				te->entity.curstate.rendermode = kRenderTransAdd;
				te->entity.curstate.renderamt = 200;
				te->entity.curstate.scale = 0.3;
				te->entity.curstate.framerate = 45;
			}
		}
	}
}

void EV_FireThanatos9(struct event_args_s *args)
{
	int idx = args->entindex;
	vec3_t origin = args->origin, angles = args->angles, forward, up, vecSrc;

	AngleVectors(angles, forward, NULL, up);

	EV_GetGunPosition(args, vecSrc, origin);
	vec3_t vecEnd = vecSrc + forward * args->fparam1;

	if (EV_IsLocal(idx))
	{
		HudStatistics().m_iShoots++;
	}

	switch (args->iparam2)
	{
		case 0:
		{
			if (args->iparam1 == 0)
			{
				if (RANDOM_LONG(0, 1))
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/thanatos9_shoota1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
				else
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/thanatos9_shoota2.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else if (args->iparam1 == 1)
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/mastercombat_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else if (args->iparam1 == 2)
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/nata_wall.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			break;
		}
		case 1:
		{
			if (args->iparam1 == 0)
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/thanatos9_shootb_loop.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else if (args->iparam1 == 1)
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/mastercombat_hit1.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			else if (args->iparam1 == 2)
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/tomahawk_wall.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			}
			if (EV_IsLocal(idx))
			{
				struct model_s *pModel;
				TEMPENTITY *pEnt;

				pModel = IEngineStudio.Mod_ForName("sprites/smoke_thanatos9.spr", false);

				pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(GetViewEntity()->attachment[RANDOM_LONG(0, 3)], pModel);
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 15.0;
				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 150;
				pEnt->entity.curstate.scale = RANDOM_FLOAT(0.7, 1.0);
				pEnt->entity.baseline.origin = up * 100;

				pEnt->die = cl.time + 0.57;
				pEnt->callback = SmokeRise_Wind; /*bWind ? SmokeRise_Wind : SmokeRise;*/
				pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_PERSIST | FTENT_SPRANIMATELOOP;

				//pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_SPRANIMATE;
				//pEnt->callback = EV_SPR;
			}
			break;
		}
	}

	
}

void EV_FireJanusmk5(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 10000000.0;
	angles[1] += args->iparam2 / 10000000.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/rshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (args->bparam2) // Charge
			gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(8, 10), 2);
		else if (args->bparam1) // Signal
			gEngfuncs.pEventAPI->EV_WeaponAnimation(4, 2);
		else 
			gEngfuncs.pEventAPI->EV_WeaponAnimation(3, 2);

		if (Hud().m_bCreateSmoke)
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.26, 15, 15, 15, 4, velocity, false, 35);
	}

	if (!args->bparam2)
	{ 
		if (EV_IsLocal(idx))
		{
			float vecScale[3] = { 20.0, -8.0, -10.0 };

			if (WeaponData.bHaveShellVec)
			{
				EV_GetDefaultShellInfo(velocity, ShellVelocity, forward, right, up, WeaponData.fShellVec, lefthand == 0);
			}
			else
			{
				if (lefthand == 0)
					vecScale[2] = -vecScale[2];

				EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);
				VectorScale(ShellVelocity, 1.25, ShellVelocity);
				ShellVelocity[2] -= 122;
			}

			VectorCopy(ent->attachment[1], ShellOrigin);
		}
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 9);
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

		EV_GetGunPosition(args, vecSrc, origin);

		VectorCopy(forward, vecAiming);

		vSpread[0] = args->fparam1;
		vSpread[1] = args->fparam2;
		vSpread[2] = 0;

		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_556MM, 0, 0, 2);
	}
	else
	{
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound2, 1.0, 0.4, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

		EV_GetGunPosition(args, vecSrc, origin);

		VectorCopy(forward, vecAiming);

		vSpread[0] = 0.0715;
		vSpread[1] = 0.0715;
		vSpread[2] = 0;

		EV_HLDM_FireBullets(idx, forward, right, up, 3, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_BUCKSHOT, 0, 0, 1);
	}
	

	
}

extern float g_flBloodhunterAnimTime;
extern int g_iBloodhunterSecAnim;

void EV_BloodhunterEffectCallback(struct tempent_s *ent, float frametime, float currenttime)
{
	Vector forward, right, up;
	AngleVectors(gEngfuncs.GetLocalPlayer()->angles, forward, right, up);
	
	static cvar_t *m_pCvarRightHand = gEngfuncs.pfnGetCvarPointer("cl_righthand");
	if (1/*m_pCvarRightHand->value*/)
		ent->tentOffset = ent->entity.curstate.vuser1 + forward * 25 + right * 5 - up * 1;
	else
		ent->tentOffset = ent->entity.curstate.vuser1 + forward * 25 - right * 5 - up * 15;
}

void EV_CreateBloodhunterEffect()
{
	struct model_s *pModel;
	if (RANDOM_LONG(0, 1))
	{
		pModel = IEngineStudio.Mod_ForName("sprites/ef_bloodhunter1.spr", false);
	}
	else
	{
		pModel = IEngineStudio.Mod_ForName("sprites/ef_bloodhunter2.spr", false);
	}
	//pModel = IEngineStudio.Mod_ForName("sprites/flame_puff01.spr", false);
	cvar_t *m_pCvarRightHand = gEngfuncs.pfnGetCvarPointer("cl_righthand");

	TEMPENTITY *pEnt;

	pEnt = CL_DefaultSprite(g_vecEye, pModel, 15);
	pEnt->entity.curstate.rendermode = kRenderTransAdd;
	pEnt->entity.curstate.renderamt = 250;
	pEnt->entity.curstate.scale = 0.125;
	pEnt->entity.curstate.framerate = 30.0;

	//if (!m_pCvarRightHand->value)
	//	pEnt->entity.angles[2] -= 180.0f;

	pEnt->clientIndex = gEngfuncs.GetLocalPlayer()->index;

	pEnt->entity.curstate.vuser1 = g_vecEye - gEngfuncs.GetLocalPlayer()->origin;

	//pEnt->die = cl.time + 1.5;
	pEnt->callback = EV_BloodhunterEffectCallback;
	pEnt->flags |= FTENT_PLYRATTACHMENT | FTENT_CLIENTCUSTOM | FTENT_SPRANIMATE;
}

void EV_FireBloodHunter(struct event_args_s *args)
{
	int idx;
	vec3_t origin, angles, velocity, forward, right, up;
	vec3_t vecSrc, vecDirShooting, vecSpread, ShellOrigin, ShellVelocity;
	int shell;
	int lefthand;
	cl_entity_t *ent;

	ent = GetViewEntity();

	if (cl_righthand->value)
		lefthand = 0;
	else
		lefthand = 1;

	origin = args->origin;
	velocity = args->velocity;
	angles = args->angles;

	EV_GetGunPosition(args, vecSrc, origin);

	idx = args->entindex;

	AngleVectors(angles, forward, right, up);

	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;
		EV_MuzzleFlash();
		if (args->bparam2)
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(2, 2);
			g_flBloodhunterAnimTime = cl.time;
			g_iBloodhunterSecAnim = 23 + (g_iWeaponStat-50) - 1;
		}
		else if (g_iBloodhunterSecAnim && cl.time - g_flBloodhunterAnimTime < 1.32)
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(1, 2);
		}
		else
		{
			gEngfuncs.pEventAPI->EV_WeaponAnimation(3 + (g_iWeaponStat - 50), 2);
			g_flBloodhunterAnimTime = 0.0;
			g_iBloodhunterSecAnim = 0;
		}
		/*
		// Moved to EV_Knife => HudAmmo().HitForBuff
		if (g_iWeaponStat - 50 < 3)
		{
			EV_CreateBloodhunterEffect();
		}*/
		

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, 0, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, 0, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, 0, 35);
		}

		if (lefthand)
			EV_GetDefaultShellInfo(args,
			origin,
			velocity,
			ShellVelocity,
			ShellOrigin,
			forward,
			right,
			up,
			35.0,
			-11.0,
			-16.0,
			false
			);
		else
			EV_GetDefaultShellInfo(args,
			origin,
			velocity,
			ShellVelocity,
			ShellOrigin,
			forward,
			right,
			up,
			35.0,
			-11.0,
			16.0,
			false
			);

		ShellVelocity *= 0.75;
		ShellOrigin = ent->attachment[1];

	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/bloodhunter-1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));

	vecDirShooting = forward;

	vecSpread.x = args->fparam1;
	vecSpread.y = args->fparam2;
	vecSpread.z = 0.0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecDirShooting, vecSpread, 8192.0, BULLET_PLAYER_45ACP, 0, NULL, 2);
}

void EV_FireInfinity(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	float bullets_left;
	float time_diff;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	bullets_left = args->iparam2;
	time_diff = args->fparam1;

	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();

		if (!args->bparam1)
		{
			if (args->iparam1 == 1)
			{
				if (bullets_left <= 1)
					gEngfuncs.pEventAPI->EV_WeaponAnimation(10, 2); // LEFTLAST_SP
				else
					gEngfuncs.pEventAPI->EV_WeaponAnimation(6, 2); // LEFT_SP
			}
			else if (args->iparam1 == 2)
			{
				if (bullets_left <= 1)
					gEngfuncs.pEventAPI->EV_WeaponAnimation(10, 2); // LEFTLAST_SP
				else
					gEngfuncs.pEventAPI->EV_WeaponAnimation(7, 2); // LEFT_SP2
			}
			else
			{
				if (bullets_left <= 1)
					gEngfuncs.pEventAPI->EV_WeaponAnimation(3, 2); // LEFTLAST
				else
					gEngfuncs.pEventAPI->EV_WeaponAnimation(2, 2); // LEFT
			}
			
		}
		else
		{
			if (args->iparam1 == 1)
			{
				if (bullets_left <= 1)
					gEngfuncs.pEventAPI->EV_WeaponAnimation(10, 2); // RIGHTLAST_SP
				else
					gEngfuncs.pEventAPI->EV_WeaponAnimation(8, 2); // RIGHT_SP
			}
			else if (args->iparam1 == 2)
			{
				if (bullets_left <= 1)
					gEngfuncs.pEventAPI->EV_WeaponAnimation(10, 2); // RIGHTLAST_SP
				else
					gEngfuncs.pEventAPI->EV_WeaponAnimation(9, 2); // RIGHT_SP2
			}
			else
			{
				if (bullets_left <= 1)
					gEngfuncs.pEventAPI->EV_WeaponAnimation(5, 2); // RIGHTLAST
				else
					gEngfuncs.pEventAPI->EV_WeaponAnimation(4, 2); // RIGHT
			}
		}

		if (Hud().m_bCreateSmoke)
		{
			if (!args->bparam1)
			{
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
			}
			else
			{
				EV_HLDM_CreateSmoke(ent->attachment[1], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[1], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[1], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
			}
		}
	}

	if (EV_IsLocal(idx))
	{
		if (!args->bparam1)
		{
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);
			VectorCopy(ent->attachment[2], ShellOrigin);
		}
		else
		{
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0, false);
			VectorCopy(ent->attachment[3], ShellOrigin);
		}

		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] -= 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, 4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->iparam1?WeaponData.szSound2:WeaponData.szSound, 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, !args->iparam1 ? 0 : 1, !args->iparam1, WeaponData.flSrcOfs);
}

void EV_FireBlockSMG(struct event_args_s *args)
{
	vec3_t origin, angles, velocity;
	vec3_t forward, right, up;
	vec3_t ShellOrigin, ShellVelocity;
	vec3_t vecDirShooting;
	vec3_t vecSrc, vecSpread;
	origin = args->origin;
	velocity = args->velocity;
	angles = args->angles;

	cl_entity_t *ent = GetViewEntity();
	int idx = args->entindex;
	int iShell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/block_shell.mdl");

	angles.x += args->iparam1 / 100.0;
	angles.y += args->iparam2 / 100.0;

	AngleVectors(angles, forward, right, up);

	if (args->bparam1)
	{
		if (EV_IsLocal(idx))
			gEngfuncs.pEfxAPI->R_BreakModel(ent->attachment[0] + forward * 35, Vector(10.0, 10.0, 10.0), Vector(0, 0, 0), 100, 3, 10, iShell, BREAK_CONCRETE);
	}
	else
	{
		if (EV_IsLocal(idx))
		{
			g_iShotsFired++;
			EV_MuzzleFlash();
			if (args->fparam2 > 1.0f)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(!args->bparam2 ? 2:4, 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(!args->bparam2 ? 3 : 5, 2);

			if (Hud().m_bCreateSmoke)
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.35, 20, 20, 20, 4, velocity, false, 35);

			if (gEngfuncs.pfnGetCvarFloat("cl_righthand") < 1.0)
				EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				20.0,
				-8.0,
				-10.0,
				false);
			else
				EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				20.0,
				-8.0,
				10.0,
				false);

			ShellOrigin = ent->attachment[1];

			if (!args->bparam2)
			{
				EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, 16.0, false);
				VectorCopy(ent->attachment[2], ShellOrigin);
			}
			else
			{
				EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 35.0, -11.0, -16.0, false);
				VectorCopy(ent->attachment[3], ShellOrigin);
			}

			ShellVelocity.z -= 75;
		}
		else
		{
			EV_GetDefaultShellInfo(
				args,
				origin,
				velocity,
				ShellVelocity,
				ShellOrigin,
				forward,
				right,
				up,
				20.0,
				-12.0,
				4.0,
				false);
		}

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], iShell, TE_BOUNCE_SHELL, idx, 9);
		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/blocksmg1-1.wav", VOL_NORM, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));

		EV_GetGunPosition(args, vecSrc, origin);
		vecSpread = Vector(0, 0, 0);
		vecDirShooting = forward;
		
		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecDirShooting, vecSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2, !args->bparam2 ? 0 : 1, !args->bparam2, WeaponManager().GetPlayerWeapon(idx, 0).flSrcOfs);
	}
}

void EV_FireDesperado(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;
	

	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	lefthand = !args->bparam2 ^ (int)(cl_righthand->value);
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 2);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (WeaponData.bHaveShellModel)
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
	else
		shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_CustomEffect(args, WeaponData.iEffect);
		EV_MuzzleFlash();


		if (empty)
		{
				gEngfuncs.pEventAPI->EV_WeaponAnimation(WeaponData.iAnimEmpty, 2);
		}
		else
		{
			if (!args->bparam2)
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart, WeaponData.iAnimEnd), 2);
			else
				gEngfuncs.pEventAPI->EV_WeaponAnimation(gEngfuncs.pfnRandomLong(WeaponData.iAnimStart2, WeaponData.iAnimEnd2), 2);
		}

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
		}

	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = { 35.0, -11.0, -16.0 };

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[1], ShellOrigin);
		VectorScale(ShellVelocity, 0.75, ShellVelocity);
		ShellVelocity[2] += 25;
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam1 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = args->fparam1;
	vSpread[1] = args->fparam2;
	vSpread[2] = 0;

	EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2);
}

void EV_FireDualSword(struct event_args_s *args)
{
	int idx = args->entindex;
	vec3_t origin = args->origin, angles = args->angles, forward, up, vecSrc;

	AngleVectors(angles, forward, NULL, up);

	EV_GetGunPosition(args, vecSrc, origin);
	vec3_t vecEnd = vecSrc + forward * args->fparam1;

	if (EV_IsLocal(idx))
	{
		HudStatistics().m_iShoots++;
	}

	int iParm = args->iparam2;

	switch (iParm)
	{
		case 1:
		{
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, va("weapons/dualsword_slash_%d.wav", args->iparam1 + 1), 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			if (args->bparam1 || args->bparam2)
			{
				gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, va("weapons/dualsword_hit%d.wav", RANDOM_LONG(1, 3)), 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));



			}
			
			
			if (EV_IsLocal(idx))
			{
				auto viewent = gEngfuncs.GetViewModel();
				model_s *pModel = IEngineStudio.Mod_ForName("models/dualswordfx.mdl", false);
				TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);

				pEnt->entity.curstate.body = g_iViewEntityBody;
				pEnt->entity.curstate.frame = 0.0f;
				pEnt->entity.curstate.framerate = 1;
				pEnt->entity.curstate.animtime = cl.time;
				pEnt->entity.curstate.sequence = args->iparam1;
				pEnt->tentOffset = Vector(0, 0, 0);
				pEnt->bounceFactor = 3;	// ??
				pEnt->fadeSpeed = 512;
				pEnt->frameMax = 31;

				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 255;
				pEnt->entity.baseline.angles = Vector(0, 0, 0);
				
				if (args->iparam1 != 3)
				{
					pEnt->die = cl.time + 1.0;
					pEnt->entity.curstate.fuser1 = cl.time;
					pEnt->callback = [](tempent_s *pEnt, float frametime, float currenttime){
						cl_entity_t *viewent = gEngfuncs.GetViewModel();
						pEnt->entity.origin = viewent->origin;
						pEnt->entity.angles = viewent->angles;
						if (currenttime > pEnt->entity.curstate.fuser1)
							pEnt->entity.curstate.renderamt = 255 - (currenttime - pEnt->entity.curstate.fuser1) * 255.0f;
					};
				}
				else
				{
					pEnt->die = cl.time + 1.5;
					pEnt->entity.curstate.fuser1 = cl.time;
					pEnt->callback = [](tempent_s *pEnt, float frametime, float currenttime){
						cl_entity_t *viewent = gEngfuncs.GetViewModel();
						pEnt->entity.origin = viewent->origin;
						pEnt->entity.angles = viewent->angles;
						if (currenttime > pEnt->entity.curstate.fuser1 + 1.0)
							pEnt->entity.curstate.renderamt = 255 - (currenttime - pEnt->entity.curstate.fuser1 - 1.0) * 2.0f * 255.0f;
					};
				}
				

				pEnt->clientIndex = idx;
				pEnt->flags &= ~(FTENT_COLLIDEWORLD | FTENT_GRAVITY);
				pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_FADEOUT | FTENT_PERSIST;

				
			}
			break;
		}
		case 2: // STAB_END
		{
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, va("weapons/dualsword_slash_4_%d.wav", args->iparam1), 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));
			break;
		}
		case 3: // SKILL_START
		{
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/dualsword_skill_start.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));

			model_s *pModel = IEngineStudio.Mod_ForName("models/dualsword_skill.mdl", false);
			TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->entity.curstate.frame = 0;
			pEnt->entity.curstate.framerate = 1;
			pEnt->entity.curstate.animtime = cl.time;
			pEnt->entity.curstate.sequence = 0;
			pEnt->tentOffset = Vector(0, 0, 0);
			pEnt->die = cl.time + 1.53;


			pEnt->clientIndex = idx;
			pEnt->flags |= FTENT_PLYRATTACHMENT;

			break;
		}
		case 4: // SKILL
		{
			
			break;
		}
		case 5: // SKILL_END
		{
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, "weapons/dualsword_skill_end.wav", 1.0, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong(0, 15));

			model_s *pModel = IEngineStudio.Mod_ForName("models/dualsword_skill.mdl", false);
			TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
			pEnt->entity.curstate.frame = 0;
			pEnt->entity.curstate.framerate = 1;
			pEnt->entity.curstate.animtime = cl.time;
			pEnt->entity.curstate.sequence = 1;
			pEnt->tentOffset = Vector(0, 0, 0);
			pEnt->die = cl.time + 1.53;

			pEnt->clientIndex = idx;
			pEnt->flags |= FTENT_PLYRATTACHMENT;

			
			break;
		}
	}


}

void EV_FireJanus11(struct event_args_s *args)
{
	if (!args->bparam2)
	{
		return EV_FireM3(args);
	}
	
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	int lefthand;
	vec3_t vSpread;


	float flDistance = 4096.0;

	ent = GetViewEntity();
	idx = args->entindex;
	lefthand = cl_righthand->value;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		EV_MuzzleFlash();
		
		gEngfuncs.pEventAPI->EV_WeaponAnimation(9, 2);
	}

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSoundSilen2, 1.0, 0.48, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

	EV_GetGunPosition(args, vecSrc, origin);

	VectorCopy(forward, vecAiming);

	vSpread[0] = vSpread[1] = vSpread[2] = 0;

	int shots;

	vSpread[0] = vSpread[1] = 0.07;
	shots = 8;
	flDistance = 8192.0;
	EV_HLDM_FireBullets(idx, forward, right, up, shots, vecSrc, forward, vSpread, flDistance, BULLET_PLAYER_JANUS11, 0, 0, 1);
}

void EV_FireGunkata(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	int empty;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	cl_entity_t *ent;
	vec3_t vSpread;


	ent = GetViewEntity();
	idx = args->entindex;
	empty = args->bparam1 != false;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 2);

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	angles[0] += args->iparam1 / 100.0;
	angles[1] += args->iparam2 / 100.0;

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	if (!args->bparam2)
	{
		if (WeaponData.bHaveShellModel)
			shell = gEngfuncs.pEventAPI->EV_FindModelIndex(WeaponData.szShell);
		else
			shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/pshell.mdl");

		if (EV_IsLocal(idx))
		{
			g_iShotsFired++;

			EV_CustomEffect(args, WeaponData.iEffect);
			EV_MuzzleFlash();


			//gEngfuncs.pEventAPI->EV_WeaponAnimation(args->bparam1, 2);

			if (Hud().m_bCreateSmoke)
			{
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 0, 0.25, 10, 10, 10, 3, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 25, 0.3, 15, 15, 15, 2, velocity, false, 35);
				EV_HLDM_CreateSmoke(ent->attachment[0], forward, 50, 0.2, 25, 25, 25, 2, velocity, false, 35);
			}

		}

		if (EV_IsLocal(idx))
		{
			float vecScale[3] = { 35.0, -11.0, -16.0 };

			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

			VectorCopy(ent->attachment[1], ShellOrigin);
			VectorScale(ShellVelocity, 0.75, ShellVelocity);
			ShellVelocity[2] += 25;
		}
		else
			EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHELL, idx, 5);

		gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));

		EV_GetGunPosition(args, vecSrc, origin);

		VectorCopy(forward, vecAiming);

		vSpread[0] = args->fparam1;
		vSpread[1] = args->fparam2;
		vSpread[2] = 0;

		EV_HLDM_FireBullets(idx, forward, right, up, 1, vecSrc, vecAiming, vSpread, 8192, BULLET_PLAYER_50AE, 0, 0, 2);
	}
	else
	{
		if (args->iparam1 && args->iparam1<=15 && args->fparam2 == 0.0f)
		{
			if (EV_IsLocal(idx))
			{
				//g_flBloodhunterAnimTime = cl.time;
				//g_iBloodhunterSecAnim = args->iparam1;
				//gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam1 + 1, 2);
				float time = cl.time;
				R_AddExtraViewModel({ time, time + args->fparam1, args->iparam1, kRenderNormal, 255 });
				//R_AddExtraViewModel({ cl.time + 0.2f, cl.time + args->fparam1 + 0.2f, args->iparam1, kRenderNormal, 255 });

				//gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, WeaponData.szSound, 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
			}

			struct model_s *pModel = IEngineStudio.Mod_ForName("models/ef_gunkata.mdl", false);
			if(pModel)
			{ 
				TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 1;
				pEnt->tentOffset = Vector(0, 0, 0);
				pEnt->die = cl.time + 0.9;
				pEnt->clientIndex = idx;
				pEnt->flags |= FTENT_PLYRATTACHMENT;
			}

		}
		else if(args->fparam1 == 1.0f && args->fparam2 > 0.2f && args->fparam2 < 0.23f) // end
		{
			struct model_s *pModel = IEngineStudio.Mod_ForName("models/ef_scorpion_hole.mdl", false);
			if (pModel)
			{
				TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
				pEnt->entity.curstate.sequence = 1;
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 1.0;
				pEnt->tentOffset = Vector(0, 0, -32);
				pEnt->die = cl.time + 0.9;
				pEnt->clientIndex = idx;
				pEnt->flags |= FTENT_PLYRATTACHMENT;
			}
			gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_AUTO, "weapons/gunkata_skill_last_exp.wav", 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
		}
		else if (args->fparam1 == 10.0f && args->fparam2 == 2.0f) // playermodel
		{
			struct model_s *pModel = IEngineStudio.Mod_ForName("models/ef_gunkata_man.mdl", false);
			if (PlayerClassManager().GetPlayerClass(idx).sex == CPlayerClassManager::SEX_FEMALE)
			{
				pModel = IEngineStudio.Mod_ForName("models/ef_gunkata_woman.mdl", false);
			}
			
			if (pModel)
			{
				TEMPENTITY *pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);
				pEnt->entity.curstate.sequence = args->iparam1;
				pEnt->entity.curstate.frame = 0;
				pEnt->entity.curstate.framerate = 1.0;
				pEnt->entity.angles = angles;

				pEnt->die = cl.time + args->fparam2;
				pEnt->entity.curstate.fuser1 = cl.time;
				pEnt->clientIndex = idx;
				pEnt->flags |= FTENT_FADEOUT | FTENT_CLIENTCUSTOM;

				pEnt->entity.curstate.rendermode = kRenderTransTexture;
				pEnt->entity.curstate.renderamt = 100;

				pEnt->callback = [](tempent_s *pEnt, float frametime, float currenttime) {
					float delta = currenttime - pEnt->entity.curstate.fuser1;
					if (delta > 1.0)
						pEnt->entity.curstate.renderamt = 100 - (delta - 1.0) * 100.0f;
				};

				int i = args->iparam1 % 6;
				if(i)
					gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_AUTO, va("weapons/gunkata_skill_0%d.wav", i), 1.0, 0.6, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
			}
		}
	}

	
}

void EV_FireSgmissile(struct event_args_s *args)
{
	int idx;
	vec3_t origin;
	vec3_t up, right, forward;
	cl_entity_t *ent;

	ent = GetViewEntity();
	idx = args->entindex;
	auto &WeaponData = WeaponManager().GetPlayerWeapon(idx, 0);

	vec3_t angles;
	vec3_t velocity;
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	int shell;
	int lefthand;
	int iSpecialAmmo = args->iparam2;

	lefthand = cl_righthand->value;

	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	VectorCopy(args->velocity, velocity);

	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	shell = gEngfuncs.pEventAPI->EV_FindModelIndex("models/shell_sgdrill.mdl");

	if (EV_IsLocal(idx))
	{
		g_iShotsFired++;

		if (Hud().m_bCreateSmoke)
		{
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 3, 0.45, 25, 25, 25, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 40, 0.35, 20, 20, 20, 3, velocity, false, 35);
			EV_HLDM_CreateSmoke(ent->attachment[0], forward, 80, 0.5, 15, 15, 15, 3, velocity, false, 35);
		}
	}

	if (EV_IsLocal(idx))
	{
		float vecScale[3] = { 22.0, -9.0, -11.0 };

		if (lefthand == 0)
			vecScale[2] = 0 - vecScale[2];

		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, vecScale[0], vecScale[1], vecScale[2], true);

		VectorCopy(ent->attachment[2], ShellOrigin);
		//VectorScale(ShellVelocity, 1.25, ShellVelocity);
		ShellVelocity[2] -= 50;

		if (args->bparam2)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(iSpecialAmmo ? WeaponData.iAnimStart2 : WeaponData.iAnimEnd2, 2);
		else if(iSpecialAmmo)
			gEngfuncs.pEventAPI->EV_WeaponAnimation(iSpecialAmmo ? WeaponData.iAnimStart : WeaponData.iAnimEnd, 2);
		else
			gEngfuncs.pEventAPI->EV_WeaponAnimation(WeaponData.iAnimEmpty, 2);
	}
	else
		EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20.0, -12.0, -4.0, false);

	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[YAW], shell, TE_BOUNCE_SHOTSHELL, idx, 3);

	vec3_t sub;
	float distance = 0;

	if (EV_IsLocal(idx))
	{
		VectorSubtract(ent->attachment[0], origin, sub);
		distance = VectorLength(sub);
	}
	args->fparam2 = 25;

	if (args->bparam2)
	{
		float v_angle1 = v_angles[1];

		for (int j = 2; j <= 8; j++)
		{

			//v_angles[1] = v_angle1;
			//v_angles[1] -= args->fparam2 / 2.0f;

			for (int i = -4; i <= 4; i++)
			{
				//v_angles[1] += args->fparam2 / 5.0f;

				v_angles[1] = v_angle1 + 6.0 * i;

				gEngfuncs.pfnAngleVectors(v_angles, forward, right, up);

				struct model_s *pModel;
				TEMPENTITY *pEnt;

				pModel = IEngineStudio.Mod_ForName("sprites/ef_sgmissile_line.spr", 0);

				if (EV_IsLocal(idx))
					pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(ent->attachment[0], pModel);
				else
					pEnt = gEngfuncs.pEfxAPI->CL_TempEntAllocHigh(origin, pModel);

				pEnt->entity.curstate.rendermode = kRenderTransAdd;
				pEnt->entity.curstate.renderamt = 5 + j * 3;
				pEnt->entity.curstate.rendercolor.b = 200;
				pEnt->entity.curstate.rendercolor.r = pEnt->entity.curstate.rendercolor.g = 255;
				pEnt->entity.curstate.scale = 0.1 - j * 0.005;
				pEnt->entity.curstate.fuser2 = 0.1 - j * 0.012;
				pEnt->entity.curstate.fuser4 = (/*args->fparam1*/600 - distance) * (j + 1) / 8;
				pEnt->entity.curstate.vuser3[0] = 0.2 - j * 0.24;
				pEnt->entity.curstate.vuser4[0] = 0.005;
				pEnt->entity.curstate.vuser4[1] = 0.05;
				pEnt->entity.angles[2] = gEngfuncs.pfnRandomLong(0, 1) ? 359.9 - gEngfuncs.pfnRandomLong(0, 15) : gEngfuncs.pfnRandomLong(0, 15);
				pEnt->flags |= FTENT_CLIENTCUSTOM | FTENT_COLLIDEALL | FTENT_SPRANIMATE;
				pEnt->frameMax = ModelFrameCount(pModel) - 1;
				pEnt->entity.curstate.vuser4[2] = pEnt->frameMax / 0.55 * 3.0;
				pEnt->entity.curstate.framerate = pEnt->frameMax / 0.55;
				pEnt->entity.curstate.fuser3 = cl.time;

				pEnt->die = cl.time + 60.0f;

				VectorCopy(pEnt->entity.origin, pEnt->entity.curstate.vuser1);


				VectorCopy(forward, pEnt->entity.curstate.velocity);

				pEnt->callback = EV_FlameCallback;
				pEnt->hitcallback = EV_FlameHitCallback;
			}
		}
	}
	
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, args->bparam2 ? WeaponData.szSound2 : WeaponData.szSound, 1.0, 0.52, 0, 94 + gEngfuncs.pfnRandomLong(0, 0xf));
}