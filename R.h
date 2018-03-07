#pragma once

#include <studio.h>

extern mstudiotexture_t gCurrentTexture;

typedef struct
{
	void(*R_StudioLighting)(float *lv, int bone, int flags, vec3_t normal);
	void(*R_StudioSetupLighting)(alight_t *plighting);
	int(*R_StudioCheckBBox)(void);
}studiofuncs_t;
extern studiofuncs_t gStudioFuncs;
extern r_studio_interface_s gStudioInterface;

void R_LightLambert(float light[3][4], float *normal, float *src, float *lambert);
void R_DrawViewModel(void);
void R_DrawSpriteModel(cl_entity_t *pEntity);
void R_StudioSetupSkin(studiohdr_t *phdr, int index);
void R_InstallHook(void);

void R_StudioSetupLighting(alight_t *plighting);
void R_StudioLighting(float *lv, int bone, int flags, vec3_t normal);

int R_StudioCheckBBox(void);
int R_StudioDrawModel(int flags);

void StudioFuncs_InstallHook(void);