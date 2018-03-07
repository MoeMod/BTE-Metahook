#include "base.h"
#include "hud.h"
#include "TriAPI.h"
#include "exportfuncs.h"
#include "eventscripts.h"
#include "DrawTargaImage.h"
#include "weapons.h"
#include "configs.h"
#include "Fonts.h"
#include "client.h"
#include "util.h"

#include "Client/HUD/ammo.h"

int g_MoneyPosIco = 0;
int g_MoneyPosSym = 0;

float g_fCountDown = 0;
char g_sCountDownText[256];

int g_iIconC4[2];

void DisplayInitialize(void)
{	
	g_iIconC4[0] = g_Texture[Hud().m_TGA.FindTexture("resource\\icon\\c4_left_default")].iTexture;
	g_iIconC4[1] = g_Texture[Hud().m_TGA.FindTexture("resource\\icon\\c4_left_install")].iTexture;
}

void DisplayRedraw(float flTime, int iIntermission)
{
	if (gConfigs.bEnableNewHud)
		return;

	Tri_Disable( GL_BLEND );
	Tri_Disable( GL_ALPHA_TEST );
	Tri_TexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );

	if(g_fCountDown > flTime)
	{
		GL_DrawTGA(g_Texture[g_iCountBg].iTexture,255,255,255,255,20,0.4*ScreenHeight,1.4);
		g_Font.SetWidth(24);
		g_Font.SetColor(251,201,96,255);

		// Get Center -- 120?
		int iStartX = 110-g_Font.GetLen(UTF8ToUnicode(g_sCountDownText))/2;

		g_Font.DrawString(UTF8ToUnicode(g_sCountDownText),iStartX,0.4*ScreenHeight+55,1000, 1000);
		g_Font.SetWidth(64);
		int iRemain = int(g_fCountDown - flTime);
		if(iRemain < 10)
		{
			g_Font.SetColor(255,50,50,255);
		}
		char txt[6];
		sprintf(txt,"%d",iRemain);
		g_Font.DrawString(UTF8ToUnicode(txt), iRemain<10?95:80, 0.4*ScreenHeight+135, 1000, 1000);
	}
	if(g_szCurWeapon2 && g_bAlive && (g_iMod == MOD_GD/* || g_iMod == MOD_ZB4*/))
	{
		g_Font.SetColor(251,201,96,255);
		g_Font.SetWidth(24);
		g_Font.DrawString(g_szCurWeapon2, ScreenWidth - g_Font.GetLen(g_szCurWeapon2) - 18 ,ScreenHeight - 50, 1000, 1000);
	}
}

int g_bC4Flash = 0;

void EngFunc_SPR_Set(HSPRITE hSPR, int r, int g, int b)
{
	if (r == 250 && !g && !b)
		g_bC4Flash = 1;
	else
		g_bC4Flash = 0;
	return gEngfuncs.pfnSPR_Set(hSPR, r, g, b);
}

void EngFunc_SPR_DrawAdditive(int frame, int x, int y, const wrect_t *prc)
{	
	if (gConfigs.bEnableNewHud && !((x > ScreenWidth * 0.3 && x < ScreenWidth * 0.7) && (y > ScreenHeight * 0.3 && y < ScreenHeight * 0.7)))
		return;
	if (x < ScreenWidth * 0.4 && y == ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2)
		return;
	if (x > ScreenWidth * 0.75 && y == ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2)
		return;
	if (x > ScreenWidth * 0.6 && y >= ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight && (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 2 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 3 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 4 || g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6))
		return;

	if (prc)
	{
		if (g_iMod == MOD_NONE || g_iMod == MOD_ZBS || g_iMod == MOD_GHOST || !g_iMod)
		{
			if((prc->right - prc->left) == 18) //MONEY
			{
				g_MoneyPosIco = y;
				if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 && HudAmmo().m_iExtraAmmo) || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6 && HudAmmo().m_iExtraAmmo)) y -= 25;
			}
			else if((prc->bottom - prc->top) == 23) ///PLUS MINUS
			{
				g_MoneyPosSym = y;
				if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 && HudAmmo().m_iExtraAmmo) || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6 && HudAmmo().m_iExtraAmmo)) y -= 25;
			}
			else if(y == g_MoneyPosIco || y == g_MoneyPosSym)
			{
				if (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 1 || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 5 && HudAmmo().m_iExtraAmmo) || (g_iWeaponData[g_iCurrentWeapon].iAmmoDisplay == 6 && HudAmmo().m_iExtraAmmo)) y -= 25;
			}
			else if (prc->left == 0 && prc->right == 32 && prc->top == 148 && prc->bottom == 148 + 32)	// C4 icon
			{
				GL_DrawTGA(g_iIconC4[g_bC4Flash], 255, 255, 255, 255, x, y, 1.0);
				return;
			}
		}
		else
		{
			if((prc->right - prc->left) == 18) //MONEY
			{
				g_MoneyPosIco = y;
				y += 180;
			}
			else if((prc->bottom - prc->top) == 23) ///PLUS MINUS
			{
				g_MoneyPosSym = y;
				y += 180;
			}
			else if(y == g_MoneyPosIco || y == g_MoneyPosSym)
			{
				y += 180;
			}
		}
	}
	return gEngfuncs.pfnSPR_DrawAdditive(frame, x, y, prc);
}

void EngFunc_FillRGBA(int x, int y, int width, int height, int r, int g, int b, int a)
{
	if (y == ScreenHeight - Hud().m_iFontHeight - Hud().m_iFontHeight / 2)
		return;
	gEngfuncs.pfnFillRGBA(x, y, width, height, r, g, b, a);
	
}