#include <winsani_in.h>
#include <winsani_out.h>
#include <stdio.h>
#include <malloc.h>
#include <interface.h>
#include <metahook.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

//typedef float vec_t;
//typedef float vec2_t[2];
//typedef float vec3_t[3];

#include <math.h>
#include <SourceSDK\mathlib\vector2d.h>
#include <SourceSDK\mathlib\vector.h>
#include <wrect.h>

#define vec3_t Vector

//typedef int (*pfnUserMsgHook)(const char *pszName, int iSize, void *pbuf);

#include <SourceSDK\tier1\strtools.h>

#include "plugins.h"

#include <gl/gl.h>
#include <const.h>
#include "pm_defs.h"
#include "HLSDK\engine\cdll_int.h"
#include <triangleapi.h>
#include <cl_entity.h>
#include <event_api.h>
#include <ref_params.h>
#include <com_model.h>
#include <cvardef.h>
#include <studio.h>
#include <r_studioint.h>
#include <pm_movevars.h>

#include "cdll_dll.h"

#define HIDEWEAPON_WEAPON		(1<<0)
#define HIDEWEAPON_FLASHLIGHT	(1<<1)
#define HIDEWEAPON_ALL			(1<<2)
#define HIDEWEAPON_HEALTH		(1<<3)
#define HIDEWEAPON_TIME			(1<<4)
#define HIDEWEAPON_MONEY		(1<<5)
#define HIDEWEAPON_CROSSHAIR	(1<<6)

#define MOD_NONE	1
#define MOD_TDM		2
#define MOD_ZB		3
#define MOD_ZBS		4
#define MOD_ZBU		5
#define MOD_HMS		6
#define MOD_GHOST	7
#define MOD_GD		8
#define MOD_DR		9
#define MOD_ZE		10
#define MOD_ZB3		11
#define MOD_ZB4		12
#define MOD_ZB2		13
#define MOD_DM		14
#define MOD_ZSE		15
#define MOD_ZB5		16

#define IS_ZOMBIE_MODE (g_iMod == MOD_ZB || g_iMod == MOD_ZBU || g_iMod == MOD_ZE || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB2 || g_iMod == MOD_ZSE || g_iMod == MOD_ZB5)

#define SCOREATTRIB_NONE    0
#define SCOREATTRIB_HERO	(1<<3)