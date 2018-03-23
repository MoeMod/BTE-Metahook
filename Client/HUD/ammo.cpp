#include "metahook.h"
#include "plugins.h"
#include "bte_const.h"
#include <pm_movevars.h>
#include <triangleapi.h>
#include "exportfuncs.h"
#include "hud.h"
#include "configs.h"
#include "eventscripts.h"
#include "display.h"
#include "util.h"
#include <weapons.h>

#include "ammo.h"
#include "zb3ui.h"

static CHudAmmo g_HudAmmo;
CHudAmmo &HudAmmo()
{
	return g_HudAmmo;
}

int WEAPON_AMMOID[31] = { -1, 9, -1, 2, 12, 5, 14, 6, 4, 13, 10, 7, 6, 4, 4, 4, 6, 10, 1, 10, 3, 5, 4, 10, 2, 11, 8, 4, 2, -1, 7 };
extern int g_iShotsFired; // ev_hldm.cpp

void CHudAmmo::Draw(float flTime)
{
	gEngfuncs.pfnDrawConsoleString(-10, -10, "!");	// Don't know why
	
	if (
		(IS_FIRSTPERSON_SPEC || g_bAlive) &&
		g_iCurrentWeapon != WEAPON_AWP &&
		g_iCurrentWeapon != WEAPON_SCOUT && 
		g_iCurrentWeapon != WEAPON_SG550 && 
		g_iCurrentWeapon != WEAPON_G3SG1 && 
		!(Hud().m_iHideHUDDisplay & HIDEWEAPON_CROSSHAIR) && 
		Hud().m_FOV > 40.0 && 
		!g_iWeaponData[g_iCurrentWeapon].iHideCrosshair
		)

		DrawCrosshair(flTime, g_iCurrentWeapon);

	if (m_flLastBuffHit > cl.time)
	{
		SPR_Set(m_hBuffHit, 255, 0, 0);
		SPR_DrawAdditive(0, ScreenWidth / 2 - m_iBuffHitWidth / 2, ScreenHeight / 2 - m_iBuffHitHeight / 2, NULL);
	}

	DrawAmmo(flTime);
	DrawExtraAmmo(flTime);

	if (g_iWeaponData[g_iCurrentWeapon].iCrosshair == 1 && g_bAlive)
	{
		int x, y;
		x = ScreenWidth / 2 - (g_iWeaponData[g_iCurrentWeapon].rcAutoaim.right - g_iWeaponData[g_iCurrentWeapon].rcAutoaim.left) / 2;
		y = ScreenHeight / 2 - (g_iWeaponData[g_iCurrentWeapon].rcAutoaim.bottom - g_iWeaponData[g_iCurrentWeapon].rcAutoaim.top) / 2;

		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
		

		if (g_iBTEWeapon == WPN_JANUS && (g_iWeaponStat == 51 || g_iWeaponStat == 52))
			gEngfuncs.pfnSPR_Set(g_iWeaponData[g_iCurrentWeapon].hAutoaim, 170, 83, 196);
		else
			gEngfuncs.pfnSPR_Set(g_iWeaponData[g_iCurrentWeapon].hAutoaim, 255, 255, 255);

		if (g_iWeaponData[g_iCurrentWeapon].rcAutoaim.right > 0 && g_iWeaponData[g_iCurrentWeapon].rcAutoaim.bottom > 0)
			gEngfuncs.pfnSPR_DrawHoles(0, x, y, &g_iWeaponData[g_iCurrentWeapon].rcAutoaim);
	}

	if (g_iShowCustomCrosshair)
		Hud().m_iHideHUDDisplay |= HIDEWEAPON_CROSSHAIR;
	else
		Hud().m_iHideHUDDisplay &= ~HIDEWEAPON_CROSSHAIR;

	if (g_iWeaponData[g_iCurrentWeapon].iCrosshair == 2 && g_bAlive && g_iShowCustomCrosshair)
	{
		int x, y;
		x = ScreenWidth / 2 - (g_iWeaponData[g_iCurrentWeapon].rcAutoaim.right - g_iWeaponData[g_iCurrentWeapon].rcAutoaim.left) / 2;
		y = ScreenHeight / 2 - (g_iWeaponData[g_iCurrentWeapon].rcAutoaim.bottom - g_iWeaponData[g_iCurrentWeapon].rcAutoaim.top) / 2;

		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);

		if (g_iBTEWeapon == WPN_JANUS && (g_iWeaponStat == 51 || g_iWeaponStat == 52))
			gEngfuncs.pfnSPR_Set(g_iWeaponData[g_iCurrentWeapon].hAutoaim, 170, 83, 196);
		else
			gEngfuncs.pfnSPR_Set(g_iWeaponData[g_iCurrentWeapon].hAutoaim, 255, 255, 255);

		if (g_iWeaponData[g_iCurrentWeapon].rcAutoaim.right > 0 && g_iWeaponData[g_iCurrentWeapon].rcAutoaim.bottom > 0)
			gEngfuncs.pfnSPR_DrawHoles(0, x, y, &g_iWeaponData[g_iCurrentWeapon].rcAutoaim);
	}
}

void CHudAmmo::DrawAmmo(float time)
{
	if ((Hud().m_iHideHUDDisplay & (HIDEWEAPON_WEAPON | HIDEWEAPON_ALL)) || gEngfuncs.IsSpectateOnly())
		return;
	if (!(Hud().m_iWeaponBits & (1 << (WEAPON_VEST))))
		return;
	if (g_iCurrentWeapon <= 0)
		return;
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoType <= 0)
		return;
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 2 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 3 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 4 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6)
		return;
	if (gConfigs.bEnableNewHud)
		return;

	int x, y;
	int r, g, b, a;
	int iFlags;

	a = (int)max(MIN_ALPHA, m_fFade);

	if (m_fFade > 0)
		m_fFade -= (Hud().m_flTimeDelta * 20);

	UnpackRGB(r, g, b, RGB_YELLOWISH);
	ScaleColors(r, g, b, a);

	int iIconWidth = g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon.right - g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon.left;
	y = ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2;

	if (g_iWeaponData[g_iCurrentWeapon].iClip < 0)
	{
		x = ScreenWidth - 4 * Hud().m_iFontWidth - iIconWidth;

		if (m_iAmmo[g_iWeaponData[g_iCurrentWeapon].iAmmoType] >= 10000)
			iFlags = DHN_DRAWZERO | DHN_5DIGITS;
		else if (m_iAmmo[g_iWeaponData[g_iCurrentWeapon].iAmmoType] >= 1000)
			iFlags = DHN_DRAWZERO | DHN_4DIGITS;
		else
			iFlags = DHN_DRAWZERO | DHN_3DIGITS;

		x = Hud().DrawHudNumber(x, y, iFlags, m_iAmmo[g_iWeaponData[g_iCurrentWeapon].iAmmoType], r, g, b);
		return;
	}
	x = ScreenWidth - 8 * Hud().m_iFontWidth - iIconWidth;

	if (g_iWeaponData[g_iCurrentWeapon].iClip >= 10000)
		iFlags = DHN_DRAWZERO | DHN_5DIGITS;
	else if (g_iWeaponData[g_iCurrentWeapon].iClip >= 1000)
		iFlags = DHN_DRAWZERO | DHN_4DIGITS;
	else
		iFlags = DHN_DRAWZERO | DHN_3DIGITS;

	x = Hud().DrawHudNumber(x, y, iFlags, g_iWeaponData[g_iCurrentWeapon].iClip, r, g, b);

	int iBarWidth = Hud().m_iFontWidth / 10;
	x += Hud().m_iFontWidth / 2;
	gEngfuncs.pfnFillRGBA(x, y, iBarWidth, Hud().m_iFontHeight, r, g, b, a);
	x += iBarWidth + Hud().m_iFontWidth / 2;

	if (m_iAmmo[g_iWeaponData[g_iCurrentWeapon].iAmmoType] >= 10000)
		iFlags = DHN_DRAWZERO | DHN_5DIGITS;
	else if (m_iAmmo[g_iWeaponData[g_iCurrentWeapon].iAmmoType] >= 1000)
		iFlags = DHN_DRAWZERO | DHN_4DIGITS;
	else
		iFlags = DHN_DRAWZERO | DHN_3DIGITS;

	Hud().DrawHudNumber(x, y, iFlags, m_iAmmo[g_iWeaponData[g_iCurrentWeapon].iAmmoType], r, g, b);
}

int DrawExtraAmmoNumber(int x, int y, int iNumber, int r, int g, int b)
{
	int k;
	int iWidth = Hud().m_iFontWidth;

	if (iNumber > -1)
	{
		if (iNumber >= 100)
		{
			k = (iNumber % 1000) / 100;
			gEngfuncs.pfnSPR_Set(Hud().GetSprite(Hud().m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(Hud().m_HUD_number_0 + k));
			x += iWidth;
		}
		else x += iWidth;

		if (iNumber >= 10)
		{
			k = (iNumber % 100) / 10;
			gEngfuncs.pfnSPR_Set(Hud().GetSprite(Hud().m_HUD_number_0 + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(Hud().m_HUD_number_0 + k));
			x += iWidth;
		}
		else x += iWidth;
		k = iNumber % 10;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(Hud().m_HUD_number_0 + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(Hud().m_HUD_number_0 + k));
		x += iWidth;
	}
	return x;
}

void CHudAmmo::DrawExtraAmmo(float time)
{
	if (gConfigs.bEnableNewHud)
		return;
	if (!g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay || !g_bAlive)
		return;
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 && !m_iExtraAmmo)
		return;

	int r, g, b;
	int x, y;
	int iIconWidth = g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon.right - g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon.left;
	y = ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2;

	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 && m_iExtraAmmo) || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6 && m_iExtraAmmo))
	{
		//y = g_MoneyPosIco;
		y = ScreenHeight - Hud().m_iFontHeight * 3 + 7;
	}
	x = ScreenWidth - 4 * Hud().m_iFontWidth - iIconWidth;


	UnpackRGB(r, g, b, RGB_LIGHTBLUE);

	if (g_iBTEWeapon == WPN_M200)
		UnpackRGB(r, g, b, RGB_YELLOWISH);

	// 画上面一排
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6 && m_iExtraAmmo))
	{
		if (m_iExtraAmmo >= 255)
		{
			x = ScreenWidth - Hud().m_iFontWidth - iIconWidth;
			x -= (m_rcInfinite.right - m_rcInfinite.left);
			SPR_Set(m_hInfinite, r, g, b);
			SPR_DrawAdditive(0, x, y, &m_rcInfinite);
			x = ScreenWidth - Hud().m_iFontWidth - iIconWidth;
		}
		else
		{
			x = DrawExtraAmmoNumber(x, y, m_iExtraAmmo, r, g, b);
		}
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
		gEngfuncs.pfnSPR_Set(g_iWeaponData[g_iCurrentWeapon].hAmmoIcon2, r, g, b);
		if (g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon2.right > 0 && g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon2.bottom > 0)
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon2);
	}

	// 画下面一排
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6)
	{
		y = ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2;
		x = ScreenWidth - 4 * Hud().m_iFontWidth - iIconWidth;
	}
	if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 3 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6)
	{
		if (g_iBTEWeapon == WPN_GAUSS && IS_ZOMBIE_MODE)
		{
			x = ScreenWidth - Hud().m_iFontWidth - iIconWidth;
			x -= (m_rcInfinite.right - m_rcInfinite.left);
			SPR_Set(m_hInfinite, r, g, b);
			SPR_DrawAdditive(0, x, y, &m_rcInfinite);
			x = ScreenWidth - Hud().m_iFontWidth - iIconWidth;
		}
		else
		{
			x = DrawExtraAmmoNumber(x, y, g_iWeaponData[g_iCurrentWeapon].iClip, r, g, b);
		}
	}
	else if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 4)
		x = DrawExtraAmmoNumber(x, y, g_iWeaponData[g_iCurrentWeapon].iClip + m_iAmmo[WEAPON_AMMOID[g_iCurrentWeapon]], r, g, b);
	else if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 2)
		x = DrawExtraAmmoNumber(x, y, m_iAmmo[WEAPON_AMMOID[g_iCurrentWeapon]], r, g, b);

	if (!(g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5))
	{
		gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
		gEngfuncs.pfnSPR_Set(g_iWeaponData[g_iCurrentWeapon].hAmmoIcon, r, g, b);
		if (g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon.right > 0 && g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon.bottom > 0)
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &g_iWeaponData[g_iCurrentWeapon].rcAmmoIcon);
	}
}

void CHudAmmo::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudAmmo::VidInit(void)
{
	m_iAlpha = 255;
	m_flLastBuffHit = m_flLastHitTime = 0.0;
	m_bNvgOn = false;
	memset(m_iAmmo, 0, sizeof(m_iAmmo));

	int iInfinite = Hud().GetSpriteIndex("infinite");

	m_hBuffHit = SPR_Load("sprites/buffhit.spr");
	m_hInfinite = Hud().GetSprite(iInfinite);
	m_iBuffHitHeight = SPR_Height(m_hBuffHit, 0);
	m_iBuffHitWidth = SPR_Width(m_hBuffHit, 0);
	m_rcInfinite = Hud().GetSpriteRect(iInfinite);
}

int CHudAmmo::DrawCrosshair(float flTime, int weaponid)
{
	int iDistance;
	int iDeltaDistance;
	int iWeaponAccuracyFlags;
	int iBarSize;
	float flCrosshairDistance;

	switch (weaponid)
	{
	case WEAPON_P228:
	case WEAPON_HEGRENADE:
	case WEAPON_SMOKEGRENADE:
	case WEAPON_FIVESEVEN:
	case WEAPON_USP:
	case WEAPON_GLOCK18:
	case WEAPON_AWP:
	case WEAPON_FLASHBANG:
	case WEAPON_DEAGLE:
	{
		iDistance = 8;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_MP5NAVY:
	{
		 iDistance = 6;
		 iDeltaDistance = 2;
		 break;
	}

	case WEAPON_M3:
	{
		iDistance = 8;
		iDeltaDistance = 6;
		break;
	}

	case WEAPON_G3SG1:
	{
		iDistance = 6;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_AK47:
	{
		iDistance = 4;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_TMP:
	case WEAPON_KNIFE:
	case WEAPON_P90:
	{
		iDistance = 7;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_XM1014:
	{
		iDistance = 9;
		iDeltaDistance = 4;
		break;
	}

	case WEAPON_MAC10:
	{
		iDistance = 9;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_AUG:
	{
		iDistance = 3;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_C4:
	case WEAPON_UMP45:
	case WEAPON_M249:
	{
		iDistance = 6;
		iDeltaDistance = 3;
		break;
	}

	case WEAPON_SCOUT:
	case WEAPON_SG550:
	case WEAPON_SG552:
	{
		iDistance = 5;
		iDeltaDistance = 3;
		break;
	}

	default:
	{
		iDistance = 4;
		iDeltaDistance = 3;
		break;
	}
	}

	iWeaponAccuracyFlags = GetWeaponAccuracyFlags(weaponid);

	if (iWeaponAccuracyFlags != 0 && cl_dynamiccrosshair && cl_dynamiccrosshair->value != 0.0)
	{
		if ((g_iPlayerFlags & FL_ONGROUND) || !(iWeaponAccuracyFlags & 1))
		{
			if ((g_iPlayerFlags & FL_DUCKING) && (iWeaponAccuracyFlags & 4))
			{
				iDistance *= 0.5;
			}
			else
			{
				float flLimitSpeed;

				switch (weaponid)
				{
				case WEAPON_MAC10:
				case WEAPON_SG550:
				case WEAPON_GALIL:
				case WEAPON_MP5NAVY:
				case WEAPON_M3:
				case WEAPON_DEAGLE:
				case WEAPON_SG552:
				{
					flLimitSpeed = 140;
					break;
				}

				case WEAPON_KNIFE:
				{
					flLimitSpeed = 170;
					break;
				}

				default:
				{
					flLimitSpeed = 0;
					break;
				}
				}

				if (g_flPlayerSpeed > flLimitSpeed && (iWeaponAccuracyFlags & 2))
					iDistance *= 1.5;
			}
		}
		else
			iDistance *= 2;

		if (iWeaponAccuracyFlags & 8)
			iDistance *= 1.4;

		if (iWeaponAccuracyFlags & 0x10)
			iDistance *= 1.4;
	}

	if (g_iShotsFired > m_iAmmoLastCheck)
	{
		m_flCrosshairDistance += iDeltaDistance;
		m_iAlpha -= 40;

		if (m_flCrosshairDistance > 15)
			m_flCrosshairDistance = 15;

		if (m_iAlpha < 120)
			m_iAlpha = 120;
	}
	else
	{
		m_flCrosshairDistance -= (0.013 * m_flCrosshairDistance) + 0.1;
		m_iAlpha += 2;
	}

	if (g_iShotsFired > 600)
		g_iShotsFired = 1;

	m_iAmmoLastCheck = g_iShotsFired;

	if (iDistance > m_flCrosshairDistance)
		m_flCrosshairDistance = iDistance;

	if (m_iAlpha > 255)
		m_iAlpha = 255;

	iBarSize = (int)((m_flCrosshairDistance - (float)iDistance) * 0.5) + 5;

	if (Hud().m_flTime > m_flLastCalcTime + 1)
	{
		CalculateCrosshairColor();
		CalculateCrosshairDrawMode();
		CalculateCrosshairSize();

		m_flLastCalcTime = Hud().m_flTime;
	}
	else
	{
		m_R = m_cvarR;
		m_G = m_cvarG;
		m_B = m_cvarB;
	}

	if (flTime < m_flLastHitTime + 0.5 && m_flLastHitTime)
	{
		if (m_flLastHitTime + 0.5 - flTime < 0.15)
		{
			m_R = m_cvarR + (255 - m_cvarR) * (m_flLastHitTime + 0.5 - flTime) / 0.15;
			m_G = m_cvarG + (5 - m_cvarG) * (m_flLastHitTime + 0.5 - flTime) / 0.15;
			m_B = m_cvarB + (5 - m_cvarB) * (m_flLastHitTime + 0.5 - flTime) / 0.15;
		}
		else
		{
			m_R = 255;
			m_G = 5;
			m_B = 5;
		}
	}

	flCrosshairDistance = m_flCrosshairDistance;

	if (m_iCrosshairScaleBase != ScreenWidth)
	{
		flCrosshairDistance *= (float)(ScreenWidth) / m_iCrosshairScaleBase;
		iBarSize = (float)(ScreenWidth * iBarSize) / m_iCrosshairScaleBase;
	}

	if (m_bNvgOn || HudZombieMod3().m_iNVG)
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, false, 250, 50, 50, m_iAlpha);
	else if (g_iBTEWeapon == WPN_JANUS && (g_iWeaponStat == 51 || g_iWeaponStat == 52))
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, m_bAdditive, 170, 83, 196, m_iAlpha);
	else
		DrawCrosshairEx(flTime, weaponid, iBarSize, flCrosshairDistance, m_bAdditive, m_R, m_G, m_B, m_iAlpha);

	return 1;
}
void CHudAmmo::SetNvgOn(bool bNvg)
{
	m_bNvgOn = bNvg;
}

int CHudAmmo::GetWeaponAccuracyFlags(int iWeaponID)
{
	int flags;

	switch (iWeaponID)
	{
	case WEAPON_USP:
	{
		flags = (g_iWeaponFlags & 1) < 1 ? 7 : 15;
		break;
	}

	case WEAPON_GLOCK18:
	{
		flags = (g_iWeaponFlags & 2) < 1 ? 7 : 23;
		break;
	}

	case WEAPON_M4A1:
	{
		flags = (g_iWeaponFlags & 4) < 1 ? 3 : 11;
		break;
	}

	case WEAPON_MAC10:
	case WEAPON_UMP45:
	case WEAPON_MP5NAVY:
	case WEAPON_TMP:
	{
		flags = 1;
		break;
	}

	case WEAPON_AUG:
	case WEAPON_GALIL:
	case WEAPON_M249:
	case WEAPON_SG552:
	case WEAPON_AK47:
	case WEAPON_P90:
	{
		flags = 3;
		break;
	}

	case WEAPON_P228:
	case WEAPON_FIVESEVEN:
	case WEAPON_DEAGLE:
	{
		flags = 7;
		break;
	}

	case WEAPON_FAMAS:
	{
		flags = (g_iWeaponFlags & 0x10) < 1 ? 3 : 19;
		break;
	}

	default:
	{
		flags = 0;
		break;
	}
	}

	return flags;
}




int CHudAmmo::DrawCrosshairEx(float flTime, int weaponid, int iBarSize, float flCrosshairDistance, bool bAdditive, int r, int g, int b, int a)
{
	bool bDrawPoint = false;
	bool bDrawCircle = false;
	bool bDrawCross = false;

	void(*pfnFillRGBA)(int x, int y, int w, int h, int r, int g, int b, int a) = (bAdditive == false) ? gEngfuncs.pfnFillRGBABlend : gEngfuncs.pfnFillRGBA;

	switch ((int)cl_crosshair_type->value)
	{
	case 1:
	{
		bDrawPoint = true;
		bDrawCross = true;
		break;
	}

	case 2:
	{
		bDrawPoint = true;
		bDrawCircle = true;
		break;
	}

	case 3:
	{
		bDrawPoint = true;
		bDrawCircle = true;
		bDrawCross = true;
		break;
	}

	case 4:
	{
		bDrawPoint = true;
		break;
	}

	default:
	{
		bDrawCross = true;
		break;
	}
	}

	if (bDrawCircle)
	{
#if 1
		float radius = (iBarSize / 2) + flCrosshairDistance;
#if 0
		switch (m_iCrosshairScaleBase)
		{
		case 640: radius += 0.1; break;
		case 800: radius += 0.8; break;
		case 1024: radius += 0.5; break;
		}
#endif
		int count = (int)((cos(0.7853981852531433) * radius) + 0.5);

		for (int i = 0; i < count; i++)
		{
			int size = sqrt((radius * radius) - (float)(i * i));

			pfnFillRGBA((ScreenWidth / 2) + i, (ScreenHeight / 2) + size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) + i, (ScreenHeight / 2) - size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - i, (ScreenHeight / 2) + size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - i, (ScreenHeight / 2) - size, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) + size, (ScreenHeight / 2) + i, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) + size, (ScreenHeight / 2) - i, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - size, (ScreenHeight / 2) + i, 1, 1, r, g, b, a);
			pfnFillRGBA((ScreenWidth / 2) - size, (ScreenHeight / 2) - i, 1, 1, r, g, b, a);
		}
#else
		float radius = flCrosshairDistance * 1.1 + (iBarSize / 2);
		int count = radius * 3;

		qglDisable(GL_TEXTURE_2D);
		qglEnable(GL_BLEND);
		qglTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		if (m_bAdditive == false)
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		else
			qglBlendFunc(GL_SRC_ALPHA, GL_ONE);

		qglColor4f(r / 255.0, g / 255.0, b / 255.0, a / 255.0);
		qglBegin(GL_LINE_LOOP);

		for (int i = 0; i < count; i++)
		{
			float angles = 2 * M_PI / count * i;
			qglVertex2f((ScreenWidth / 2) + radius * cos(angles), (ScreenHeight / 2) + radius * sin(angles));
		}

		qglEnd();
		qglColor3f(1, 1, 1);
		qglEnable(GL_TEXTURE_2D);
		qglDisable(GL_BLEND);
#endif
	}

	if (bDrawPoint)
		pfnFillRGBA((ScreenWidth / 2) - 1, (ScreenHeight / 2) - 1, 3, 3, r, g, b, a);

	if (bDrawCross)
	{
		pfnFillRGBA((ScreenWidth / 2) - (int)flCrosshairDistance - iBarSize + 1, ScreenHeight / 2, iBarSize, 1, r, g, b, a);
		pfnFillRGBA((ScreenWidth / 2) + (int)flCrosshairDistance, ScreenHeight / 2, iBarSize, 1, r, g, b, a);
		pfnFillRGBA(ScreenWidth / 2, (ScreenHeight / 2) - (int)flCrosshairDistance - iBarSize + 1, 1, iBarSize, r, g, b, a);
		pfnFillRGBA(ScreenWidth / 2, (ScreenHeight / 2) + (int)flCrosshairDistance, 1, iBarSize, r, g, b, a);
	}

	return 1;
}

void CHudAmmo::CalculateCrosshairDrawMode(void)
{
	float value = cl_crosshair_translucent->value;

	if (value == 0)
		m_bAdditive = false;
	else if (value == 1)
		m_bAdditive = true;
	else
		gEngfuncs.Con_Printf("usage: cl_crosshair_translucent <1|0>\n");
}

void CHudAmmo::CalculateCrosshairSize(void)
{
	char *value = cl_crosshair_size->string;

	if (!value)
		return;

	int size = atoi(value);

	if (size)
	{
		if (size > 3)
			size = -1;
	}
	else
	{
		if (strcmp(value, "0"))
			size = -1;
	}

	if (!stricmp(value, "auto"))
		size = 0;
	else if (!stricmp(value, "small"))
		size = 1;
	else if (!stricmp(value, "medium"))
		size = 2;
	else if (!stricmp(value, "large"))
		size = 3;

	if (size == -1)
	{
		gEngfuncs.Con_Printf("usage: cl_crosshair_size <auto|small|medium|large>\n");
		return;
	}

	switch (size)
	{
	case 0:
	{
		if (ScreenWidth < 640)
			m_iCrosshairScaleBase = 1024;
		else if (ScreenWidth < 1024)
			m_iCrosshairScaleBase = 800;
		else
			m_iCrosshairScaleBase = 640;

		break;
	}

	case 1:
	{
		m_iCrosshairScaleBase = 1024;
		break;
	}

	case 2:
	{
		m_iCrosshairScaleBase = 800;
		break;
	}

	case 3:
	{
		m_iCrosshairScaleBase = 640;
		break;
	}
	}
}


static char s_shared_token[1500];

char *SharedParse(char *data)
{
	int len = 0, c;
	s_shared_token[0] = '\0';

	if (!data)
		return NULL;

skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;

		data++;
	}

	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;

		goto skipwhite;
	}

	if (c == '\"')
	{
		data++;

		while (1)
		{
			c = *data++;

			if (c == '\"' || !c)
			{
				s_shared_token[len] = '\0';
				return data;
			}

			s_shared_token[len++] = c;
		}
	}

	if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
	{
		s_shared_token[len++] = c;
		s_shared_token[len] = '\0';
		return data + 1;
	}

	do
	{
		s_shared_token[len] = c;
		data++;
		len++;
		c = *data;

		if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
			break;
	} while (c > 32);

	s_shared_token[len] = '\0';
	return data;
}

const char *SharedParse(const char *data)
{
	int len = 0, c;
	s_shared_token[0] = '\0';

	if (!data)
		return NULL;

skipwhite:
	while ((c = *data) <= ' ')
	{
		if (c == 0)
			return NULL;

		data++;
	}

	if (c == '/' && data[1] == '/')
	{
		while (*data && *data != '\n')
			data++;

		goto skipwhite;
	}

	if (c == '\"')
	{
		data++;

		while (1)
		{
			c = *data++;

			if (c == '\"' || !c)
			{
				s_shared_token[len] = '\0';
				return data;
			}

			s_shared_token[len++] = c;
		}
	}

	if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
	{
		s_shared_token[len++] = c;
		s_shared_token[len] = '\0';
		return data + 1;
	}

	do
	{
		s_shared_token[len] = c;
		data++;
		len++;
		c = *data;

		if (c == '{' || c == '}' || c == ')' || c == '(' || c == '\'' || c == ',')
			break;
	} while (c > 32);

	s_shared_token[len] = '\0';
	return data;
}

char *SharedVarArgs(char *format, ...)
{
	va_list argptr;
	const int BufLen = 1024;
	const int NumBuffers = 4;
	static char string[NumBuffers][BufLen];
	static int curstring = 0;

	curstring = (curstring + 1) % NumBuffers;

	va_start(argptr, format);
	_vsnprintf(string[curstring], BufLen, format, argptr);
	va_end(argptr);

	return string[curstring];
}

char *SharedGetToken(void)
{
	return s_shared_token;
}


void CHudAmmo::CalculateCrosshairColor(void)
{
#ifdef TIMETEST_CALCCROSSHAIRCOLOR
	float startTime = gPerformanceCounter.GetCurTime();
#endif
	char *value = cl_crosshair_color->string;

	if (value && strcmp(value, m_szLastCrosshairColor))
	{
		int cvarR, cvarG, cvarB;
		char *token;
		char *data = value;

		data = SharedParse(data);
		token = SharedGetToken();

		if (token)
		{
			cvarR = atoi(token);

			data = SharedParse(data);
			token = SharedGetToken();

			if (token)
			{
				cvarG = atoi(token);

				data = SharedParse(data);
				token = SharedGetToken();

				if (token)
				{
					cvarB = atoi(token);

					if (m_cvarR != cvarR || m_cvarG != cvarG || m_cvarB != cvarB)
					{
						int r, g, b;

						r = min(max(0, cvarR), 255);
						g = min(max(0, cvarG), 255);
						b = min(max(0, cvarB), 255);

						m_R = r;
						m_G = g;
						m_B = b;
						m_cvarR = cvarR;
						m_cvarG = cvarG;
						m_cvarB = cvarB;
					}

					strcpy(m_szLastCrosshairColor, value);
				}
			}
		}
	}

#ifdef TIMETEST_CALCCROSSHAIRCOLOR
	gEngfuncs.Con_Printf("TimeTest CHudAmmo::CalculateCrosshairColor take %.1f sec\n", gPerformanceCounter.GetCurTime() - startTime);
#endif
}