#include "base.h"
#include "hud.h"
#include "exportfuncs.h"
#include "configs.h"
#include "msghook.h"
#include "message.h"
#include "HUD3D.h"

#include "weapons.h"
#include "Client/WeaponManager.h"

#include "Client/HUD/overview.h"
#include "Client/HUD/ZombieSelect.h"
#include "Client/HUD/DrawTABPanel.h"
#include "Client/HUD/hudmenu.h"

void(*pCommandImpulse)(void);
void(*pCommandInScoreDown)(void);
void(*pCommandInScoreUp)(void);
void(*pCommandInForwardUp)(void);
void(*pCommandInForwardDown)(void);
void(*pCommandInJumpDown)(void);
void(*pCommandInDuckDown)(void);
void(*pCommandInDuckUp)(void);
void(*pCommandSlot1)(void);
void(*pCommandSlot2)(void);
void(*pCommandSlot3)(void);
void(*pCommandSlot4)(void);
void(*pCommandSlot5)(void);
void(*pCommandSlot6)(void);
void(*pCommandSlot7)(void);
void(*pCommandSlot8)(void);
void(*pCommandSlot9)(void);
void(*pCommandSlot10)(void);
void(*pCommandNvg)(void);

void(*pCommandNextWeapon)(void);
void(*pCommandPrevWeapon)(void);

int	Engfunc_ClientCmd(char *szCmdString)
{
	if (strstr(szCmdString, "menuselect"))
	{
		g_Menu_Drawing = 0;
	}
	else if (strstr(szCmdString, ".mgui"))
	{
		//MGUI_ReadFromResourceFile(szCmdString);
		return 0;
	}

	return gEngfuncs.pfnClientCmd(szCmdString);
}

void Engfunc_WeaponAnim(int sequence, int body)
{
	if (g_iBTEWeapon == WPN_INFINITYEX)
	{
		if (g_iButton) return;
	}
	else if (g_iBTEWeapon == WPN_M200)
	{
		if (sequence >0 && sequence < 4)
		{
			if (g_iWeaponData[g_iWeapon].iClip <= 1)
				return gEventAPI.EV_WeaponAnimation(3, body);
			else
				return gEventAPI.EV_WeaponAnimation(1, body);
		}
	}
	/*
	else if (g_iBTEWeapon == WPN_JANUS5)
	{
		if (sequence >= 3 && sequence <= 5)
		{
			if (g_iWeaponStat == 51)
				return gEventAPI.EV_WeaponAnimation(4, body);

			if (g_iWeaponStat == 52)
				return gEventAPI.EV_WeaponAnimation(8 + gEngfuncs.pfnRandomLong(0, 2), body);

			return gEventAPI.EV_WeaponAnimation(3, body);
		}
	}
	else if (g_iBTEWeapon == WPN_JANUS7)
	{
		if (sequence >= 1 && sequence <= 2)
		{
			if (g_iWeaponStat == 51)
				return gEventAPI.EV_WeaponAnimation(5, body);

			if (g_iWeaponStat == 52)
				return gEventAPI.EV_WeaponAnimation(gEngfuncs.pfnRandomLong(9, 10), body);

			return gEventAPI.EV_WeaponAnimation(3, gEngfuncs.pfnRandomLong(3, 4));
		}
	}
	*/

	g_iWeaponAnim = sequence;

	return gEventAPI.EV_WeaponAnimation(sequence, body);
}

void CmdFunc_InScoreDown(void)
{
	if (pCvar_DrawPanel->value<1) return pCommandInScoreDown();
	HudTABBoard().SetCanDraw(true);
	
	//if(g_iMod!= MOD_NONE && g_iMod!= MOD_TDM && g_iMod!= MOD_DM && g_iMod!= MOD_GD && g_iMod!= MOD_ZB && g_iMod!= MOD_ZB3 && g_iMod!= MOD_ZB4 && g_iMod!= MOD_ZBS) return pCommandInScoreDown();
	return;
}
void CmdFunc_InScoreUp(void)
{
	if (pCvar_DrawPanel->value<1) return pCommandInScoreUp();
	HudTABBoard().SetCanDraw(false);
	//if(g_iMod!= MOD_NONE && g_iMod!= MOD_TDM && g_iMod!= MOD_DM && g_iMod!= MOD_GD&& g_iMod!= MOD_ZB && g_iMod!= MOD_ZB3 && g_iMod!= MOD_ZB4 && g_iMod!= MOD_ZBS) return pCommandInScoreUp();
	return;
}
void CmdFunc_Impulse(void)
{
	if (gEngfuncs.Cmd_Argc() <= 3 || strncmp(gEngfuncs.Cmd_Argv(1), "[MetaHook]", 5))
	{
		pCommandImpulse();
		return;
	}

	if (!strcmp(gEngfuncs.Cmd_Argv(2), "player"))
	{
		if (!strcmp(gEngfuncs.Cmd_Argv(3), "join"))
			gEngfuncs.pfnServerCmd("#MetaHook join");

		return;
	}
}
void CmdFunc_InJumpDown()
{
	if (g_iHookVAngle && g_bAlive && g_iBTEWeapon == WPN_M2)
		return;
	else
		pCommandInJumpDown();
}
void CmdFunc_InDuckDown()
{
	if (g_iHookVAngle && g_bAlive && g_iBTEWeapon == WPN_M2)
		return;
	else
		pCommandInDuckDown();
}
void CmdFunc_InDuckUp()
{
	if (g_iHookVAngle && g_bAlive && g_iBTEWeapon == WPN_M2)
		return;
	else
		pCommandInDuckUp();
}

float g_flLastForwardUp;
float g_flLastForwardDown;

void CmdFunc_InForwardDown()
{
	if (Hud().m_flTime <= g_flLastForwardUp + 0.1)
	{
		if (g_iMod == MOD_ZB4)
			gEngfuncs.pfnClientCmd("+dash");
	}
	g_flLastForwardDown = Hud().m_flTime;

	pCommandInForwardDown();
}
void CmdFunc_InForwardUp()
{
	if (Hud().m_flTime >= g_flLastForwardDown + 0.1)
	{
		if (g_iMod == MOD_ZB4)
			gEngfuncs.pfnClientCmd("-dash");
	}
	g_flLastForwardUp = Hud().m_flTime;

	pCommandInForwardUp();
}

void CmdFunc_Slot1()
{
	if (HudZBSelect().m_bCanDraw)
	{
		if (GetKeyState(VK_SHIFT) < 0)
			HudZBSelect().Select(10);
		else
			HudZBSelect().Select(1);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(1);
		return;
	}

	pCommandSlot1();
}

void CmdFunc_Slot2()
{
	if (HudZBSelect().m_bCanDraw)
	{
		if (GetKeyState(VK_SHIFT) < 0)
			HudZBSelect().Select(11);
		else
			HudZBSelect().Select(2);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(2);
		return;
	}

	pCommandSlot2();
}

void CmdFunc_Slot3()
{
	if (HudZBSelect().m_bCanDraw)
	{
		if (GetKeyState(VK_SHIFT) < 0)
			HudZBSelect().Select(12);
		else
			HudZBSelect().Select(3);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(3);
		return;
	}

	pCommandSlot3();
}

void CmdFunc_Slot4()
{
	if (HudZBSelect().m_bCanDraw)
	{
		if (GetKeyState(VK_SHIFT) < 0)
			HudZBSelect().Select(13);
		else
			HudZBSelect().Select(4);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(4);
		return;
	}

	pCommandSlot4();
}

void CmdFunc_Slot5()
{
	if (HudZBSelect().m_bCanDraw)
	{
		HudZBSelect().Select(5);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(5);
		return;
	}

	if (g_iMod == MOD_ZB3)
	{
		// Support for previous server ( NO LONGER USED )
		// More Info -> zb3ui
		gEngfuncs.pfnClientCmd("speedup");
		gEngfuncs.pfnClientCmd("zbskill");

		return;
	}

	if (g_iMod == MOD_ZB4)
	{
		gEngfuncs.pfnClientCmd("z4_kick");
		return;
	}

	if (g_iMod == MOD_ZB5)
	{
		if (g_iTeam == TEAM_TERRORIST)
			gEngfuncs.pfnClientCmd("hms_supply");
		else if (g_iTeam == TEAM_CT)
			gEngfuncs.pfnClientCmd("z5_zbskill");

		return;
	}

	if (g_iMod == MOD_ZSE)
	{
		gEngfuncs.pfnClientCmd("zbskill");
		return;
	}

	pCommandSlot5();
}

void CmdFunc_Slot6()
{
	if (HudZBSelect().m_bCanDraw)
	{
		HudZBSelect().Select(6);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(6);
		return;
	}

	if (g_iMod == MOD_ZB3)
	{
		// Support for previous server ( NO LONGER USED )
		// More Info -> zb3ui
		gEngfuncs.pfnClientCmd("accshoot");
		return;
	}

	if (g_iMod == MOD_ZB4)
	{
		gEngfuncs.pfnClientCmd("z4_accshoot");
		return;
	}

	if (g_iMod == MOD_ZB5 && g_iTeam == TEAM_TERRORIST)
	{
		gEngfuncs.pfnClientCmd("hms_atkup");
		return;
	}

	if (g_iMod == MOD_ZB5 && g_iTeam == TEAM_CT)
	{
		gEngfuncs.pfnClientCmd("zbskill");

		return;
	}

	pCommandSlot6();
}

void CmdFunc_Slot7()
{
	if (HudZBSelect().m_bCanDraw)
	{
		HudZBSelect().Select(7);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(7);
		return;
	}
	if (g_iMod == MOD_ZB5)
	{
		gEngfuncs.pfnClientCmd("hms_heal");
		return;
	}

	pCommandSlot7();
}

void CmdFunc_Slot8()
{
	if (HudZBSelect().m_bCanDraw)
	{
		HudZBSelect().Select(8);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(8);
		return;
	}

	pCommandSlot8();
}

void CmdFunc_Slot9()
{
	if (HudZBSelect().m_bCanDraw)
	{
		HudZBSelect().Select(9);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(9);
		return;
	}

	pCommandSlot9();
}

void CmdFunc_Slot10()
{
	if (HudZBSelect().m_bCanDraw)
	{
		HudZBSelect().Select(0);
		return;
	}

	if (HudMenu().m_bCanDraw)
	{
		HudMenu().Select(0);
		return;
	}

	pCommandSlot10();
}

void CmdFunc_Nvg()
{
	if (g_iMod == MOD_ZSE)
	{
		if (g_isZombie)
		{
			if (g_iZBNvg)
			{
				g_iZBNvg = 0;
				gEngfuncs.pfnClientCmd("spk sounds/items/nvg_off.wav");
				gEngfuncs.pfnClientCmd("nvgoff");
			}
			else
			{
				g_iZBNvg = 1;
				gEngfuncs.pfnClientCmd("spk sounds/items/nvg_on.wav");
				gEngfuncs.pfnClientCmd("nvgon");
			}
		}

		return;
	}

	pCommandNvg();
}

void CmdFunc_NextWeapon()
{
	if (!hud_fastswitch_wheel->value)
	{
		pCommandNextWeapon();
		return;
	}

	pCommandNextWeapon();
	gEngfuncs.pfnClientCmd("+attack;wait;-attack");
}

void CmdFunc_PrevWeapon()
{
	if (!hud_fastswitch_wheel->value)
	{
		pCommandPrevWeapon();
		return;
	}

	pCommandPrevWeapon();
	gEngfuncs.pfnClientCmd("+attack;wait;-attack");
}

int EngFunc_AddCommand(char *szCmds, void(*pFunction)(void))
{
	if (!strcmp(szCmds, "slot1"))
	{
		pCommandSlot1 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot1);
		return 1;
	}
	else if (!strcmp(szCmds, "slot2"))
	{
		pCommandSlot2 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot2);
		return 1;
	}
	else if (!strcmp(szCmds, "slot3"))
	{
		pCommandSlot3 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot3);
		return 1;
	}
	else if (!strcmp(szCmds, "slot4"))
	{
		pCommandSlot4 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot4);
		return 1;
	}
	else if (!strcmp(szCmds, "slot5"))
	{
		pCommandSlot5 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot5);
		return 1;
	}
	else if (!strcmp(szCmds, "slot6"))
	{
		pCommandSlot6 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot6);
		return 1;
	}
	else if (!strcmp(szCmds, "slot7"))
	{
		pCommandSlot7 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot7);
		return 1;
	}
	else if (!strcmp(szCmds, "slot8"))
	{
		pCommandSlot8 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot8);
		return 1;
	}
	else if (!strcmp(szCmds, "slot9"))
	{
		pCommandSlot9 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot9);
		return 1;
	}
	else if (!strcmp(szCmds, "slot10"))
	{
		pCommandSlot10 = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Slot10);
		return 1;
	}
	else if (!strcmp(szCmds, "impulse"))
	{
		pCommandImpulse = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Impulse);
		return 1;
	}
	else if (!strcmp(szCmds, "+score") || !strcmp(szCmds, "+showscores"))
	{
		pCommandInScoreDown = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InScoreDown);
		return 1;
	}
	else if (!strcmp(szCmds, "-score") || !strcmp(szCmds, "-showscores"))
	{
		pCommandInScoreUp = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InScoreUp);
		return 1;
	}
	else if (!strcmp(szCmds, "+jump"))
	{
		pCommandInJumpDown = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InJumpDown);
		return 1;
	}
	/*else if(!strcmp(szCmds, "+duck"))
	{
	pCommandInDuckDown = pFunction;
	gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InDuckDown);
	return 1;
	}*/
	else if (!strcmp(szCmds, "-duck"))
	{
		pCommandInDuckUp = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InDuckUp);
		return 1;
	}

	else if (!strcmp(szCmds, "+forward"))
	{
		pCommandInForwardDown = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InForwardDown);
		return 1;
	}
	else if (!strcmp(szCmds, "-forward"))
	{
		pCommandInForwardUp = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_InForwardUp);
		return 1;
	}
	else if (!strcmp(szCmds, "nightvision"))
	{
		pCommandNvg = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_Nvg);
		return 1;
	}
	else if (!strcmp(szCmds, "invnext"))
	{
		pCommandNextWeapon = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_NextWeapon);
		return 1;
	}
	else if (!strcmp(szCmds, "invprev"))
	{
		pCommandPrevWeapon = pFunction;
		gEngfuncs.pfnAddCommand(szCmds, CmdFunc_PrevWeapon);
		return 1;
	}

	return gEngfuncs.pfnAddCommand(szCmds, pFunction);
}
struct model_s *Engfunc_LoadMapSprite(const char *pszPath)
{
	g_flLastForwardDown = g_flLastForwardUp = 0.0f;

	char szPath[64];
	strcpy(szPath, pszPath);

	//HudTABBoard().SetBg(szPath);
	gHud3D.SetMapBg(szPath);
	return HudOverview().m_pSprite = gEngfuncs.LoadMapSprite(pszPath);
}

byte *Engfunc_COM_LoadFile(char *pszPath, int iUseHunk, int *piLength)
{
	byte *pbResult = gEngfuncs.COM_LoadFile(pszPath, iUseHunk, piLength);

	if (!pbResult)
		return NULL;

	if (!strncmp(pszPath, "overviews/", 10))
		HudOverview().m_bCanCheck = true;

	return pbResult;
}

char *Engfunc_COM_ParseFile(char *pszData, char *pszToken)
{
	static char szToken[1024];
	char *pszFile, *pszResult = gEngfuncs.COM_ParseFile(pszData, pszToken);

	if (!HudOverview().m_bCanCheck)
		return pszResult;

	if (!_stricmp(pszToken, "zoom"))
	{
		gEngfuncs.COM_ParseFile(pszResult, szToken);
		HudOverview().m_flZoom = (float)atof(szToken);
	}
	else if (!_stricmp(pszToken, "origin"))
	{
		pszFile = gEngfuncs.COM_ParseFile(pszResult, szToken);
		HudOverview().m_vecOrigin.x = (float)atof(szToken);
		pszFile = gEngfuncs.COM_ParseFile(pszFile, szToken);
		HudOverview().m_vecOrigin.y = (float)atof(szToken);
	}
	else if (!_stricmp(pszToken, "rotated"))
	{
		gEngfuncs.COM_ParseFile(pszResult, szToken);
		HudOverview().m_bIsRotate = atoi(szToken) != FALSE;
	}
	else if (!_stricmp(pszToken, "height"))
	{
		pszFile = gEngfuncs.COM_ParseFile(pszResult, szToken);
		HudOverview().m_iHeight = (float)atof(szToken);
	}

	return pszResult;
}
void Engfunc_COM_FreeFile(void *pBuffer)
{
	HudOverview().m_bCanCheck = false;
	gEngfuncs.COM_FreeFile(pBuffer);
}
client_sprite_t *EngFunc_SPR_GetList(char *psz, int *piCount)
{
	if (!strcmp(psz, "sprites/hud.txt"))
		return gEngfuncs.pfnSPR_GetList(psz, piCount);

	return LoadWeaponSprite(psz, piCount);
}
HSPRITE	Engfunc_SPR_Load(const char *pszSprite)
{
	HSPRITE hResult = gEngfuncs.pfnSPR_Load(pszSprite);

	if (!strcmp(pszSprite, "sprites/camera.spr"))
	{
		HudOverview().m_pCamera = (model_t *)gEngfuncs.GetSpritePointer(hResult);
		HudOverview().m_pInfection = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/iinfection.spr"));
		HudOverview().m_pRadio = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/iradio.spr"));
		HudOverview().m_pBackPack = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/ibackpack.spr"));
		HudOverview().m_pHostage = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/ihostage.spr"));

		HudOverview().m_pC4 = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/ic4.spr"));
		HudOverview().m_pVIP = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/iplayervip.spr"));
		HudOverview().m_rgpPlayer[2] = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/iplayerc4.spr"));

		HudTABBoard().m_rgpPlayerDead[0] = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/ideadblue.spr"));
		HudTABBoard().m_rgpPlayerDead[1] = (model_t *)gEngfuncs.GetSpritePointer(gEngfuncs.pfnSPR_Load("sprites/ideadred.spr"));
	}
	else if (!strcmp(pszSprite, "sprites/iplayerred.spr"))
		HudOverview().m_rgpPlayer[1] = (model_t *)gEngfuncs.GetSpritePointer(hResult);
	else if (!strcmp(pszSprite, "sprites/iplayerblue.spr"))
		HudOverview().m_rgpPlayer[0] = (model_t *)gEngfuncs.GetSpritePointer(hResult);
	else if (!strcmp(pszSprite, "sprites/iplayerdead.spr"))
		HudOverview().m_pDeath = (model_t *)gEngfuncs.GetSpritePointer(hResult);

	return hResult;
}
void Engfunc_BloodStream(float * org, float * dir, int pcolor, int speed)
{
	return /*gEfxAPI.R_BloodStream(org, dir, pcolor, speed)*/;
}

TEMPENTITY *CL_DefaultSprite(const vec3_t pos, model_t *sprite, float framerate);

void Engfunc_BloodSprite(float * org, int colorindex, int modelIndex, int modelIndex2, float size)
{
	if (gConfigs.bEnableBlood)
		return gEfxAPI.R_BloodSprite(org, colorindex == 180 ? 247 : colorindex, modelIndex, modelIndex2, size);

	if (colorindex == 247)
	{
		int model = gEngfuncs.pEventAPI->EV_FindModelIndex("sprites/sparkeffect2.spr");
		TEMPENTITY *te = gEngfuncs.pEfxAPI->R_DefaultSprite(org, model, 0);
		if (te)
		{
			te->entity.curstate.rendermode = kRenderTransAdd;
			te->entity.curstate.renderamt = 120;
			te->entity.curstate.scale = 0.4;
		}

		return;
	}
	return gEfxAPI.R_BloodSprite(org, colorindex, modelIndex, modelIndex2, size);
}

void Engfunc_PlaySound(int ent, float *origin, int channel, const char *sample, float volume, float attenuation, int fFlags, int pitch)
{
	/*if(ent<33&&ent>0)
	{
	if(strstr(sample,"weapons")) return;
	}
	else */return gEventAPI.EV_PlaySound(ent, origin, channel, sample, volume, attenuation, fFlags, pitch);
}

#include "eventscripts.h"

int EfxAPI_DecalIndex(int id)
{
	return gEfxAPI.Draw_DecalIndex(id);
}
#if 0
int EfxAPI_DecalIndexFromName(char *name)
{
	if (g_bSfsniperShoot == 1)
		return gEfxAPI.Draw_DecalIndexFromName("{sfshot");

	return gEfxAPI.Draw_DecalIndexFromName(name);
}

void EfxAPI_BulletImpactParticles(float *pos)
{
	if (gEngfuncs.PM_PointContents(pos, NULL) == CONTENTS_SKY)
		return;

	if (g_bSfsniperShoot)
		return;

	return gEfxAPI.R_BulletImpactParticles(pos);
}

void EventAPI_PlayerTrace(float *start, float *end, int traceFlags, int ignore_pe, struct pmtrace_s *tr)
{
	//gEngfuncs.pEventAPI->EV_PlayerTrace(start, end, traceFlags, ignore_pe, tr);
}

void EfxAPI_R_StreakSplash(float *pos, float *dir, int color, int count, float speed, int velocityMin, int velocityMax)
{
	//gEngfuncs.Con_Printf("%d %d %f\n", color, count, speed);

	if (gEngfuncs.PM_PointContents(pos, NULL) == CONTENTS_SKY)
		return;

	if (g_bSfsniperShoot == 1)
		return gEfxAPI.R_StreakSplash(pos, dir, 3, 200/*count*/, speed, velocityMin, velocityMax);
	else if (g_bSfsniperShoot == 2)
		return gEfxAPI.R_StreakSplash(pos, dir, 0, 30, speed, velocityMin, velocityMax);
	else if (g_bSfsniperShoot == 3)
		return gEfxAPI.R_StreakSplash(pos, dir, 0, 150, speed, velocityMin, velocityMax);
	else if (g_bSfsniperShoot == 4)
		return gEfxAPI.R_StreakSplash(pos, dir, color, 100, speed, velocityMin, velocityMax);
	else if (g_bSfsniperShoot == 5)
		return gEfxAPI.R_StreakSplash(pos, dir, color, 300, speed, velocityMin, velocityMax);
	else if (g_bSfsniperShoot == 6)
		return gEfxAPI.R_StreakSplash(pos, dir, 0, count, speed, velocityMin, velocityMax);

	if (g_bZgShoot)
		return gEfxAPI.R_StreakSplash(pos, dir, 2, count, speed, velocityMin, velocityMax);

	if (color != 30)
		return gEfxAPI.R_StreakSplash(pos, dir, color, count, speed, velocityMin, velocityMax);

	if (cl_gunstreak->value)
		return gEfxAPI.R_StreakSplash(pos, dir, 4, count, speed, velocityMin, velocityMax);

	return gEfxAPI.R_StreakSplash(pos, dir, color, count, speed, velocityMin, velocityMax);
}
#endif
void Engfunc_PlaySoundByName(char *szSound, float volume)
{
	if (hud_fastswitch_wheel->value)
	{
		if (!strcmp(szSound, "common/wpn_hudon.wav") || !strcmp(szSound, "common/wpn_select.wav"))
			return;
	}

	return gEngfuncs.pfnPlaySoundByName(szSound, volume);
}

int DrawCharacter(int x, int y, int number, int r, int g, int b)
{
	return 0;
}

int Engfunc_DrawConsoleString(int x, int y, char *string)
{
	return gEngfuncs.pfnDrawConsoleString(x, y, string);
}

int Engfunc_VGUI2DrawCharacter(int x, int y, int ch, unsigned int font)
{
	return gEngfuncs.pfnVGUI2DrawCharacter(x, y, ch, font);
}