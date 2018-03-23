#include "metahook.h"
#include "exportfuncs.h"
#include "util.h"
#include "eventscripts.h"
#include "event_args.h"
#include "event_api.h"
#include "cl_entity.h"
#include "mathlib/Vector.h"

void(*g_pfnEV_EjectBrass)(float *origin, float *velocity, float rotation, int model, int soundtype, int idx, int angle_velocity) = (void(*)(float *, float *, float, int, int, int, int))0x1945180;
void(*g_pfnEV_GetDefaultShellInfo)(event_args_t *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin, float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale, bool bReverseDirection) = (void(*)(event_args_t *, float *, float *, float *, float *, float *, float *, float *, float, float, float, bool))0x1945260;

void EV_EjectBrass(float *origin, float *velocity, float rotation, int model, int soundtype, int idx, int angle_velocity)
{
	(*g_pfnEV_EjectBrass)(origin, velocity, rotation, model, soundtype, idx, angle_velocity);
}

void EV_GetDefaultShellInfo(event_args_t *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin, float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale, bool bReverseDirection)
{
	g_pfnEV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, forwardScale, upScale, rightScale, bReverseDirection);
}

void EV_GetDefaultShellInfo(float *velocity, float *ShellVelocity, float *forward, float *right, float *up, float *f, bool bReverseDirection)
{
	float fR, fU;

	fR = gEngfuncs.pfnRandomFloat(f[0], f[1]);
	fU = gEngfuncs.pfnRandomFloat(f[2], f[3]);

	VectorScale(velocity, 0.65, velocity);

	if (bReverseDirection)
		VectorScale(velocity, -1.0, velocity);

	for (int i = 0; i < 3; i++)
		ShellVelocity[i] = velocity[i] + (forward[i] * 50 + right[i] * fR + up[i] * fU);
}

void EV_MuzzleFlash(void)
{
	cl_entity_t *ent = gEngfuncs.GetViewModel();

	if (!ent)
		return;

	ent->curstate.effects |= EF_MUZZLEFLASH;
}
qboolean EV_IsPlayer(int idx)
{
	if (idx >= 1 && idx <= gEngfuncs.GetMaxClients())
		return true;

	return false;
}
qboolean EV_IsLocal(int idx)
{
	if (IS_FIRSTPERSON_SPEC)
		return (g_iUser2 == idx);
	else
		return gEngfuncs.pEventAPI->EV_IsLocal(idx - 1) ? true : false;
}
void EV_GetGunPosition(event_args_t *args, float *pos, float *origin)
{
	int idx;
	vec3_t view_ofs;

	idx = args->entindex;

	VectorClear(view_ofs);
	view_ofs.z = 28;

	if (EV_IsPlayer(idx))
	{
		if (EV_IsLocal(idx) && !IS_FIRSTPERSON_SPEC)
			gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(view_ofs);
		else if (args->ducking == 1)
			view_ofs.z = 12;
	}

	VectorAdd(origin, view_ofs, pos);
}
Vector EV_GetPosition(event_args_t *args, float vForward, float vRight, float vUp)
{
	vec3_t vecOrigin, origin, angles, forward, right, up;
	VectorCopy(args->origin, origin);
	VectorCopy(args->angles, angles);
	AngleVectors(angles, forward, right, up);
	EV_GetGunPosition(args, vecOrigin, origin);
	return vecOrigin + vForward*forward + vRight*right + vUp*up;
}