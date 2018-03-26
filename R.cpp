#include <metahook.h>
#include <plugins.h>
#include <client.h>
#include <studio.h>
#include <configs.h>
#include <exportfuncs.h>
#include <qgl.h>
#include <shader.h>
#include <weapons.h>
#include "calcbody.h"
#include "strtools.h"
#include "pm_movevars.h"

#include "Client/hud.h"
#include "Client/HUD3D.h"
#include "Client/HUD3D_ZB.h"
#include "Client/muzzleflash.h"
#include "Client/PlayerClassManager.h"
#include "Client/WeaponManager.h"
#include "Client/TextureManager.h"

#include "R.h"

#define GetEngfuncsAddress(addr) (g_dwEngineBase+addr-0x1D01000)

#define R_LIGHTLAMBERT_SIG "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x2C\x8B\x0D\x2A\x2A\x2A\x2A\x53\x56\x57\x33\xFF\x3B\xCF\x75\x2A\x8B\x45\x10\x8B\x4D\x14\x8B\x10"
void(*g_pfnR_LightLambert)(float light[3][4], float *normal, float *src, float *lambert);

typedef void(*type_R_DrawViewModel)(void);
//extern type_R_DrawViewModel	g_real_R_DrawViewModel;
type_R_DrawViewModel			g_pfn_R_DrawViewModel;
type_R_DrawViewModel			g_real_R_DrawViewModel;
type_R_DrawViewModel			g_pfn_R_PreDrawViewModel;
//void (*g_pfn_R_PreDrawViewModel) = 0;

//#define R_STUDIO_SETUPSKIN_SIG "\x55\x8B\xEC\x83\xE4\xF8\x83\xEC\x2C\x8B\x0D\x2A\x2A\x2A\x2A\x53\x56\x57\x33\xFF\x3B\xCF\x75\x2A\x8B\x45\x10\x8B\x4D\x14\x8B\x10"
#define R_STUDIOSETUPSKIN_SIG "\xA0\x2A\x2A\x2A\x2A\x81\xEC\x0C\x01\x00\x00\xA8\x02\x53\x56\x57\x0F\x85"
void(*g_pfnR_StudioSetupSkin)(studiohdr_t *phdr, int index);

void(*g_pfnR_DrawSpriteModel)(cl_entity_t *);

#define R_LOADSKYS_SIG "\x83\xEC\x6C\xA1\x2A\x2A\x2A\x2A\x56\x85\xC0\xC7\x44\x24\x0C\x00\x00\x00\x00\x75\x2A\x33\xF6\x8D\x4C\x24\x04\x8D\x86\x2A\x2A\x2A\x2A"
void(*g_pfnR_LoadSkys)(void) = NULL;

mstudiotexture_t gCurrentTexture;

void R_LightLambert(float light[3][4], float *normal, float *src, float *lambert)
{
	g_pfnR_LightLambert(light, normal, src, lambert);

	if (g_iBTEWeapon == WPN_STORMGIANT && gCurrentTexture.flags & STUDIO_NF_ADDITIVE)
	{
		return;
	}

	if (g_iBTEWeapon == WPN_DUALSWORD && gCurrentTexture.flags & STUDIO_NF_ADDITIVE)
	{
		return;
	}

	if (gCurrentTexture.flags & STUDIO_NF_FULLBRIGHT)
	{
		lambert[0] = lambert[1] = lambert[2] = 1.0;
	}
}

void R_DrawViewModel(void)
{
	cl_entity_t *viewent = gEngfuncs.GetViewModel();

	if (1/*g_iViewEntityRenderMode >= 0*/)
	{
		if (WeaponManager().GetCurWeapon().iViewModelDisplay)
			viewent->curstate.rendermode = 100;
		else
			viewent->curstate.rendermode = g_iViewEntityRenderMode;

		viewent->curstate.renderfx = g_iViewEntityRenderFX;
		viewent->curstate.renderamt = g_iViewEntityRenderAmout;
		viewent->curstate.rendercolor = g_byViewEntityRenderColor;
	}

	float time = gEngfuncs.GetAbsoluteTime();

	//Shader_Begin();

	gMuzzleflashManager.Draw(time);

	if (Hud().m_FOV > 40.0)
		g_real_R_DrawViewModel();

	gHud3D.Redraw(time);
	gHud3D_ZB.Redraw(time);

	if (!gConfigs.bEnableShader)
		return;

	if (0/*gl_fxaa->value*/)
	{
#if 1
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, g_iScreenTexture);
		glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, ScreenWidth, ScreenHeight);

		//qglBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferId);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//glBindTexture(GL_TEXTURE_2D, m_iColorBufferId);
		//glGenerateMipmap(GL_TEXTURE_2D);
		//glBindTexture(GL_TEXTURE_2D, 0);



		//qglBindFramebuffer(GL_FRAMEBUFFER, m_iFrameBufferId);

		qglUseProgramObjectARB(progFXAA);
		qglUniform1iARB(uniformFXAA.tex0, 0);
		qglUniform1fARB(uniformFXAA.rt_w, ScreenWidth);
		qglUniform1fARB(uniformFXAA.rt_h, ScreenHeight);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColor4ub(255, 255, 255, 255);
		glBindTexture(GL_TEXTURE_2D, g_iScreenTexture);

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glLoadIdentity();

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();

		glOrtho(0, 1, 1, 0, 0.1, 100);

		glDisable(GL_DEPTH_TEST);
		glCullFace(GL_NONE);
		glDisable(GL_CULL_FACE);

		glColor4f(1.0, 1.0, 1.0, 1.0);

		glBegin(GL_QUADS);
		glTexCoord2f(0, 0);
		glVertex3f(0, 1, -1);
		glTexCoord2f(1, 0);
		glVertex3f(1, 1, -1);
		glTexCoord2f(1, 1);
		glVertex3f(1, 0, -1);
		glTexCoord2f(0, 1);
		glVertex3f(0, 0, -1);
		glEnd();

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);

		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);

		qglUseProgramObjectARB(0);


#endif


	}

	return;
}

extern TEMPENTITY * g_pPetrolboomerFlame;
extern TEMPENTITY * g_pRunebladeEffect;
extern TEMPENTITY *g_pDualSwordEffect1;
extern TEMPENTITY *g_pDualSwordEffect2;

void R_DrawSpriteModel(cl_entity_t *pEntity)
{
	/*
	static cl_entity_t *pViewEntity = gEngfuncs.GetViewModel();

	if (pEntity && pViewEntity)
	{
	if (pEntity->curstate.scale < 0.3 && pEntity->model->name && strstr(pEntity->model->name, "flame3"))
	pEntity->origin = pViewEntity->attachment[1];
	}
	*/

	if (g_pPetrolboomerFlame && pEntity == &(g_pPetrolboomerFlame->entity))
		return;
	if (g_pRunebladeEffect && pEntity == &(g_pRunebladeEffect->entity))
		return;
	if (g_pDualSwordEffect1 && pEntity == &(g_pDualSwordEffect1->entity))
		return;
	if (g_pDualSwordEffect2 && pEntity == &(g_pDualSwordEffect2->entity))
		return;

	return g_pfnR_DrawSpriteModel(pEntity);
}

void R_StudioSetupSkin(studiohdr_t *phdr, int index)
{
	mstudiotexture_t *ptexture = (mstudiotexture_t *)((byte *)phdr + phdr->textureindex);

	gCurrentTexture = ptexture[index];
	
	int iTexture = 0;
	// models/p_zombibomb
	if (strlen(IEngineStudio.GetCurrentEntity()->model->name) >= 18 && !strnicmp(IEngineStudio.GetCurrentEntity()->model->name + 9, "zombibomb", 9))
	{
		if (IEngineStudio.GetViewEntity() == IEngineStudio.GetCurrentEntity() && g_iBTEWeapon == WPN_ZOMBIBOMB)
		{
			if (g_iCurrentWeapon == WEAPON_SMOKEGRENADE)
				iTexture = TextureManager().GetReplacedTexture("Zombi Bomb Green", gCurrentTexture.name);
		}
		else if (!strnicmp(IEngineStudio.GetCurrentEntity()->model->name + 7, "p_zombibomb-c", 13))
		{
			iTexture = TextureManager().GetReplacedTexture("Zombi Bomb Green", gCurrentTexture.name);
		}
		else if (*(IEngineStudio.GetCurrentEntity()->model->name + 7) == 'w' && IEngineStudio.GetCurrentEntity()->curstate.skin)
		{
			iTexture = TextureManager().GetReplacedTexture("Zombi Bomb Green", gCurrentTexture.name);
		}
		if (iTexture >= 0)
			qglEnable(GL_ALPHA_TEST);

	}
	else if (IEngineStudio.GetViewEntity() == IEngineStudio.GetCurrentEntity())
	{
		int id = 0;
		if (g_bAlive)
			id = gEngfuncs.GetLocalPlayer()->index;
		else
			id = gEngfuncs.GetLocalPlayer()->curstate.iuser2;
		iTexture = TextureManager().GetReplacedTexture(PlayerClassManager()[id].szTextureReplaceKey, gCurrentTexture.name);

		if (iTexture >= 0)
			qglEnable(GL_ALPHA_TEST);
	}

	if (iTexture > 0)
	{
		//TextureManager()[iTexture].Update();
		qglBindTexture(GL_TEXTURE_2D, iTexture);
	}
	else
	{
		//TextureManager()[ptexture[index].index].Update();
		g_pfnR_StudioSetupSkin(phdr, index);
	}
}

bool gLoadSky = true;
unsigned(*gSkyTexNumber) = (unsigned(*))0x280E0A0;
const char *suf[] = { "rt","bk","lf","ft","up","dn" };

void R_LoadSkys(void)
{
	//return g_pfnR_LoadSkys();
	
	movevars_t * const pmovevars = (movevars_t *)0x027e8fc0;

	int		i, j;
	char	name[64];
	byte	*buffer;
	qboolean printed = false;

	if (!gLoadSky)
	{
		for (i = 0; i < 6; i++)
		{
			if (gSkyTexNumber[i])
			{
				qglDeleteTextures(1, &gSkyTexNumber[i]);
				gSkyTexNumber[i] = 0;
			}
		}

		//return;
	}

	for (i = 0; i<6; i++)
	{
		Q_snprintf(name, sizeof(name), "gfx\\env\\%s%s.tga", pmovevars->skyName, suf[i]);

		auto &Texture = TextureManager().GetReplacedTexture("Sky", name);
		if (Texture)
		{
			gSkyTexNumber[i] = Texture;
		}
		else
		{
			gSkyTexNumber[i] = TextureManager()[name];
		}
		
		qglBindTexture(GL_TEXTURE_2D, gSkyTexNumber[i]);
		

		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		qglTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (!printed)
		{
			gEngfuncs.Con_DPrintf("SKY:  ");
			printed = true;
		}

		gEngfuncs.Con_DPrintf("%s%s, ", pmovevars->skyName, suf[i]);
	}

	if (printed)
	{
		gEngfuncs.Con_DPrintf("done\n");
	}

	gLoadSky = false;
}

void R_InstallHook(void)
{
	g_pfn_R_DrawViewModel = (decltype(g_pfn_R_DrawViewModel))GetEngfuncsAddress(0x1D45660);
	g_pfn_R_PreDrawViewModel = (decltype(g_pfn_R_PreDrawViewModel))GetEngfuncsAddress(0x1D45A10);
	g_pfnR_LightLambert = (decltype(g_pfnR_LightLambert))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, R_LIGHTLAMBERT_SIG, sizeof(R_LIGHTLAMBERT_SIG) - 1);
	g_pfnR_StudioSetupSkin = (void(*)(studiohdr_t *, int))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, R_STUDIOSETUPSKIN_SIG, sizeof(R_STUDIOSETUPSKIN_SIG) - 1);
	g_pfnR_LoadSkys = (void(*)(void))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, R_LOADSKYS_SIG, sizeof(R_LOADSKYS_SIG) - 1);

	g_pMetaHookAPI->InlineHook((void *)g_pfn_R_DrawViewModel, R_DrawViewModel, (void *&)g_real_R_DrawViewModel);
	g_pMetaHookAPI->InlineHook(g_pfnR_StudioSetupSkin, R_StudioSetupSkin, (void *&)g_pfnR_StudioSetupSkin);
	g_pMetaHookAPI->InlineHook(g_pfnR_LightLambert, R_LightLambert, (void *&)g_pfnR_LightLambert);
	g_pMetaHookAPI->InlineHook((void *)0x1D448F0, R_DrawSpriteModel, (void *&)g_pfnR_DrawSpriteModel);
	g_pMetaHookAPI->InlineHook(g_pfnR_LoadSkys, R_LoadSkys, (void *&)g_pfnR_LoadSkys);
}

#define R_STUDIOLIGHTING_SIG "\x51\xDB\x05\x2A\x2A\x2A\x2A\x8A\x4C\x24\x10\xB8\x01\x00\x00\x00\x84\xC8\xD9\x5C\x24\x00"
#define R_STUDIOLIGHTING_SIG_NEW "\x55\x8B\xEC\x51\xDB\x05\x2A\x2A\x2A\x2A\x8A\x4D\x10\xB8\x01\x00\x00\x00\x84\xC8\xD9\x5D\xFC"
#define STUDIO_NF_FULLBRIGHT  0x0004

studiofuncs_t gStudioFuncs;
alight_t g_savedLighting;

void R_StudioSetupLighting(alight_t *plighting)
{
	//plighting->color[0] = plighting->color[1] = plighting->color[2] = 1.0;
	memcpy(&g_savedLighting, plighting, sizeof(alight_t));
	gStudioFuncs.R_StudioSetupLighting(plighting);
}

double g_dbStormgiantEffectTime = 0.0;

void R_StudioLighting(float *lv, int bone, int flags, vec3_t normal)
{

	if ((g_iBTEWeapon == WPN_STORMGIANT) && flags & STUDIO_NF_ADDITIVE)
	{
		int r_ambientlight = g_savedLighting.ambientlight;
		float r_color[3];
		memcpy(r_color, g_savedLighting.color, sizeof(g_savedLighting.color));
		float r_shadelight = g_savedLighting.shadelight;

		if (cl.time < g_dbStormgiantEffectTime + 1.0)
		{
			g_savedLighting.ambientlight = 100 - (cl.time - g_dbStormgiantEffectTime) * 100.0;
			g_savedLighting.shadelight = 128 - (cl.time - g_dbStormgiantEffectTime) * 128.0;
		}
		else
		{
			g_savedLighting.ambientlight = 100;
			g_savedLighting.shadelight = 128.0f;
		}
		//g_savedLighting.color[0] = g_savedLighting.color[1] = g_savedLighting.color[2] = 1.0;


		IEngineStudio.StudioSetupLighting(&g_savedLighting);

		gStudioFuncs.R_StudioLighting(lv, bone, flags, normal);

		g_savedLighting.ambientlight = r_ambientlight;
		memcpy(g_savedLighting.color, r_color, sizeof(g_savedLighting.color));
		g_savedLighting.shadelight = r_shadelight;

		IEngineStudio.StudioSetupLighting(&g_savedLighting);

		return;
	}

	if (flags & STUDIO_NF_FULLBRIGHT)
	{
		int r_ambientlight = g_savedLighting.ambientlight;
		float r_color[3];
		memcpy(r_color, g_savedLighting.color, sizeof(g_savedLighting.color));
		float r_shadelight = g_savedLighting.shadelight;

		g_savedLighting.ambientlight = 100;
		//g_savedLighting.color[0] = g_savedLighting.color[1] = g_savedLighting.color[2] = 1.0;
		g_savedLighting.shadelight = 128.0f;

		IEngineStudio.StudioSetupLighting(&g_savedLighting);

		gStudioFuncs.R_StudioLighting(lv, bone, flags, normal);

		g_savedLighting.ambientlight = r_ambientlight;
		memcpy(g_savedLighting.color, r_color, sizeof(g_savedLighting.color));
		g_savedLighting.shadelight = r_shadelight;

		IEngineStudio.StudioSetupLighting(&g_savedLighting);

		return;
	}

	//nothing to do, just do the original way.

	gStudioFuncs.R_StudioLighting(lv, bone, flags, normal);

}

//r_studio_interface_s *pClientStudioInterface = (r_studio_interface_s *)0x19E06B0;
/*
int R_StudioDrawModel(int flags)
{
static cl_entity_t *pCurrentEntity = NULL;
static cl_entity_t *pViewEntity = NULL;

pCurrentEntity = IEngineStudio.GetCurrentEntity();
pViewEntity = IEngineStudio.GetViewEntity();

ClientPrint(HUD_PRINTTALK, "Check!");

if (pCurrentEntity && pViewEntity && pCurrentEntity != pViewEntity)
{
if (pCurrentEntity->model)
{
ClientPrint(HUD_PRINTTALK, va("Condition! model %s", pCurrentEntity->model->name));

pCurrentEntity->origin = pViewEntity->attachment[0];
pCurrentEntity->angles = pViewEntity->angles;

glDisable(GL_ALPHA_TEST);
int iResult = pClientStudioInterface->StudioDrawModel(flags);
glEnable(GL_ALPHA_TEST);

return iResult;
}
}

return pClientStudioInterface->StudioDrawModel(flags);
}
*/

r_studio_interface_s gStudioInterface;

int R_StudioCheckBBox(void)
{
	cl_entity_s *viewent = IEngineStudio.GetViewEntity();
	cl_entity_s *curent = IEngineStudio.GetCurrentEntity();
	if (viewent == curent)
		return 1;
	return gStudioFuncs.R_StudioCheckBBox();
}

float g_flBloodhunterAnimTime = 0.0;
int g_iBloodhunterSecAnim = 0;

int Hook_R_StudioDrawModel(int flags)
{
	cl_entity_s *viewent = IEngineStudio.GetViewEntity();
	cl_entity_s *curent = IEngineStudio.GetCurrentEntity();
	static cvar_t *m_pCvarRightHand = gEngfuncs.pfnGetCvarPointer("cl_righthand");

	if (curent == viewent)
	{
		if (g_iBTEWeapon == WPN_BLOODHUNTER && g_iBloodhunterSecAnim)
		{
			//int frame = (cl.time - g_dbBloodhunterAnimTime) * 30.0;
			static cl_entity_t saveent;
			saveent = *curent;
			curent->model = IEngineStudio.Mod_ForName("models/v_bloodhunter.mdl", 0);

			//从CStudioModelRenderer::StudioEstimateFrame抄来的f'(t)微积分QAQ
			//dfdt = (m_clTime - m_pCurrentEntity->curstate.animtime) * m_pCurrentEntity->curstate.framerate * pseqdesc->fps;

			curent->curstate.animtime = g_flBloodhunterAnimTime;
			curent->curstate.framerate = 1.0;
			curent->curstate.sequence = g_iBloodhunterSecAnim;

			gStudioInterface.StudioDrawModel(flags);
			*curent = saveent;
		}

		if (g_iBTEWeapon == WPN_STORMGIANT)
		{
			static BodyEnumInfo_t info[6] =
			{
				{ 0, 1 },	// 1号Part显示2号Sub-model，它有2个Sub-model
				{ 0, 1 },
				{ 0, 1 },
				{ 0, 1 },
				{ 0, 4 },
				{ 0, 2 }
			};

			if (cl.time < g_dbStormgiantEffectTime + 0.5)
				info[4].body = 3;
			else if (cl.time < g_dbStormgiantEffectTime + 1.0)
				info[4].body = 2;
			else
				info[4].body = 0;

			info[5].body = g_iViewEntityBody;

			int iBackupBody = curent->curstate.body;

			curent->curstate.body = CalcBody(info, 6);

			int iReturn = gStudioInterface.StudioDrawModel(flags);

			curent->curstate.body = iBackupBody;

			return iReturn;
		}

		/*
		if (g_iCurrentWeapon == WEAPON_KNIFE)
		{
		if (!m_pCvarRightHand->value)
		{
		//m_pCvarRightHand->value = 0;
		}
		}*/
	}
	/*
	if (IS_ZOMBIE_MODE && Q_strstr(curent->model->name, "supplybox") && vPlayer[gEngfuncs.GetLocalPlayer()->index].team == 1)
	{
	static cl_entity_t saveent;
	saveent = *curent;
	curent->model = IEngineStudio.Mod_ForName("sprites/e_button01.spr", FALSE);
	curent->curstate.rendermode = kRenderTransAdd;
	curent->curstate.renderamt = 200;
	curent->curstate.scale = 0.12;
	curent->origin[2] += 25;
	g_pfnR_DrawSpriteModel(curent);
	*curent = saveent;
	}
	*/
	int iReturn = gStudioInterface.StudioDrawModel(flags);

	if (curent == viewent)
	{
		if (g_pPetrolboomerFlame && viewent->curstate.sequence != 1 && viewent->curstate.sequence != 4 && viewent->curstate.sequence != 5)
		{
			vec3_t up, right, forward;
			gEngfuncs.pfnAngleVectors(gEngfuncs.GetLocalPlayer()->angles, forward, right, up);

			g_pPetrolboomerFlame->entity.origin = viewent->attachment[1] + up * 5;
			g_pfnR_DrawSpriteModel(&(g_pPetrolboomerFlame->entity));

		}
		if (g_pRunebladeEffect)
		{
			g_pRunebladeEffect->entity.origin = viewent->attachment[0];
			//g_pfnR_DrawSpriteModel(&(g_pRunebladeEffect->entity));
		}
		if (g_pDualSwordEffect1)
		{
			if (viewent->curstate.sequence > 6 && viewent->curstate.sequence <= 11)
			{
				g_pDualSwordEffect1->entity.origin = viewent->attachment[1];
				g_pfnR_DrawSpriteModel(&(g_pDualSwordEffect1->entity));
			}
		}
		if (g_pDualSwordEffect2)
		{
			if (viewent->curstate.sequence <= 6)
			{
				g_pDualSwordEffect2->entity.origin = viewent->attachment[0];
				g_pfnR_DrawSpriteModel(&(g_pDualSwordEffect2->entity));
			}
		}
	}

	if (!strnicmp(curent->model->name, "models/w_", 9))
	{
		if (!curent->curstate.sequence)
		{
			static cl_entity_t saveent;
			saveent = *curent;
			static model_s *pModel = IEngineStudio.Mod_ForName("models/ef_gundrop.mdl", false);
			curent->model = pModel;
			curent->curstate.frame = ((float)(cl.time * 30.0));
			gStudioInterface.StudioDrawModel(flags);
			*curent = saveent;
		}

	}


	return iReturn;
}

void StudioFuncs_InstallHook(void)
{
	/*if (g_dwEngineBuildnum >= 5953)
	gStudioFuncs.R_StudioLighting = (void(*)(float *, int, int, vec3_t))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, R_STUDIOLIGHTING_SIG_NEW, sizeof(R_STUDIOLIGHTING_SIG_NEW) - 1);
	else
	*/gStudioFuncs.R_StudioLighting = (void(*)(float *, int, int, vec3_t))g_pMetaHookAPI->SearchPattern((void *)g_dwEngineBase, g_dwEngineSize, R_STUDIOLIGHTING_SIG, sizeof(R_STUDIOLIGHTING_SIG) - 1);

g_pMetaHookAPI->InlineHook(gStudioFuncs.R_StudioLighting, R_StudioLighting, (void *&)gStudioFuncs.R_StudioLighting);
g_pMetaHookAPI->InlineHook(gStudioFuncs.R_StudioSetupLighting, R_StudioSetupLighting, (void *&)gStudioFuncs.R_StudioSetupLighting);
g_pMetaHookAPI->InlineHook(gStudioFuncs.R_StudioCheckBBox, R_StudioCheckBBox, (void *&)gStudioFuncs.R_StudioCheckBBox);
}