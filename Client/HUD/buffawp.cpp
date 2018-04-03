
#include "hud.h"
#include "DrawTargaImage.h"
#include "bink/bink.h"
#include "buffawp.h"

#include "Client/HUD/DrawTGA.h"

static CHudBuffAWPSniperScope g_HudBuffAWPSniperScope;
CHudBuffAWPSniperScope &HudBuffAWPSniperScope()
{
	return g_HudBuffAWPSniperScope;
}

void CHudBuffAWPSniperScope::Init(void)
{
	m_flStartTime = 0.0;
}

void CHudBuffAWPSniperScope::VidInit(void)
{
	m_iTgaBG = Hud().m_TGA.FindTexture("sprites\\buffawp_bg");
	m_iTgaBar = Hud().m_TGA.FindTexture("sprites\\buffawp_bar");
	m_iTgaLight[0] = Hud().m_TGA.FindTexture("sprites\\buffawp_light_1");
	m_iTgaLight[1] = Hud().m_TGA.FindTexture("sprites\\buffawp_light_2");
	m_iTgaLight[2] = Hud().m_TGA.FindTexture("sprites\\buffawp_light_3");
	m_iTexture = Hud().m_TGA.FindTexture("sprites\\buffawp_outline");
}

void CHudBuffAWPSniperScope::Run(void)
{
	if (m_flStartTime > 0.0)
		return;
	m_flStartTime = cl.time;
}

void CHudBuffAWPSniperScope::End(void)
{
	m_flStartTime = 0.0;
}

void CHudBuffAWPSniperScope::Draw(float flTime)
{
	if (!m_flStartTime)
		return;

	int iH = ScreenHeight / 588.0f * 256.0f;

	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, ScreenWidth / 2 - iH, ScreenHeight / 2 - iH, iH, iH, 255);
	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, ScreenWidth / 2 - iH, ScreenHeight / 2 + iH, iH, -iH, 255);
	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, ScreenWidth / 2 + iH, ScreenHeight / 2 - iH, -iH, iH, 255);
	GL_DrawTGA2(g_Texture[m_iTexture].iTexture, ScreenWidth / 2 + iH, ScreenHeight / 2 + iH, -iH, -iH, 255);


	GL_DrawTGA2(g_Texture[m_iTgaBG].iTexture, ScreenWidth / 2 - g_Texture[m_iTgaBG].iWidth / 2, ScreenHeight / 4 * 3, g_Texture[m_iTgaBG].iWidth, g_Texture[m_iTgaBG].iHeight, 255);
	int iCharged = (cl.time - m_flStartTime) / 0.8;
	float flChargingPercent = (cl.time - m_flStartTime - iCharged * 0.8) / 0.8;
	if (iCharged >= 3)
	{
		iCharged = 3;
		flChargingPercent = 0;
	}
	
	for (int i = 0; i < 3; i++)
	{
		if (iCharged > i)
			GL_DrawTGA2(g_Texture[m_iTgaLight[i]].iTexture, ScreenWidth / 2 - g_Texture[m_iTgaLight[i]].iWidth / 2, ScreenHeight / 4 * 3, g_Texture[m_iTgaLight[i]].iWidth, g_Texture[m_iTgaLight[i]].iHeight, 255);
		else
			break;
	}
	
	GL_DrawTGA2(g_Texture[m_iTgaBar].iTexture, ScreenWidth / 2 - g_Texture[m_iTgaBar].iWidth / 2, ScreenHeight / 4 * 3 + 24, g_Texture[m_iTgaBar].iWidth * flChargingPercent, g_Texture[m_iTgaBar].iHeight, 255);
}