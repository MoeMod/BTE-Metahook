#include "metahook.h"
#include "exportfuncs.h"
#include "plugins.h"
#include "BTE_Const.h"
#include "hud.h"
#include "links.h"
#include "DrawTargaImage.h"
#include "Configs.h"

#include "gl/gl.h"
#include "triangleapi.h"
#include "TriAPI.h"

#include "Client/WeaponManager.h"

#include "Client/HUD/sfsniper.h"
#include "Client/HUD/destroyer.h"
#include "Client/HUD/buffawp.h"
#include "Client/HUD/DrawTGA.h"

int __fastcall CHudRoundTimer_Draw(void *pthis, int i, float flTime)
{
	if (gConfigs.bEnableNewHud)
		return 1;

	if (g_iMod != MOD_ZB4)
		return g_pfnCHudRoundTimer_Draw(pthis, i, flTime);

	return 1;
}

int __fastcall CHudAmmo_DrawCrosshair(void *pthis, int, float flTime, int weaponid)
{
	return 1;
}

void __fastcall CHudSayText_Draw(void *pthis, int, float flTime)
{
	if (gConfigs.bEnableNewHud)
	{
		return;
	}
	return g_pfnCHudSayText_Draw(pthis, 0, flTime);
}

extern int g_iSniperScope[4];

int __fastcall HudSniperScope_Draw(void *p, int i, float f)
{
	if (WeaponManager().GetCurWeapon().iSniperScopeType == 1 && 0.0 < Hud().m_FOV && Hud().m_FOV <= 40.0)
	{
		int iH = ScreenHeight / 592.0f * 256.0f;
		int iY = ScreenHeight / 2 - iH;
		int iX = ScreenWidth / 2 - iH;
		GL_DrawTGA2(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[0]].iTexture, iX, iY, iH, iH, 255);
		GL_DrawTGA2(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[1]].iTexture, ScreenWidth / 2, iY, iH, iH, 255);
		GL_DrawTGA2(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[2]].iTexture, iX, ScreenHeight / 2, iH, iH, 255);
		GL_DrawTGA2(g_Texture[WeaponManager().GetCurWeapon().iSniperScope[3]].iTexture, ScreenWidth / 2, ScreenHeight / 2, iH, iH, 255);

		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + iH, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + iH, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);

		/*gEngfuncs.pfnFillRGBABlend(ScreenWidth/2,0,1,ScreenHeight,0,0,0,255);
		gEngfuncs.pfnFillRGBABlend(0,ScreenHeight/2+1,ScreenWidth,1,0,0,0,255);*/
		return g_pfnHudSniperScope_Draw(p, i, f);
	}
	if (WeaponManager().GetCurWeapon().iSniperScopeType == 2 && 0.0 < Hud().m_FOV && Hud().m_FOV <= 40.0)
	{
		int iH = ScreenHeight / 588.0f * 256.0f;
		int iY = ScreenHeight / 2 - iH;
		int iX = ScreenWidth / 2 - iH;
		GL_DrawTGA2(g_Texture[g_iSniperScope[0]].iTexture, iX, iY, iH, iH, 255);
		GL_DrawTGA2(g_Texture[g_iSniperScope[1]].iTexture, ScreenWidth / 2, iY, iH, iH, 255);
		GL_DrawTGA2(g_Texture[g_iSniperScope[2]].iTexture, iX, ScreenHeight / 2, iH, iH, 255);
		GL_DrawTGA2(g_Texture[g_iSniperScope[3]].iTexture, ScreenWidth / 2, ScreenHeight / 2, iH, iH, 255);

		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + iH, 0, ScreenWidth / 2 - iH, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + iH, ScreenWidth, ScreenHeight / 2 - iH, 0, 0, 0, 255);

		/*gEngfuncs.pfnFillRGBABlend(ScreenWidth/2,0,1,ScreenHeight,0,0,0,255);
		gEngfuncs.pfnFillRGBABlend(0,ScreenHeight/2+1,ScreenWidth,1,0,0,0,255);*/
		return 0/*g_pfnHudSniperScope_Draw(p,i,f)*/;
	}
	if (pCvar_DrawScope->value != 0 && g_iBTEWeapon != WPN_SFSNIPER && g_iBTEWeapon != WPN_DESTROYER && g_iBTEWeapon != WPN_BUFFAWP && 0.0 < Hud().m_FOV && Hud().m_FOV <= 40.0)
	{
		//LogToFile("BreakPoint");
		int tid;
		float scale;
		float x, y, w, h;

		if (g_iBTEWeapon == WPN_M200)
			tid = Hud().m_TGA.GetTGA("other\\scope_m200");
		else
			tid = Hud().m_TGA.GetTGA("other\\scope");

		scale = ScreenHeight / 0.85 / g_MHTga[tid].height;

		w = g_MHTga[tid].width * scale;
		h = g_MHTga[tid].height * scale;

		x = ScreenWidth / 2 - w / 2;
		y = ScreenHeight / 2 - h / 2;

		Tri_Enable(GL_TEXTURE_2D);
		Tri_BindTexture(GL_TEXTURE_2D, g_MHTga[tid].texid);
		if (g_iVideoMode)
		{
			glColor4ub(255, 255, 255, 255);
		}
		else gEngfuncs.pTriAPI->Color4ub(255, 255, 255, 255);
		Tri_Enable(GL_BLEND);
		Tri_Enable(GL_ALPHA_TEST);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		Tri_AlphaFunc(GL_GREATER, 0.0);
		gEngfuncs.pTriAPI->Begin(TRI_QUADS);
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(x, y + h, 0);
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(x + w, y + h, 0);
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(x + w, y, 0);
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(x, y, 0);
		gEngfuncs.pTriAPI->End();

		int LENGTH_SCOPE = w / 2;

		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + LENGTH_SCOPE, 0, ScreenWidth / 2 - LENGTH_SCOPE, ScreenHeight, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE, 0, 0, 0, 255);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + LENGTH_SCOPE, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE, 0, 0, 0, 255);

		// Don't know why
		gEngfuncs.pfnDrawConsoleString(-10, -10, "!");

		return 0;
	}

	if (g_iBTEWeapon == WPN_SFSNIPER)
	{
		HudSfsniperScope().Draw(f);

		return 0;
	}

	if (g_iBTEWeapon == WPN_DESTROYER)
	{
		HudDestroyerSniperScope().Draw(f);

		return 0;
	}

	if (g_iBTEWeapon == WPN_BUFFAWP)
	{
		HudBuffAWPSniperScope().Draw(f);
	}

	return g_pfnHudSniperScope_Draw(p, i, f);
}

void ClientLinks_InstallHook()
{
	g_pMetaHookAPI->InlineHook(g_pfnCHudRoundTimer_Draw, CHudRoundTimer_Draw, (void *&)g_pfnCHudRoundTimer_Draw);
	g_pMetaHookAPI->InlineHook(g_pfnCHudAmmo_DrawCrosshair, CHudAmmo_DrawCrosshair, (void *&)g_pfnCHudAmmo_DrawCrosshair);
	g_pMetaHookAPI->InlineHook(g_pfnCHudSayText_Draw, CHudSayText_Draw, (void *&)g_pfnCHudSayText_Draw);
	g_pMetaHookAPI->InlineHook(g_pfnHudSniperScope_Draw, HudSniperScope_Draw, (void *&)g_pfnHudSniperScope_Draw);

}