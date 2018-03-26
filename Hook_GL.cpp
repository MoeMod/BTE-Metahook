#include "metahook.h"
#include "bte_const.h"
#include "gl/gl.h"
#include "strtools.h"
#include "exportfuncs.h"
#include <eventscripts.h>
#include "hud.h"
#include "Client/HUD/nvg.h"
#include "studio.h"
#include "sys.h"

#define MAX_LIGHTMAPS		96

#define BLOCK_SIZE	128

struct glRect_t
{
	int l;
	int t;
	int w;
	int h;
};

int lightmap_textures;
glpoly_t *lightmap_polys[MAX_LIGHTMAPS];
qboolean lightmap_modified[MAX_LIGHTMAPS];
glRect_t lightmap_rectchange[MAX_LIGHTMAPS];
int allocated[MAX_LIGHTMAPS][BLOCK_SIZE];
byte lightmaps[MAX_LIGHTMAPS * BLOCK_SIZE * BLOCK_SIZE * 4];

int AllocBlock(int w, int h, int *x, int *y)
{
	int		i, j;
	int		best, best2;
	int		bestx;
	int		texnum;

	for (texnum = 0; texnum<MAX_LIGHTMAPS; texnum++)
	{
		best = BLOCK_SIZE;

		for (i = 0; i<BLOCK_SIZE - w; i++)
		{
			best2 = 0;

			for (j = 0; j<w; j++)
			{
				if (allocated[texnum][i + j] >= best)
					break;
				if (allocated[texnum][i + j] > best2)
					best2 = allocated[texnum][i + j];
			}
			if (j == w)
			{	// this is a valid spot
				*x = i;
				*y = best = best2;
			}
		}

		if (best + h > 128)
			continue;

		for (i = 0; i<w; i++)
			allocated[texnum][*x + i] = best + h;

		return texnum;
	}

	Sys_ErrorEx("AllocBlock: full");

	return 0;
}

void(*R_BuildLightMap)(msurface_t *psurf, unsigned char *dest, int stride) = (void(*)(msurface_t *, unsigned char *, int))0x1D47F70;
void(*BuildSurfaceDisplayList)(msurface_t *fa) = (void(*)(msurface_t *))0x1D4A3A0;
void(*GL_SelectTexture)(int) = (void(*)(int))0x1D3CA40;
void(*GL_Bind)(int) = (void(*)(int))0x1D3C9E0;

void GL_CreateSurfaceLightmap(msurface_t *surf)
{
	int		smax, tmax, s, t, l, i;
	byte	*base;

	if (surf->flags & 0x14)	// (SURF_DRAWSKY | SURF_DRAWTURB)
		return;

	if ((surf->flags & 0x20) && (surf->texinfo->flags & 1))	// SURF_DRAWTILED
		return;

	smax = (surf->extents[0] >> 4) + 1;
	tmax = (surf->extents[1] >> 4) + 1;

	int lightmap_bytes = *(int *)0x280FA28;

	surf->lightmaptexturenum = AllocBlock(smax, tmax, &surf->light_s, &surf->light_t);
	base = lightmaps + surf->lightmaptexturenum*lightmap_bytes*BLOCK_SIZE*BLOCK_SIZE;
	base += (surf->light_t * BLOCK_SIZE + surf->light_s) * lightmap_bytes;
	R_BuildLightMap(surf, base, BLOCK_SIZE*lightmap_bytes);
}

cvar_t *gl_monolight = (cvar_t *)0x1EC7AD4;
cvar_t *r_dynamic = (cvar_t *)0x1EC7700;

void GL_BuildLightmaps(void)
{
	Q_memset(allocated, 0, sizeof(allocated));
	*(int *)0x2C2016C = 1;	// r_framecount

	if (!lightmap_textures)
	{
		lightmap_textures = *(int *)0x1EC81B0;	// texture_extension_number
		*(int *)0x1EC81B0 += MAX_LIGHTMAPS;	// texture_extension_number
	}

	*(int *)0x1EC20C0 = GL_RGBA;	// gl_lightmap_format
	*(int *)0x280FA28 = 4;	// lightmap_bytes
	*(int *)0x280E5D4 = 4;	// lightmap_used

	if (!gl_monolight->value)
		*(int *)0x280E5D4 = 3;	// lightmap_used

	for (int i = 1; i < 2048; i++)
	{
		model_t *m = IEngineStudio.GetModelByIndex(i);

		if (!m)
			break;

		if (m->name[0] == '*')
			continue;

		*(model_t **)0x280FA24 = m;	// currentmodel
		*(mvertex_t **)0x2C1F460 = m->vertexes;	// r_pcurrentvertbase

		for (int j = 0; j < m->numsurfaces; j++)
		{
			GL_CreateSurfaceLightmap(m->surfaces + j);

			if (m->surfaces[j].flags & 0x10)		// SURF_DRAWTURB
				continue;

			BuildSurfaceDisplayList(m->surfaces + j);
		}
	}

	if (!*(int *)0x1EC7B2C)	// gl_texsort
		GL_SelectTexture(*(int *)0x1EC81B8);	// TEXTURE1_SGIS

	for (int i = 0; i < MAX_LIGHTMAPS; i++)
	{
		if (!allocated[i][0])
			break;

		lightmap_modified[i] = false;

		GL_Bind(lightmap_textures + i);

		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// lightmap_used						// gl_lightmap_format
		glTexImage2D(GL_TEXTURE_2D, 0, *(int *)0x280E5D4, BLOCK_SIZE, BLOCK_SIZE, 0, *(int *)0x1EC20C0, GL_UNSIGNED_BYTE,
			lightmaps + i * BLOCK_SIZE * BLOCK_SIZE * (*(int *)0x280FA28)	// lightmap_bytes
			);
	}

	if (!*(int *)0x1EC7B2C)	// gl_texsort
		GL_SelectTexture(*(int *)0x1EC81B4);	// TEXTURE0_SGIS
}

void R_RenderDynamicLightmaps(msurface_t *fa)
{
	texture_t	*t;
	byte		*base;
	int			maps;
	glRect_t    *theRect;
	int smax, tmax;

	(*(int *)0x2C2017C)++;	// c_brush_polys

	if (fa->flags & 0x14)		// SURF_DRAWSKY | SURF_DRAWTURB
		return;

	int lightmap_bytes = *(int *)0x280FA28;
	int r_framecount = *(int *)0x2C2016C;
	int *d_lightstylevalue = (int *)0x2C20320;	// int[256]

	fa->polys->chain = lightmap_polys[fa->lightmaptexturenum];
	lightmap_polys[fa->lightmaptexturenum] = fa->polys;

	// check for lightmap modification
	for (maps = 0; maps < MAXLIGHTMAPS && fa->styles[maps] != 255;
		maps++)
		if (d_lightstylevalue[fa->styles[maps]] != fa->cached_light[maps])
			goto dynamic;

	if (fa->dlightframe == r_framecount	// dynamic this frame
		|| fa->cached_dlight)			// dynamic previously
	{
	dynamic:
		if (r_dynamic->value)
		{
			lightmap_modified[fa->lightmaptexturenum] = true;
			theRect = &lightmap_rectchange[fa->lightmaptexturenum];
			if (fa->light_t < theRect->t) {
				if (theRect->h)
					theRect->h += theRect->t - fa->light_t;
				theRect->t = fa->light_t;
			}
			if (fa->light_s < theRect->l) {
				if (theRect->w)
					theRect->w += theRect->l - fa->light_s;
				theRect->l = fa->light_s;
			}
			smax = (fa->extents[0] >> 4) + 1;
			tmax = (fa->extents[1] >> 4) + 1;
			if ((theRect->w + theRect->l) < (fa->light_s + smax))
				theRect->w = (fa->light_s - theRect->l) + smax;
			if ((theRect->h + theRect->t) < (fa->light_t + tmax))
				theRect->h = (fa->light_t - theRect->t) + tmax;
			base = lightmaps + fa->lightmaptexturenum*lightmap_bytes*BLOCK_SIZE*BLOCK_SIZE;
			base += fa->light_t * BLOCK_SIZE * lightmap_bytes + fa->light_s * lightmap_bytes;
			R_BuildLightMap(fa, base, BLOCK_SIZE*lightmap_bytes);
		}
	}
}

void(*g_pfnCL_LinkPlayers)(void);

void CL_LinkPlayers(void)
{
	cl_entity_t *player = gEngfuncs.GetLocalPlayer();
	int effects = player->curstate.effects;

	if (IS_ZOMBIE_MODE &&  HudNVG().m_bOn)
	{
		if (effects & (EF_BRIGHTLIGHT | EF_DIMLIGHT))
			player->curstate.effects &= ~(EF_BRIGHTLIGHT | EF_DIMLIGHT);
	}

	g_pfnCL_LinkPlayers();

	player->curstate.effects = effects;
}

void(*g_pfnR_SetupFrame)(void) = NULL;
void(*g_pfnR_RenderScene)(void) = NULL;

float				*r_blend = (float *)0x2788E14;	//55 (2788E14)
cvar_t				*r_fullbright = (cvar_t *)0x1EC7608;	//79 (1EC7608)


void R_SetupFrame(void)
{
	g_pfnR_SetupFrame();

	if (HudNVG().g_bZBNightVisionOn)
	{
		r_fullbright->value = 1;
	}
}

void R_RenderScene(void)
{
	g_pfnR_RenderScene();

	r_fullbright->value = 0;
}

void(*g_pfnR_RenderBrushPoly)(msurface_t *psurf);
void R_RenderBrushPoly(msurface_t *psurf)
{
	int idx;
	if (IS_FIRSTPERSON_SPEC)
		idx = g_iUser2;
	else
		idx = gEngfuncs.GetLocalPlayer()->index;

	if (IS_ZOMBIE_MODE && HudNVG().m_bOn && vPlayer[idx].team != 1)
	{
		glColor4f(0.7, 0.5, 0.5, (*r_blend));
	}

	g_pfnR_RenderBrushPoly(psurf);
}

void(*g_pfnR_DrawSequentialPoly)(msurface_t *s, int face);
void R_DrawSequentialPoly(msurface_t *s, int face)
{
	int idx;
	if (IS_FIRSTPERSON_SPEC)
		idx = g_iUser2;
	else
		idx = gEngfuncs.GetLocalPlayer()->index;

	if (IS_ZOMBIE_MODE && HudNVG().m_bOn && vPlayer[idx].team != 1)
	{
		glColor4f(0.7, 0.5, 0.5, (*r_blend));
	}

	g_pfnR_DrawSequentialPoly(s, face);
}

void GL_InitHook(void)
{
	void *dummy;
	// Patch R_DrawSequentialPoly
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D48653 + 0x1), (DWORD)lightmaps);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D48609 + 0x2), (DWORD)&lightmap_textures);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D48A22 + 0x2), (DWORD)&lightmap_textures);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4861D + 0x3), (DWORD)lightmap_modified);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4862A + 0x3), (DWORD)lightmap_modified);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D48638 + 0x2), (DWORD)lightmap_rectchange);

	// Patch R_BlendLightmaps
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4925D + 0x2), (DWORD)lightmaps);
	g_pMetaHookAPI->WriteMemory((void *)(0x1D493E8 + 0x2), (byte *)"\x60", 1);	// change the loop condition to < 96(MAX_LIGHTMAPS)
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D49206 + 0x1), (DWORD)&lightmap_textures);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D491F7 + 0x3), (DWORD)lightmap_polys);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D49240 + 0x3), (DWORD)lightmap_modified);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D49263 + 0x3), (DWORD)lightmap_modified);

	// Patch R_RenderBrushPoly
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D495FC + 0x1), (DWORD)lightmaps);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4953B + 0x3), (DWORD)lightmap_polys);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D49542 + 0x3), (DWORD)lightmap_polys);
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D495DF + 0x3), (DWORD)lightmap_modified);

	// Patch R_DecalMPoly
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4C45A + 0x2), (DWORD)&lightmap_textures);

	// Patch R_DrawBrushModel
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D499FA + 0x1), (DWORD)sizeof(lightmap_polys));
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D49A01 + 0x1), (DWORD)lightmap_polys);

	// Patch R_DrawWorld
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4A0C8 + 0x1), (DWORD)sizeof(lightmap_polys));
	g_pMetaHookAPI->WriteDWORD((void *)(0x1D4A0CF + 0x1), (DWORD)lightmap_polys);

	g_pMetaHookAPI->InlineHook((void *)0x1D4A2C0, AllocBlock, dummy);
	g_pMetaHookAPI->InlineHook((void *)0x1D4A810, GL_BuildLightmaps, dummy);
	g_pMetaHookAPI->InlineHook((void *)0x1D48AC0, R_RenderDynamicLightmaps, dummy);
	g_pMetaHookAPI->InlineHook((void *)0x1D16590, CL_LinkPlayers, (void *&)g_pfnCL_LinkPlayers);
	g_pMetaHookAPI->InlineHook((void *)0x1D45F90, R_SetupFrame, (void *&)g_pfnR_SetupFrame);
	g_pMetaHookAPI->InlineHook((void *)0x1D46E80, R_RenderScene, (void *&)g_pfnR_RenderScene);

	// g_pMetaHookAPI->InlineHook((void *)0x1D497A0, R_SetRenderMode, (void *&)g_pfnR_SetRenderMode);
	g_pMetaHookAPI->InlineHook((void *)0x1D49490, R_RenderBrushPoly, (void *&)g_pfnR_RenderBrushPoly);
	g_pMetaHookAPI->InlineHook((void *)0x1D484E0, R_DrawSequentialPoly, (void *&)g_pfnR_DrawSequentialPoly);
}