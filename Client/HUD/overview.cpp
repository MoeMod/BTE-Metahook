#include "metahook.h"
#include "bte_const.h"
#include "gl/gl.h"
#include "triangleapi.h"
#include "event_api.h"
#include "pmtrace.h"
#include "pm_defs.h"
#include "hud.h"
#include "exportfuncs.h"
#include "BaseUI.h"
#include "TriAPI.h"
#include "message.h"
#include "Fonts.h"
#include "util.h"
#include "calcscreen.h"
#include "TextureManager.h"
#include "configs.h"

#include "zb3ui.h"
#include "zb4ui.h"
#include "overview.h"

static CHudOverview g_HudOverview;
CHudOverview &HudOverview()
{
	return g_HudOverview;
}

#define CALC_SCALE	1.4
#define CAMETA_BORDER	80
#define SCALE	8

int iTag[2];
cvar_t *cl_teammate_tag;

float flNextTraceCheck[33];

void CHudOverview::Init(void)
{
	m_iFlags |= HUD_ACTIVE;

	cl_radaron = gEngfuncs.pfnRegisterVariable("cl_radaron", "1", FCVAR_ARCHIVE);

	cl_teammate_tag = gEngfuncs.pfnRegisterVariable("cl_teammate_tag", "0", FCVAR_ARCHIVE);

	iTag[0] = vgui::surface()->CreateNewTextureID();
	iTag[1] = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(iTag[0], "resource\\hud\\other\\cttag", true, false);
	vgui::surface()->DrawSetTextureFile(iTag[1], "resource\\hud\\other\\trtag", true, false);

	m_iBombEntity = 0;
	m_fBombEntityTime = 0.0f;
}
void CHudOverview::VidInit()
{
	m_bCanCheck = false;
	m_pSprite = NULL;
	
	memset(flNextTraceCheck, 0, sizeof(flNextTraceCheck));
	memset(m_bCanSee, 0, sizeof(m_bCanSee));

}
void CHudOverview::CalcRefdef(struct ref_params_s *pParams)
{
	//VectorCopy(pParams->viewangles, mainViewAngles);
	//VectorCopy(pParams->vieworg, mainViewOrigin);

	float fYaw = pParams->viewangles[1] * (M_PI / 180.0);
	m_fViewAngles_SinYaw = sin(fYaw);
	m_fViewAngles_CosYaw = cos(fYaw);


	//displayCenterX = ScreenWidth / 2;
	//displayCenterY = ScreenHeight / 2;
	return;
}

int CHudOverview::CalcRadarPoint(const float* origin, int& screenx, int& screeny, int iCheck, int scale)
{
	int iReturn = 0;
	float dx = origin[0] - g_vecEyePos[0];
	float dy = origin[1] - g_vecEyePos[1];

	float x = dx*  m_fViewAngles_SinYaw - dy*m_fViewAngles_CosYaw;
	float y = dx*(-m_fViewAngles_CosYaw) - dy*m_fViewAngles_SinYaw;

	screenx = 100 + x / 10 * m_flZoom;
	screeny = 100 + y / 10 * m_flZoom;

	// Check Point (For Overview Spr Icon)
	if (iCheck)
	{
		if (screenx < scale / 2)
		{
			screenx = scale / CALC_SCALE;
			iReturn = 1;
		}
		if (screenx > 200 - scale / 2)
		{
			screenx = 200 - scale / CALC_SCALE;
			iReturn = 1;
		}
		if (screeny < scale / 2)
		{
			screeny = scale / CALC_SCALE;
			iReturn = 1;
		}
		if (screeny > 200 - scale / 2)
		{
			screeny = 200 - scale / CALC_SCALE;
			iReturn = 1;
		}
	}
	return iReturn;
}
void CHudOverview::EntityESP(void)
{
	if (gConfigs.bEnableNewHud) return;
	if (!cl_radaron->value) return;
	if (!m_bCanDraw) return;

	float screenaspect, xs, ys, xStep, yStep, x, y, z;
	int ix, iy, i, xTiles, yTiles, frame;

	i = m_pSprite->numframes / (4 * 3);
	i = sqrt(float(i));
	xTiles = i * 4;
	yTiles = i * 3;
	screenaspect = 4.0f / 3.0f;
	xs = m_vecOrigin.x;
	ys = m_vecOrigin.y;
	z = vieworg[2] - (10 + 1.2 * 200)*4.1f / m_flZoom;

	frame = 0;
	int numframes = ModelFrameCount(m_pSprite);

	gEngfuncs.pTriAPI->RenderMode(kRenderTransTexture);
	gEngfuncs.pTriAPI->CullFace(TRI_NONE);
	if (g_iMod == MOD_ZB4)
		gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 1.0);
	else
		gEngfuncs.pTriAPI->Color4f(1.0, 1.0, 1.0, 0.8);


	if (m_bIsRotate)
	{
		xStep = (2 * 4096.0f / m_flZoom) / xTiles;
		yStep = -(2 * 4096.0f / (m_flZoom* screenaspect)) / yTiles;
		y = ys + (4096.0f / (m_flZoom * screenaspect));
		for (iy = 0; iy < yTiles; iy++)
		{
			x = xs - (4096.0f / (m_flZoom));
			for (ix = 0; ix < xTiles; ix++)
			{
				if (frame >= numframes)
					break;

				Tri_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x8370);
				Tri_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x8370);
				gEngfuncs.pTriAPI->SpriteTexture(m_pSprite, frame);
				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(x, y, z);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y, z);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y + yStep, z);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(x, y + yStep, z);
				gEngfuncs.pTriAPI->End();
				frame++;
				x += xStep;
			}
			y += yStep;
		}
	}
	else
	{
		xStep = -(2 * 4096.0f / m_flZoom) / xTiles;
		yStep = -(2 * 4096.0f / (m_flZoom* screenaspect)) / yTiles;
		x = xs + (4096.0f / (m_flZoom * screenaspect));
		for (ix = 0; ix < yTiles; ix++)
		{
			y = ys + (4096.0f / (m_flZoom));
			for (iy = 0; iy < xTiles; iy++)
			{
				if (frame >= numframes)
					break;

				Tri_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, 0x8370);
				Tri_TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, 0x8370);
				gEngfuncs.pTriAPI->SpriteTexture(m_pSprite, frame);
				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(x, y, z);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y, z);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f(x + xStep, y + yStep, z);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f(x, y + yStep, z);
				gEngfuncs.pTriAPI->End();
				frame++;
				y += yStep;
			}
			x += xStep;
		}
	}
}
bool CHudOverview::CanDraw(void)
{
	return m_pSprite && cl_radaron->value && !gEngfuncs.GetLocalPlayer()->curstate.iuser1 && g_bAlive && Hud().m_FOV > 40.0;

	if (!gConfigs.bEnableNewHud)
		return m_pSprite && cl_radaron->value && !gEngfuncs.GetLocalPlayer()->curstate.iuser1 && g_bAlive && Hud().m_FOV > 40.0;
	else
		return false;
}

void CHudOverview::Draw(float time)
{
	if (!CanDraw())
		return;

	if (!gConfigs.bEnableNewHud)
	{
		if (strlen(g_szLocation))
		{
			int iLength, iHeight;
			gEngfuncs.pfnDrawSetTextColor(0.0, 0.8, 0.0);
			gEngfuncs.pfnDrawConsoleStringLen(g_szLocation, &iLength, &iHeight);
			gEngfuncs.pfnDrawConsoleString(64 - iLength / 2, 210, g_szLocation);
		}
	}
}

void CHudOverview::DrawEntities(void)
{
	if (!CanDraw())
		return;
	
	if (!gConfigs.bEnableNewHud)
	{
		if (g_iMod == MOD_ZB4 && g_bAlive)
		{
			HudZombieMod4().m_Radar.DrawBackground();
			HudZombieMod4().m_Radar.Redraw();
		}
		else
		{
			Tri_Disable(GL_TEXTURE_2D);
			gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
			gEngfuncs.pTriAPI->Color4f(0, 0, 0, 1);
			gEngfuncs.pTriAPI->Begin(TRI_LINES);
			gEngfuncs.pTriAPI->Vertex3f(1, 0, 0);
			gEngfuncs.pTriAPI->Vertex3f(201, 1, 0);
			gEngfuncs.pTriAPI->End();
			gEngfuncs.pTriAPI->Begin(TRI_LINES);
			gEngfuncs.pTriAPI->Vertex3f(201, 1, 0);
			gEngfuncs.pTriAPI->Vertex3f(201, 200, 0);
			gEngfuncs.pTriAPI->End();
			gEngfuncs.pTriAPI->Begin(TRI_LINES);
			gEngfuncs.pTriAPI->Vertex3f(201, 200, 0);
			gEngfuncs.pTriAPI->Vertex3f(0, 200, 0);
			gEngfuncs.pTriAPI->End();
			gEngfuncs.pTriAPI->Begin(TRI_LINES);
			gEngfuncs.pTriAPI->Vertex3f(1, 200, 0);
			gEngfuncs.pTriAPI->Vertex3f(1, 0, 0);
			gEngfuncs.pTriAPI->End();
			Tri_Enable(GL_TEXTURE_2D);
		}

		gEngfuncs.pTriAPI->RenderMode(kRenderTransAdd);
		gEngfuncs.pTriAPI->Color4f(1, 0.62745f, 0, 1.0f);
		gEngfuncs.pTriAPI->SpriteTexture(m_pCamera, 0);
		gEngfuncs.pTriAPI->Begin(TRI_TRIANGLES);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(100 + CAMETA_BORDER*0.7, 100 - CAMETA_BORDER*0.7, 0);
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(100 - CAMETA_BORDER*0.7, 100 - CAMETA_BORDER*0.7, 0);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(100, 100, 0);
		gEngfuncs.pTriAPI->End();
	}

	cl_entity_t *pPlayer = gEngfuncs.GetLocalPlayer();
	cl_entity_t *pEntity = NULL;
	Vector vecForward2, vecRight2;
	float vSub[3], sAng[3];
	int iScale = SCALE;
	int fOut[2];
	int iCallBack;
	int iLocalIndex = pPlayer->index;

	if (!gConfigs.bEnableNewHud)
	{
		for (int i = 1; i <= m_iMaxClient; i++)
		{
			iScale = SCALE;
			if (i == iLocalIndex) continue;
			if (g_iMod == MOD_DM) break;
			pEntity = gEngfuncs.GetEntityByIndex(i);
			if (!pEntity) continue;

			if (vPlayer[pEntity->index].fInfectTime > Hud().m_flTime && (g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZSE)) goto DRAW_ESP;
			if (vPlayer[pEntity->index].team != vPlayer[iLocalIndex].team)
				continue;
		DRAW_ESP:
			if (vPlayer[pEntity->index].killtime < Hud().m_flTime)
				continue;
			Tri_Enable(GL_TEXTURE_2D);
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
			gEngfuncs.pTriAPI->Color4f(1, 1, 1, 1);
			gEngfuncs.pTriAPI->SpriteTexture(this->m_rgpPlayer[vPlayer[i].team - 1], 0);

			if (g_PlayerExtraInfo[pEntity->index].iFlag & SCOREATTRIB_BOMB)
				gEngfuncs.pTriAPI->SpriteTexture(m_rgpPlayer[2], 0);
			else if (/*g_PlayerExtraInfo[pEntity->index].iFlag & SCOREATTRIB_HERO ||*/ g_PlayerExtraInfo[pEntity->index].iFlag & SCOREATTRIB_VIP)
				gEngfuncs.pTriAPI->SpriteTexture(m_pVIP, 0);


			int iZB = 0;

			if (vPlayer[i].fAudioTime > Hud().m_flTime)
			{
				gEngfuncs.pTriAPI->SpriteTexture(this->m_pRadio, 0);
				float fSubTime = vPlayer[i].fAudioTime - Hud().m_flTime;
				fSubTime -= (int)fSubTime;
				fSubTime -= 0.5;
				fSubTime = fabsf(fSubTime);
				iScale = 10 + 15.0*fSubTime;
			}
			else if (vPlayer[i].fInfectTime > Hud().m_flTime && IS_ZOMBIE_MODE)
			{
				gEngfuncs.pTriAPI->SpriteTexture(this->m_pInfection, 0);
				float fSubTime = vPlayer[i].fInfectTime - Hud().m_flTime;
				fSubTime -= (int)fSubTime;
				fSubTime -= 0.5;
				fSubTime = fabsf(fSubTime);
				iScale = 10 + 15.0*fSubTime;
				iZB = 1;
			}
			else if (vPlayer[i].fDeathTime > Hud().m_flTime)
			{
				gEngfuncs.pTriAPI->SpriteTexture(this->m_pDeath, 0);
				float fSubTime = vPlayer[i].fDeathTime - Hud().m_flTime;
				fSubTime -= (int)fSubTime;
				fSubTime -= 0.5;
				fSubTime = fabsf(fSubTime);
				iScale = 10 + 15.0*fSubTime;
			}

			VectorCopy(pEntity->angles, sAng);

			vSub[1] = gEngfuncs.GetLocalPlayer()->angles[1] - sAng[1];
			vSub[0] = vSub[2] = 0;
			vSub[1] -= 90.0;
			gEngfuncs.pfnAngleVectors(vSub, vecForward2, vecRight2, NULL);
			iCallBack = CalcRadarPoint(iZB ? vPlayer[pEntity->index].vLastPos : pEntity->origin, fOut[0], fOut[1], 1, iScale);
			if (iCallBack) iScale /= CALC_SCALE;
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * iScale, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * -iScale, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * -iScale, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * iScale, 0);
			gEngfuncs.pTriAPI->End();
		}
		if (m_fBombEntityTime > Hud().m_flTime && vPlayer[iLocalIndex].team == 2)
		{
			iScale = SCALE;
			pEntity = gEngfuncs.GetEntityByIndex(m_iBombEntity);
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
			gEngfuncs.pTriAPI->Color4f(1, 1, 1, 1);
			gEngfuncs.pTriAPI->SpriteTexture(this->m_pBackPack, 0);
			VectorCopy(pEntity->angles, sAng);
			vSub[1] = gEngfuncs.GetLocalPlayer()->angles[1] - sAng[1];
			vSub[0] = vSub[2] = 0;
			vSub[1] -= 90.0;

			gEngfuncs.pfnAngleVectors(vSub, vecForward2, vecRight2, NULL);
			iCallBack = CalcRadarPoint(pEntity->origin, fOut[0], fOut[1], 1, iScale);
			if (iCallBack) iScale /= CALC_SCALE;
			gEngfuncs.pTriAPI->Begin(TRI_QUADS);
			gEngfuncs.pTriAPI->TexCoord2f(1, 0);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * iScale, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 0);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * -iScale, 0);
			gEngfuncs.pTriAPI->TexCoord2f(0, 1);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * -iScale, 0);
			gEngfuncs.pTriAPI->TexCoord2f(1, 1);
			gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * iScale, 0);
			gEngfuncs.pTriAPI->End();
		}
	}
	int bHostageUseMsgPos[4];
	int iHostageIndex = 0;

	if ((g_iMod == MOD_NONE || g_iMod == MOD_GHOST) && vPlayer[iLocalIndex].team == 1)
	{
		for (int i = 33; i<512; i++)
		{
			cl_entity_s *pEnt = gEngfuncs.GetEntityByIndex(i);
			if (!pEnt)
				continue;

			if (pEnt->curstate.iuser1 != 1213)
				continue;

			iHostageIndex++;
			bHostageUseMsgPos[iHostageIndex - 1] = FALSE;

			vec3_t sub;
			VectorSubtract(pEnt->origin, g_vecOrigin, sub);

			if (VectorLength(sub) > 1500)
			{
				bHostageUseMsgPos[iHostageIndex - 1] = TRUE;

				continue;
			}

			if (pEnt->origin[0] == pEnt->curstate.vuser1[0] && pEnt->origin[1] == pEnt->curstate.vuser1[1] && pEnt->origin[2] == pEnt->curstate.vuser1[2])
			{
				bHostageUseMsgPos[iHostageIndex - 1] = TRUE;

				continue;
			}

			VectorCopy(pEnt->origin, pEnt->curstate.vuser1);

			if (pEnt->curstate.sequence >= 49 && pEnt->curstate.sequence <= 56)
			{

				continue;
			}
			if (!gConfigs.bEnableNewHud)
			{
				iScale = SCALE;
				gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
				gEngfuncs.pTriAPI->Color4f(1, 1, 1, 1);
				gEngfuncs.pTriAPI->SpriteTexture(this->m_pHostage, 0);
				VectorCopy(pEnt->angles, sAng);
				vSub[1] = gEngfuncs.GetLocalPlayer()->angles[1] - sAng[1];
				vSub[0] = vSub[2] = 0;
				vSub[1] -= 90.0;

				gEngfuncs.pfnAngleVectors(vSub, vecForward2, vecRight2, NULL);

				iCallBack = CalcRadarPoint(pEnt->origin, fOut[0], fOut[1], 1, iScale);


				if (iCallBack) iScale /= CALC_SCALE;
				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * iScale, 0);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * -iScale, 0);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * -iScale, 0);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * iScale, 0);
				gEngfuncs.pTriAPI->End();
			}
		}


		for (int i = 0; i < 8; i++)
		{
			if (!bHostageUseMsgPos[i])
				continue;

			if (g_vecHostagePos[i][0] == 0 && g_vecHostagePos[i][1] == 0 && g_vecHostagePos[i][2] == 0)
				continue;


			if (!gConfigs.bEnableNewHud)
			{
				iScale = SCALE;
				gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
				gEngfuncs.pTriAPI->Color4f(1, 1, 1, 1);
				gEngfuncs.pTriAPI->SpriteTexture(this->m_pHostage, 0);

				vec3_t angle;
				memset(angle, 0, sizeof(angle));
				vSub[1] = gEngfuncs.GetLocalPlayer()->angles[1] - angle[1];
				vSub[0] = vSub[2] = 0;
				vSub[1] -= 90.0;
				gEngfuncs.pfnAngleVectors(vSub, vecForward2, vecRight2, NULL);

				iCallBack = CalcRadarPoint(g_vecHostagePos[i], fOut[0], fOut[1], 1, iScale);

				if (iCallBack) iScale /= CALC_SCALE;
				gEngfuncs.pTriAPI->Begin(TRI_QUADS);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * iScale, 0);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * iScale + vecForward2.y * -iScale, 0);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * -iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * -iScale, 0);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f(fOut[0] + vecRight2.x * -iScale + vecForward2.x * iScale, fOut[1] + vecRight2.y * -iScale + vecForward2.y * iScale, 0);
				gEngfuncs.pTriAPI->End();
			}
		}
	}
	cl_entity_t *pEntity2 = NULL;
	for (int i = 1; i <= 32; i++)
	{
		if (i == iLocalIndex /*|| g_iMod == MOD_NONE || *//*g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZSE*/) continue;
		pEntity2 = gEngfuncs.GetEntityByIndex(i);
		if (!pEntity2) continue;
		if (g_iMod == MOD_DM) break;

		if (vPlayer[pEntity2->index].team != vPlayer[iLocalIndex].team)
			continue;

		if (vPlayer[pEntity2->index].killtime < Hud().m_flTime)
			continue;

		if (pCvar_Name->value)
		{
			Vector pPos = vPlayer[i].info->origin;
			pPos[2] += 30.0;
			float vecScreen[2];
			if (CalcScreen(pPos, vecScreen))
			{
				if (g_iMod == MOD_NONE || g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB5 || g_iMod == MOD_ZSE)
				{
					if (!cl_teammate_tag->value)
						continue;

					if (cl_teammate_tag->value < 2 && g_iMod == MOD_ZB || g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB4 || g_iMod == MOD_ZB5 || g_iMod == MOD_ZSE)
						continue;

					float distance = (g_vecOrigin - (Vector)vPlayer[i].info->origin).Length();

					if (distance < 500.0)
					{
						m_bCanSee[i] = true;
					}
					else
					{
						if (Hud().m_flTime > flNextTraceCheck[i])
						{
							m_bCanSee[i] = false;

							flNextTraceCheck[i] = Hud().m_flTime + 0.5;

							pmtrace_t tr;

							gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, false);
							gEngfuncs.pEventAPI->EV_PushPMStates();
							gEngfuncs.pEventAPI->EV_SetSolidPlayers(iLocalIndex - 1);
							gEngfuncs.pEventAPI->EV_SetTraceHull(2);
							gEngfuncs.pEventAPI->EV_PlayerTrace(g_vecOrigin, vPlayer[i].info->origin, PM_STUDIO_BOX, -1, &tr);

							if (tr.ent > 0)
							{
								physent_t *ent = gEngfuncs.pEventAPI->EV_GetPhysent(tr.ent);
								if (ent->info == i)
									m_bCanSee[i] = true;
							}

							gEngfuncs.pEventAPI->EV_PopPMStates();


						}
					}

					if (!m_bCanSee[i])
						continue;

					Tri_Enable(GL_BLEND);
					Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

					if (distance > 1000.0)
						distance = 1000;
					vgui::surface()->DrawSetColor(255, 255, 255, 255);

					float scale = 1.0 - distance / 500.0;
					scale = scale < 0.3 ? 0.3 : scale;

					int w, h;

					vgui::surface()->DrawSetTexture(iTag[g_iTeam - 1]);
					vgui::surface()->DrawGetTextureSize(iTag[g_iTeam - 1], w, h);
					vgui::surface()->DrawTexturedRect(vecScreen[0] - w / 2 * scale, vecScreen[1], vecScreen[0] + w * scale, vecScreen[1] + h * scale);
				}
				else
				{
					hud_player_info_t hPlayer;
					gEngfuncs.pfnGetPlayerInfo(i, &hPlayer);
					g_FontBold.SetColor(255, 255, 255, 100);
					g_FontBold.SetWidth(16);
					wchar_t *name;
					name = UTF8ToUnicode(hPlayer.name);
					g_FontBold.DrawString(name, vecScreen[0] - g_FontBold.GetLen(name) / 2, vecScreen[1], 500);
				}
			}
		}
	}
	if ((g_iMod == MOD_ZB2 || g_iMod == MOD_ZB3 || g_iMod == MOD_ZB5 || g_iMod == MOD_ZSE)/* && vPlayer[gEngfuncs.GetLocalPlayer()->index].team != 2*/)
	{
		int iSupplyBox = 0;

		for (int i = 34; i<512; i++)
		{
			cl_entity_s *pEnt = gEngfuncs.GetEntityByIndex(i);
			if (pEnt && pEnt->curstate.fuser1 > Hud().m_flTime)
			{
				float vecScreen[2];
				if (pEnt->curstate.iuser1 == 1 && vPlayer[gEngfuncs.GetLocalPlayer()->index].team != 2)
				{
					iSupplyBox++;

					if (pCvar_TipIcon->value)
					{
						if (CalcScreen(pEnt->origin, vecScreen))
						{
							auto &Texture = TextureManager()[HudZombieMod3().m_iSupplyBox];
							Texture.Draw(vecScreen[0] - Texture.w() / 2, vecScreen[1] - Texture.h() / 2 - 5);

							Vector vOriDis;

							VectorSubtract(gEngfuncs.GetLocalPlayer()->origin, pEnt->origin, vOriDis);
							float distance = vOriDis.Length() / 39.37f;
							char p1[256];
							wchar_t p2[256];
							sprintf(p1, "[%dm]", (short int)distance);

							g_FontBold.C2W(p2, 255, p1);
							g_FontBold.SetColor(157, 248, 255, 255);
							g_FontBold.SetWidth(16);
							g_FontBold.DrawString(p2, int(vecScreen[0] - g_FontBold.GetLen(p2) / 2 + 1), vecScreen[1] + Texture.h() / 2 + 20 - 5, 100);

						}
					}
					gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
					if (!gConfigs.bEnableNewHud)
					{
						int fOut[2];
						Tri_Enable(GL_TEXTURE_2D);
						CalcRadarPoint(pEnt->origin, fOut[0], fOut[1], 1, 6);
						gEngfuncs.pfnSPR_Set(Hud().GetSprite(HudZombieMod3().m_iRadarItem), 133, 247, 255);
						gEngfuncs.pfnSPR_DrawAdditive(0, fOut[0], fOut[1], &Hud().GetSpriteRect(HudZombieMod3().m_iRadarItem));
						gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
					}
				}
				else if (pEnt->curstate.iuser1 == 2 && vPlayer[gEngfuncs.GetLocalPlayer()->index].team == 2 && pEnt->curstate.iuser4 == 1)
				{
					if (CalcScreen(pEnt->origin, vecScreen))
					{
						auto &Texture = TextureManager()[HudZombieMod3().m_iZombieTrap];
						Texture.Draw(vecScreen[0] - Texture.w() / 2, vecScreen[1] - Texture.h() / 2);

						Vector vOriDis;

						VectorSubtract(gEngfuncs.GetLocalPlayer()->origin, pEnt->origin, vOriDis);
						float distance = sqrt(vOriDis.x*vOriDis.x + vOriDis.y*vOriDis.y + vOriDis.z*vOriDis.z) / 40.0f;
						char p1[256];
						wchar_t p2[256];
						sprintf(p1, "[%dm]", (short int)distance);

						g_FontBold.C2W(p2, 255, p1);
						g_FontBold.SetColor(255, 203, 125, 255);
						g_FontBold.SetWidth(14);
						g_FontBold.DrawString(p2, int(vecScreen[0] - g_FontBold.GetLen(p2) / 2 + 1), vecScreen[1] + Texture.h() / 2 + 25, 1000);

					}
				}
			}
		}
	}
	gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
}