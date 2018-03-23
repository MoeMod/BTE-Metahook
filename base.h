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

#include <bte_const.h>