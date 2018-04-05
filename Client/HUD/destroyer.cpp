#include "metahook.h"
#include "triangleapi.h"
#include "gl/gl.h"
#include "exportfuncs.h"
#include "hud.h"

#include "pmtrace.h"
#include "pm_defs.h"

#include "event_api.h"
#include "TriAPI.h"
#include "DrawTargaImage.h"
#include "destroyer.h"
#include "bink/bink.h"
#include "BaseUI.h"

#include "Client/HUD/DrawTGA.h"

static CHudDestroyerSniperScope g_HudDestroyerSniperScope;
CHudDestroyerSniperScope &HudDestroyerSniperScope()
{
	return g_HudDestroyerSniperScope;
}

void CHudDestroyerSniperScope::Init(void)
{
	m_hBink = BinkOpen("cstrike\\sprites\\destroyer_aim01.bik", 135266304);
	if (m_hBink)
	{
		m_pMem = (BYTE *)malloc(m_hBink->Width * m_hBink->Height * 4);
		m_iBink = vgui::surface()->CreateNewTextureID();
	}
	m_bStartBink = FALSE;
	m_flStartTime = 0.0;
}

void CHudDestroyerSniperScope::VidInit(void)
{
	m_iAim01 = Hud().m_TGA.FindTexture("sprites\\destroyer_aim01");
	m_iAim02 = Hud().m_TGA.FindTexture("sprites\\destroyer_aim02");
	m_iFrame01 = Hud().m_TGA.FindTexture("sprites\\destroyer_frame01");
	m_iFrame02 = Hud().m_TGA.FindTexture("sprites\\destroyer_frame02");
	m_iRange = Hud().m_TGA.FindTexture("sprites\\destroyer_range");
	m_iNumberZero = Hud().GetSpriteIndex("destroyer_count0");
	m_iMeter = Hud().GetSpriteIndex("destroyer_countM");
	m_rcMeter = Hud().GetSpriteRect(m_iMeter);
}

int CHudDestroyerSniperScope::DrawDestroyerSniperScopeNumbers(int x, int y, int iFlags, int iNumber, int r, int g, int b)
{
	int k;
	int iWidth = Hud().GetSpriteRect(m_iNumberZero).right - Hud().GetSpriteRect(m_iNumberZero).left;

	if (iNumber > 0)
	{
		if (iNumber >= 10000)
		{
			k = iNumber / 10000;
			gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iNumberZero + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iNumberZero + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS))
			x += iWidth;

		if (iNumber >= 1000)
		{
			k = (iNumber % 10000) / 1000;
			gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iNumberZero + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iNumberZero + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
			x += iWidth;

		if (iNumber >= 100)
		{
			k = (iNumber % 1000) / 100;
			gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iNumberZero + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iNumberZero + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
			x += iWidth;

		if (iNumber >= 10)
		{
			k = (iNumber % 100) / 10;
			gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iNumberZero + k), r, g, b);
			gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iNumberZero + k));
			x += iWidth;
		}
		else if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
			x += iWidth;

		k = iNumber % 10;
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iNumberZero + k), r, g, b);
		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iNumberZero + k));
		x += iWidth;
	}
	else if (iFlags & DHN_DRAWZERO)
	{
		gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iNumberZero), r, g, b);

		if (iFlags & (DHN_5DIGITS))
			x += iWidth;

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS))
			x += iWidth;

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS))
			x += iWidth;

		if (iFlags & (DHN_5DIGITS | DHN_4DIGITS | DHN_3DIGITS | DHN_2DIGITS))
			x += iWidth;

		gEngfuncs.pfnSPR_DrawAdditive(0, x, y, &Hud().GetSpriteRect(m_iNumberZero));
		x += iWidth;
	}
	return x;
}

void CHudDestroyerSniperScope::SetCharge(BOOL bAimEnemy)
{
	m_iTexture = bAimEnemy ? m_iFrame02 : m_iFrame01;
	if (bAimEnemy && m_bStartBink != TRUE && m_iCurrentAim != m_iAim02)
	{
		m_flStartTime = cl.time;
		m_iFrame = 1;
		m_bStartBink = TRUE;
	}
	else if (!bAimEnemy)
	{
		m_iCurrentAim = m_iAim01;
		m_bStartBink = FALSE;
	}
}

void CHudDestroyerSniperScope::Finish(void)
{
	m_iCurrentAim = m_iAim02;
	m_bStartBink = FALSE;
}

CHudDestroyerSniperScope::~CHudDestroyerSniperScope()
{
	if (m_pMem)
		free(m_pMem);
}

int CHudDestroyerSniperScope::CalculateDistance(void)
{
	if (!gEngfuncs.GetLocalPlayer())
		return 0;
	vec3_t forward;
	pmtrace_t tr;
	gEngfuncs.pfnAngleVectors(v_angles, forward, NULL, NULL);
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction(false, true);
	gEngfuncs.pEventAPI->EV_PushPMStates();
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(gEngfuncs.GetLocalPlayer()->index - 1);
	gEngfuncs.pEventAPI->EV_SetTraceHull(2);
	gEngfuncs.pEventAPI->EV_PlayerTrace(g_vecEyePos, g_vecEyePos + forward * 8192, PM_NORMAL, -1, &tr);
	gEngfuncs.pEventAPI->EV_PopPMStates();

	int iEntity = gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr);

	if (iEntity > 0 && iEntity <= gEngfuncs.GetMaxClients() && vPlayer[iEntity].team != vPlayer[gEngfuncs.GetLocalPlayer()->index].team)
		SetCharge(TRUE);
	else
		SetCharge(FALSE);

	return int((tr.endpos - g_vecEyePos).Length() / 39.37f);
}

void CHudDestroyerSniperScope::Run(void)
{
	SetCharge(FALSE);
	m_flStartTime = cl.time;
}

void CHudDestroyerSniperScope::End(void)
{
	m_flStartTime = 0.0;
}

void CHudDestroyerSniperScope::Draw(float flTime)
{
	if (!m_flStartTime || !m_hBink)
		return;

	float LENGTH_SCOPE = min(ScreenWidth, ScreenHeight) / 2 / 0.75;

	if (m_iTexture == m_iFrame01)
	{
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - LENGTH_SCOPE + 1, ScreenHeight, 0, 12, 32, 238);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + LENGTH_SCOPE - 1, 0, ScreenWidth / 2 - LENGTH_SCOPE + 10, ScreenHeight, 0, 12, 32, 238);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE + 1, 0, 12, 32, 238);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + LENGTH_SCOPE, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE + 10, 0, 12, 32, 238);
	}
	else
	{
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth / 2 - LENGTH_SCOPE + 1, ScreenHeight, 5, 21, 31, 238);
		gEngfuncs.pfnFillRGBABlend(ScreenWidth / 2 + LENGTH_SCOPE - 1, 0, ScreenWidth / 2 - LENGTH_SCOPE + 10, ScreenHeight, 5, 21, 31, 238);
		gEngfuncs.pfnFillRGBABlend(0, 0, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE + 1, 5, 21, 31, 238);
		gEngfuncs.pfnFillRGBABlend(0, ScreenHeight / 2 + LENGTH_SCOPE, ScreenWidth, ScreenHeight / 2 - LENGTH_SCOPE + 10, 5, 21, 31, 238);
	}
	int iX, iY, iWidth, iHeight;

	//iWidth = g_Texture[m_iTexture].iWidth;
	//iHeight = g_Texture[m_iTexture].iHeight;

	iWidth = iHeight = min(ScreenWidth, ScreenHeight) / 2 / 0.75;

	iX = ScreenWidth / 2 - iWidth;
	iY = ScreenHeight / 2 - iHeight;

	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, iX, iY, iWidth, iHeight, 255);
	iY = ScreenHeight / 2 + iHeight;
	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, iX, iY, iWidth, -iHeight, 255);
	iX = ScreenWidth / 2 + iWidth;
	iY = ScreenHeight / 2 - iHeight;
	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, iX, iY, -iWidth, iHeight, 255);
	iY = ScreenHeight / 2 + iHeight;
	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, iX, iY, -iWidth, -iHeight, 255);

	//iX = ScreenWidth / 2 + 130;
	//iY -= 275;
	iX = ScreenWidth / 2 + iWidth / 4;
	iY -= iHeight / 2;
	

	GL_DrawTGA2(g_Texture[m_iRange].iTexture, iX, iY, g_Texture[m_iRange].iWidth, g_Texture[m_iRange].iHeight, 255);
	iY += 13;
	iX += 85;
	iX = DrawDestroyerSniperScopeNumbers(iX, iY, DHN_3DIGITS | DHN_DRAWZERO, CalculateDistance(), 255, 255, 255);
	gEngfuncs.pfnSPR_Set(Hud().GetSprite(m_iMeter), 255, 255, 255);
	gEngfuncs.pfnSPR_DrawAdditive(0, iX, iY, &m_rcMeter);

	if (m_bStartBink)
	{
		if (flTime - m_flStartTime > (float(m_hBink->FrameRateDiv) / float(m_hBink->FrameRate) / 50))
		{
			m_iFrame += 3;
			m_flStartTime = flTime;
			if (m_iFrame >= m_hBink->Frames)
			{
				Finish();
			}
			BinkGoto(m_hBink, m_iFrame, 0);
			BinkDoFrame(m_hBink);
			BinkCopyToBuffer(m_hBink, m_pMem, m_hBink->Width * 4, m_hBink->Height, 0, 0, BINKSURFACE32RA);
			vgui::surface()->DrawSetTextureRGBA(m_iBink, m_pMem, m_hBink->Width, m_hBink->Height, 1, true);
		}

		iX = ScreenWidth / 2 - m_hBink->Width / 2;
		iY = ScreenHeight / 2 - m_hBink->Height / 2;

		Tri_Enable(GL_TEXTURE_2D);
		Tri_Enable(GL_BLEND);
		Tri_BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		vgui::surface()->DrawSetTexture(m_iBink);
		vgui::surface()->DrawSetColor(255, 255, 255, 255);
		vgui::surface()->DrawTexturedRect(iX, iY, iX + m_hBink->Width, iY + m_hBink->Height);
	}
	else
	{
		GL_DrawTGA2(g_Texture[m_iCurrentAim].iTexture, ScreenWidth / 2 - g_Texture[m_iCurrentAim].iWidth / 2, ScreenHeight / 2 - g_Texture[m_iCurrentAim].iHeight / 2, g_Texture[m_iCurrentAim].iWidth, g_Texture[m_iCurrentAim].iHeight, 255);
	}
}