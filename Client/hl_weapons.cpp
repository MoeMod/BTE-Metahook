#include <metahook.h>
#include "client.h"
#include "exportfuncs.h"
#include <usercmd.h>
#include <entity_state.h>
#include "cdll_dll.h"
#include "weapons.h"
#include <cvardef.h>
#include "com_weapons.h"

int g_rseq;
int g_gaitseq;
vec3_t g_clorg;
vec3_t g_clang;

int g_irunninggausspred;
float g_flApplyVel;

void CounterStrike_GetSequence(int *seq, int *gaitseq)
{
	*seq = g_rseq;
	*gaitseq = g_gaitseq;
}

void CounterStrike_SetSequence(int seq, int gaitseq)
{
	g_rseq = seq;
	g_gaitseq = gaitseq;
}

void CounterStrike_SetOrientation(float *o, float *a)
{
	VectorCopy(o, g_clorg);
	VectorCopy(a, g_clang);
}

void CounterStrike_GetOrientation(float *o, float *a)
{
	VectorCopy(g_clorg, o);
	VectorCopy(g_clang, a);
}

void HUD_WeaponsPostThink(local_state_s *from, local_state_s *to, usercmd_t *cmd, double time, unsigned int random_seed);

void HUD_PostRunCmd(struct local_state_s *from, struct local_state_s *to, struct usercmd_s *cmd, int runfuncs, double time, unsigned int random_seed)
{
	g_runfuncs = runfuncs;

	if (cl_lw && cl_lw->value)
	{
		HUD_WeaponsPostThink(from, to, cmd, time, random_seed);
	}
	else
	{
		to->client.fov = g_lastFOV;
	}

	//gEngfuncs.Con_Printf("%d\n", to->weapondata[g_iCurrentWeapon].m_fInReload);
	g_bInReload = to->weapondata[g_iCurrentWeapon].m_fInReload;
	//g_iWeaponStat = to->weapondata[g_iCurrentWeapon].m_iWeaponState;
	//g_iCurrentWeapon = from->client.m_iId;
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay != 1 && g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay != 5 && g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay != 6)
		g_iShowCustomCrosshair = 0;

	static cl_entity_t *viewmodel;
	viewmodel = gEngfuncs.GetViewModel();

	if (cmd)
	{
		if (cmd->buttons & IN_ATTACK2)
		{
			g_iButton = 1;
			if (g_iBTEWeapon == WPN_GAUSS)
				g_irunninggausspred = true;
		}
		else g_iButton = 0;

		cmd->buttons &= ~IN_RELOAD;
	}

	if (runfuncs)
	{
		CounterStrike_SetSequence(to->playerstate.sequence, to->playerstate.gaitsequence);
		CounterStrike_SetOrientation(to->playerstate.origin, cmd->viewangles);
	}

	gExportfuncs.HUD_PostRunCmd(from, to, cmd, runfuncs, time, random_seed);

	if (g_irunninggausspred == 1)
	{
		Vector forward;
		gEngfuncs.pfnAngleVectors(v_angles, forward, NULL, NULL);
		to->client.velocity = to->client.velocity - forward * g_flApplyVel * 5;
		g_irunninggausspred = false;
	}
}