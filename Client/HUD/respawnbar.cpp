#include "base.h"
#include "hud.h"
#include "DrawTargaImage.h"
#include "respawnbar.h"

static CHudRespawnBar g_HudRespawnBar;
CHudRespawnBar &HudRespawnBar()
{
	return g_HudRespawnBar;
}

void CHudRespawnBar::Init(void)
{
	m_iFlags |= HUD_ACTIVE;
}

void CHudRespawnBar::VidInit(void)
{
	m_bOn = FALSE;
	m_iBG = Hud().m_TGA.FindTexture("resource\\hud\\other\\respawn_bg");
	m_iBar = Hud().GetSpriteIndex("respawnbar");
}

void CHudRespawnBar::Draw(float time)
{
	if(!m_bOn) return;
	if(m_flShowTime<=time) return;

	//Calculate Position Wide=432,High=72
	int iStartX,iStartY;
	iStartX = (int)(ScreenWidth - 432)*0.5;
	iStartY = (int)ScreenHeight*0.7;

	GL_DrawTGA(g_Texture[m_iBG].iTexture,255,255,255,255,iStartX,iStartY,1);
	//Draw Bar 419,9
	HSPRITE hBar;
	hBar =  Hud().GetSprite(m_iBar);
	gEngfuncs.pfnSPR_Set(hBar, 255, 255, 255);
	//Calc Width
	wrect_t wrecSpr = Hud().GetSpriteRect(m_iBar);
	wrecSpr.right = (int)419*(time-m_flStartTime)/(m_flShowTime-m_flStartTime);
	iStartX+=8;
	iStartY+=24;
	gEngfuncs.pfnSPR_Draw(0, iStartX,iStartY, &wrecSpr);

}
